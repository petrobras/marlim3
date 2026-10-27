"""Lock policy, artifact integrity and real uv mirror-install regression tests."""

import argparse
import functools
import http.server
import io
import json
import shutil
import stat
import subprocess
import threading
import zipfile
from pathlib import Path

import pytest

from scripts import dependencies as deps


HEAD = "a" * 40
HEAD_REPO = "contributor/marlim3"


def wheel_bytes(comment=b""):
    stream = io.BytesIO()
    with zipfile.ZipFile(stream, "w", zipfile.ZIP_DEFLATED) as archive:
        archive.comment = comment
        files = {
            "portability_demo/__init__.py": '__version__ = "1.0.0"\n',
            "portability_demo-1.0.0.dist-info/METADATA": (
                "Metadata-Version: 2.1\nName: portability-demo\n"
                "Version: 1.0.0\nRequires-Python: >=3.12\n"
            ),
            "portability_demo-1.0.0.dist-info/WHEEL": (
                "Wheel-Version: 1.0\nGenerator: tests\n"
                "Root-Is-Purelib: true\nTag: py3-none-any\n"
            ),
            "portability_demo-1.0.0.dist-info/RECORD": "",
        }
        for name, content in files.items():
            info = zipfile.ZipInfo(name, date_time=(2020, 1, 1, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            archive.writestr(info, content)
    return stream.getvalue()


@pytest.fixture
def project(tmp_path):
    root = tmp_path / "project"
    root.mkdir()
    (root / "pyproject.toml").write_text(
        '[project]\nname = "fixture"\nversion = "0.0.0"\n'
        'requires-python = ">=3.12,<3.13"\ndependencies = ["portability-demo==1.0.0"]\n'
        f'[tool.uv]\nrequired-version = "=={deps.UV_VERSION}"\n'
    )
    wheel = wheel_bytes()
    (root / "uv.lock").write_text(
        'version = 1\nrevision = 3\nrequires-python = ">=3.12,<3.13"\n'
        '[[package]]\nname = "fixture"\nversion = "0.0.0"\n'
        'source = { virtual = "." }\ndependencies = [{ name = "portability-demo" }]\n'
        '[package.metadata]\nrequires-dist = [{ name = "portability-demo", specifier = "==1.0.0" }]\n'
        '[[package]]\nname = "portability-demo"\nversion = "1.0.0"\n'
        'source = { registry = "https://pypi.org/simple" }\n'
        'wheels = [{ url = "https://files.pythonhosted.org/packages/test/portability_demo-1.0.0-py3-none-any.whl", '
        f'hash = "sha256:{deps.digest(wheel)}", size = {len(wheel)} }}]\n'
    )
    return root


def provenance(root, data):
    return {
        "schema": 1,
        "repository": deps.REPOSITORY,
        "head_repository": HEAD_REPO,
        "head_sha": HEAD,
        "uv_version": deps.UV_VERSION,
        "inputs": deps.input_hashes(root),
        "lock_sha256": deps.digest(data),
    }


def apply(root, data, proof):
    deps.apply_payload(
        root,
        data,
        proof,
        repository=deps.REPOSITORY,
        head_repository=HEAD_REPO,
        head_sha=HEAD,
    )


def test_repository_lock_is_public_and_version_pin_is_consistent():
    root = Path(__file__).resolve().parents[1]
    data = deps.validate_project(root)
    deps.validate_lock(
        (root / "uv.lock").read_bytes(), data["project"]["name"]
    )
    workflow = (root / ".github/workflows/build-test-release.yml").read_text()
    generator = (root / deps.WORKFLOW).read_text()
    readthedocs = (root / ".readthedocs.yaml").read_text()
    assert data["tool"]["uv"]["required-version"] == f"=={deps.UV_VERSION}"
    assert f'"uv=={deps.UV_VERSION}"' in workflow
    assert f'version: "{deps.UV_VERSION}"' in generator
    assert f'"uv=={deps.UV_VERSION}"' in readthedocs


@pytest.mark.parametrize(
    "old,new",
    [
        ("https://pypi.org/simple", "https://internal.invalid/simple"),
        (
            "https://files.pythonhosted.org/packages/",
            "https://internal.invalid/packages/",
        ),
        (
            "https://files.pythonhosted.org/packages/",
            "https://files.pythonhosted.org.evil.invalid/packages/",
        ),
        (
            "https://files.pythonhosted.org/packages/",
            "https://user:secret@files.pythonhosted.org/packages/",
        ),
        (
            "https://files.pythonhosted.org/packages/",
            "http://files.pythonhosted.org/packages/",
        ),
        ("sha256:", "md5:"),
        ('source = { virtual = "." }', 'source = { virtual = "../outside" }'),
    ],
)
def test_policy_rejects_nonportable_sources_and_artifacts(project, old, new):
    data = (project / "uv.lock").read_text().replace(old, new).encode()
    with pytest.raises(deps.DependencyError):
        deps.validate_lock(data, "fixture")


def test_policy_checks_artifacts_of_unselected_packages(project):
    data = (
        (project / "uv.lock").read_text()
        + '\n[[package]]\nname = "desktop-only"\nversion = "1"\nsource = { registry = "https://internal.invalid/simple" }\n'
    )
    with pytest.raises(deps.DependencyError):
        deps.validate_lock(data.encode(), "fixture")


@pytest.mark.parametrize(
    "configuration",
    [
        '\n[tool.uv.sources]\nportability-demo = { index = "private" }\n',
        '\n[[tool.uv.index]]\nurl = "https://internal.invalid/simple"\n',
    ],
)
def test_project_cannot_select_a_corporate_source(project, configuration):
    path = project / "pyproject.toml"
    path.write_text(path.read_text() + configuration)
    with pytest.raises(deps.DependencyError):
        deps.validate_project(project)


def test_project_rejects_direct_urls_before_resolution(project):
    path = project / "pyproject.toml"
    path.write_text(
        path.read_text().replace(
            "portability-demo==1.0.0",
            "portability-demo @ https://private.invalid/demo.whl",
        )
    )
    with pytest.raises(deps.DependencyError):
        deps.validate_project(project)


def test_index_and_hash_override_environment_is_isolated(monkeypatch):
    for name in (
        "UV_INDEX",
        "UV_EXTRA_INDEX_URL",
        "UV_DEFAULT_INDEX",
        "UV_FIND_LINKS",
        "UV_NO_VERIFY_HASHES",
        "PIP_INDEX_URL",
        "UV_CONFIG_FILE",
    ):
        monkeypatch.setenv(name, "unexpected")
    monkeypatch.setenv("UV_INDEX_JFROG_PASSWORD", "test-secret")
    monkeypatch.setenv("SSL_CERT_FILE", "approved-ca.pem")
    public = deps.uv_env()
    corporate = deps.uv_env(corporate=True)
    assert "UV_INDEX_JFROG_PASSWORD" not in public
    assert corporate["UV_INDEX_JFROG_PASSWORD"] == "test-secret"
    assert corporate["SSL_CERT_FILE"] == "approved-ca.pem"
    assert (
        "UV_INDEX" not in corporate and "UV_NO_VERIFY_HASHES" not in corporate
    )
    assert "UV_DEFAULT_INDEX" not in public and "PIP_INDEX_URL" not in public


@pytest.mark.parametrize(
    "index",
    [
        "",
        "http://private.invalid/simple",
        "https://u:p@private.invalid/simple",
        "https://private.invalid/simple?token=secret",
    ],
)
def test_corporate_index_requires_transport_and_separate_credentials(
    index, monkeypatch
):
    monkeypatch.delenv("MARLIM_JFROG_INDEX", raising=False)
    with pytest.raises(deps.DependencyError):
        deps.index_for("corporate", index)


@pytest.mark.parametrize(
    "spec",
    [
        "--index=bad",
        "package @ https://bad.invalid",
        "../package",
        "name; rm -rf .",
        "name>=1",
    ],
)
def test_upgrade_requests_are_data_not_command_options(project, spec):
    (project / deps.REQUEST_FILE).write_text(
        "schema = 1\nupgrade = [" + json.dumps(spec) + "]\n"
    )
    with pytest.raises(deps.DependencyError):
        deps.read_request(project)


def test_pending_upgrade_blocks_installation_even_with_current_lock(project):
    (project / deps.REQUEST_FILE).write_text(
        'schema = 1\nupgrade = ["portability-demo==1.0.0"]\n'
    )
    with pytest.raises(deps.DependencyError, match="pending"):
        deps.check_public(project)


def test_failed_preflight_preserves_both_official_files(project, monkeypatch):
    original = deps.input_hashes(project)

    def fail_lock(args, root, **kwargs):
        if args[0] == "add":
            path = root / "pyproject.toml"
            path.write_text(
                path.read_text().replace(
                    "portability-demo==1.0.0", "portability-demo==2.0.0"
                )
            )
            return ""
        raise deps.DependencyError("Package is unavailable in mirror")

    monkeypatch.setattr(deps, "uv", fail_lock)
    args = argparse.Namespace(
        action="add",
        packages=["portability-demo==2.0.0"],
        dependency_group=None,
        mode="corporate",
        index="https://private.invalid/simple",
    )
    with pytest.raises(deps.DependencyError):
        deps.change_dependencies(args, project)
    assert deps.input_hashes(project) == original


def test_local_add_changes_manifest_without_copying_temporary_registry(
    project, monkeypatch
):
    original_lock = (project / "uv.lock").read_bytes()

    def resolve(args, root, **kwargs):
        if args[0] == "add":
            path = root / "pyproject.toml"
            path.write_text(
                path.read_text().replace(
                    "portability-demo==1.0.0", "portability-demo==2.0.0"
                )
            )
        else:
            path = root / "uv.lock"
            path.write_text(
                path.read_text().replace(
                    "https://pypi.org/simple", "https://private.invalid/simple"
                )
            )
        return ""

    monkeypatch.setattr(deps, "uv", resolve)
    args = argparse.Namespace(
        action="add",
        packages=["portability-demo==2.0.0"],
        dependency_group=None,
        mode="corporate",
        index="https://private.invalid/simple",
    )
    deps.change_dependencies(args, project)
    assert (project / "uv.lock").read_bytes() == original_lock
    assert "2.0.0" in (project / "pyproject.toml").read_text()
    assert "private.invalid" not in (project / "pyproject.toml").read_text()


def test_generator_preserves_checkout_and_exposes_stale_lock_as_artifact(
    project, tmp_path, monkeypatch
):
    original = deps.input_hashes(project)
    monkeypatch.setattr(deps, "git", lambda *args: HEAD)

    def resolve(args, root, **kwargs):
        if args[0] == "lock" and "--check" not in args:
            path = root / "uv.lock"
            path.write_text(path.read_text() + "\n# regenerated\n")
        return ""

    monkeypatch.setattr(deps, "uv", resolve)
    output = tmp_path / "artifact"
    args = argparse.Namespace(
        output=output,
        repository=deps.REPOSITORY,
        head_repository=HEAD_REPO,
        head_sha=HEAD,
        base_sha=None,
    )
    deps.generate_lock(args, project)
    assert deps.input_hashes(project) == original
    assert (output / "uv.lock").exists()
    assert json.loads((output / "provenance.json").read_text())["changed"]
    with pytest.raises(deps.DependencyError, match="Apply"):
        deps.check_generated(project, output)


@pytest.mark.parametrize(
    "field,value",
    [
        ("head_sha", "b" * 40),
        ("repository", "other/repository"),
        ("head_repository", "other/fork"),
        ("uv_version", "0.11.23"),
        ("lock_sha256", "0" * 64),
    ],
)
def test_artifact_cannot_overwrite_another_checkout(
    project, monkeypatch, field, value
):
    data = (project / "uv.lock").read_bytes()
    proof = provenance(project, data)
    proof[field] = value
    with pytest.raises(deps.DependencyError, match="provenance"):
        apply(project, data, proof)
    assert (project / "uv.lock").read_bytes() == data


def test_artifact_rejects_manifest_changed_during_download(project):
    data = (project / "uv.lock").read_bytes()
    proof = provenance(project, data)
    path = project / "pyproject.toml"
    path.write_text(path.read_text() + "\n# another dependency edit\n")
    with pytest.raises(deps.DependencyError, match="provenance"):
        apply(project, data, proof)


def test_apply_consumes_request_only_after_validation(project, monkeypatch):
    data = (project / "uv.lock").read_bytes()
    (project / deps.REQUEST_FILE).write_text(
        'schema = 1\nupgrade = ["portability-demo==1.0.0"]\n'
    )
    proof = provenance(project, data)
    monkeypatch.setattr(deps, "git", lambda *args: HEAD)

    def fail(*args, **kwargs):
        raise deps.DependencyError("Lock does not match manifest")

    monkeypatch.setattr(deps, "uv", fail)
    with pytest.raises(deps.DependencyError):
        apply(project, data, proof)
    assert (project / deps.REQUEST_FILE).exists()
    monkeypatch.setattr(deps, "uv", lambda *args, **kwargs: "")
    apply(project, data, proof)
    assert not (project / deps.REQUEST_FILE).exists()


@pytest.mark.parametrize("kind", ["traversal", "duplicate", "symlink"])
def test_artifact_archive_is_not_extracted_to_arbitrary_paths(tmp_path, kind):
    path = tmp_path / "artifact.zip"
    with zipfile.ZipFile(path, "w") as archive:
        archive.writestr("uv.lock", "lock")
        archive.writestr("provenance.json", "{}")
        if kind == "traversal":
            archive.writestr("../summary.md", "outside")
        elif kind == "duplicate":
            archive.writestr("summary.md", "summary")
            with pytest.warns(UserWarning, match="Duplicate"):
                archive.writestr("uv.lock", "duplicate")
        else:
            member = zipfile.ZipInfo("summary.md")
            member.create_system = 3
            member.external_attr = (stat.S_IFLNK | 0o777) << 16
            archive.writestr(member, "../outside")
    with pytest.raises(deps.DependencyError):
        deps.artifact_payload(path)
    assert not (tmp_path.parent / "summary.md").exists()


def test_prepared_run_rejects_stale_inputs(project):
    state = project / deps.STATE_DIR
    state.mkdir()
    (state / "environment.json").write_text(json.dumps({"inputs": {}}))
    with pytest.raises(deps.DependencyError, match="changed"):
        deps.run_prepared(
            argparse.Namespace(preview=False, command=["python"]), project
        )


def test_failed_reinstall_invalidates_previous_readiness(project, monkeypatch):
    state = project / deps.STATE_DIR / "environment.json"
    state.parent.mkdir()
    state.write_text(json.dumps({"inputs": deps.input_hashes(project)}))

    def fail_install(args, root, **kwargs):
        if args[:2] == ["pip", "sync"]:
            raise deps.DependencyError("Mirror unavailable")
        return ""

    monkeypatch.setattr(deps, "uv", fail_install)
    with pytest.raises(deps.DependencyError, match="Mirror unavailable"):
        deps.sync_environment(
            sync_args("https://private.invalid/simple"), project
        )
    assert not state.exists()


def test_apply_lock_downloads_only_the_matching_failed_generation_run(
    project, monkeypatch
):
    data = (project / "uv.lock").read_bytes() + b"\n# public result\n"
    proof = provenance(project, data)
    buffer = io.BytesIO()
    with zipfile.ZipFile(buffer, "w") as archive:
        archive.writestr("uv.lock", data)
        archive.writestr("provenance.json", json.dumps(proof))
        archive.writestr("summary.md", "Public result")

    def git(root, *args):
        if args[0] == "branch":
            return "feature"
        if args[0] == "remote":
            return f"git@github.com:{HEAD_REPO}.git"
        return HEAD

    def api(args, **kwargs):
        if args[0] == "uv":
            return ""
        if args[-1].endswith("/artifacts?per_page=100"):
            return json.dumps(
                {
                    "artifacts": [
                        {
                            "id": 123,
                            "name": f"public-uv-lock-{HEAD}",
                            "size_in_bytes": len(buffer.getvalue()),
                            "expired": False,
                        }
                    ]
                }
            )
        return json.dumps(
            {
                "head_sha": HEAD,
                "head_repository": {"full_name": HEAD_REPO},
                "path": deps.WORKFLOW,
                "status": "completed",
                "conclusion": "failure",
                "event": "pull_request",
            }
        )

    downloads = []

    def download(args, **kwargs):
        downloads.append(args)
        kwargs["stdout"].write(buffer.getvalue())
        return subprocess.CompletedProcess(args, 0, stderr=b"")

    monkeypatch.setattr(deps, "git", git)
    monkeypatch.setattr(deps, "command", api)
    monkeypatch.setattr(deps.subprocess, "run", download)
    deps.apply_lock(
        argparse.Namespace(repository=deps.REPOSITORY, run=456), project
    )
    assert downloads == [
        ["gh", "api", f"repos/{deps.REPOSITORY}/actions/artifacts/123/zip"]
    ]
    assert (project / "uv.lock").read_bytes() == data


@pytest.mark.parametrize("branch", ["main", "develop", ""])
def test_apply_lock_refuses_default_and_detached_branches(
    project, monkeypatch, branch
):
    monkeypatch.setattr(deps, "git", lambda *args: branch)
    with pytest.raises(deps.DependencyError, match="feature branch"):
        deps.apply_lock(
            argparse.Namespace(repository=deps.REPOSITORY, run=456), project
        )


def pinned_uv_available():
    if not shutil.which("uv"):
        return False
    result = subprocess.run(
        ["uv", "--version"], capture_output=True, text=True, check=False
    )
    return result.stdout.startswith(f"uv {deps.UV_VERSION} ")


@pytest.fixture
def mirror(tmp_path, monkeypatch):
    directory = tmp_path / "mirror"
    filename = "portability_demo-1.0.0-py3-none-any.whl"
    package = directory / "packages" / filename
    package.parent.mkdir(parents=True)
    package.write_bytes(wheel_bytes())
    page = directory / "simple/portability-demo/index.html"
    page.parent.mkdir(parents=True)
    page.write_text(
        f'<a href="/packages/{filename}#sha256={deps.digest(package.read_bytes())}">{filename}</a>'
    )
    requests, blocked = [], []

    class Handler(http.server.SimpleHTTPRequestHandler):
        def log_message(self, *args):
            pass

        def do_GET(self):
            requests.append(self.path)
            return super().do_GET()

    class Proxy(http.server.BaseHTTPRequestHandler):
        def log_message(self, *args):
            pass

        def do_CONNECT(self):
            blocked.append(self.path)
            self.send_error(502, "Public access blocked")

    server = http.server.ThreadingHTTPServer(
        ("127.0.0.1", 0), functools.partial(Handler, directory=str(directory))
    )
    proxy = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Proxy)
    for service in (server, proxy):
        threading.Thread(target=service.serve_forever, daemon=True).start()
    monkeypatch.setenv("HTTPS_PROXY", f"http://127.0.0.1:{proxy.server_port}")
    monkeypatch.setenv("NO_PROXY", "127.0.0.1,localhost")
    monkeypatch.setenv("UV_CACHE_DIR", str(tmp_path / "cache"))
    monkeypatch.setenv("UV_HTTP_RETRIES", "0")
    monkeypatch.setenv("UV_INDEX", "http://127.0.0.1:1/simple")
    monkeypatch.setenv("UV_NO_VERIFY_HASHES", "1")
    try:
        yield (
            f"http://127.0.0.1:{server.server_port}/simple",
            package,
            page,
            requests,
            blocked,
        )
    finally:
        for service in (server, proxy):
            service.shutdown()
            service.server_close()


def sync_args(index):
    return argparse.Namespace(
        mode="corporate",
        index=index,
        group=[],
        only_group=[],
        no_dev=True,
        no_install_project=True,
        environment=None,
        inexact=False,
    )


@pytest.mark.skipif(
    not pinned_uv_available(), reason="Requires the project's pinned uv binary"
)
def test_real_uv_installs_public_lock_exclusively_from_mirror(project, mirror):
    index, package, page, requests, blocked = mirror
    original = (project / "uv.lock").read_bytes()
    deps.sync_environment(sync_args(index), project)
    result = subprocess.run(
        [
            str(deps.environment_python(project / ".venv")),
            "-c",
            "import portability_demo; print(portability_demo.__version__)",
        ],
        capture_output=True,
        text=True,
        check=True,
    )
    assert result.stdout.strip() == "1.0.0"
    assert (project / "uv.lock").read_bytes() == original
    assert any(path.endswith(".whl") for path in requests)
    assert blocked == []


@pytest.mark.skipif(
    not pinned_uv_available(), reason="Requires the project's pinned uv binary"
)
def test_real_uv_does_not_adopt_unverified_existing_package_bytes(
    project, mirror
):
    index, package, page, requests, blocked = mirror
    deps.sync_environment(sync_args(index), project)
    python = deps.environment_python(project / ".venv")
    result = subprocess.run(
        [
            str(python),
            "-c",
            "import portability_demo; print(portability_demo.__file__)",
        ],
        capture_output=True,
        text=True,
        check=True,
    )
    module = Path(result.stdout.strip())
    module.write_text('__version__ = "unverified"\n')
    deps.sync_environment(sync_args(index), project)
    assert '__version__ = "1.0.0"' in module.read_text()
    assert blocked == []


@pytest.mark.skipif(
    not pinned_uv_available(), reason="Requires the project's pinned uv binary"
)
@pytest.mark.parametrize("failure", ["hash", "missing"])
def test_real_uv_rejects_incompatible_mirror_without_public_fallback(
    project, mirror, failure
):
    index, package, page, requests, blocked = mirror
    original = (project / "uv.lock").read_bytes()
    if failure == "hash":
        package.write_bytes(wheel_bytes(comment=b"Repacked archive"))
    else:
        page.write_text("<html>No approved versions</html>")
    with pytest.raises(
        deps.DependencyError, match="Hash mismatch|unsatisfiable|No solution"
    ):
        deps.sync_environment(sync_args(index), project)
    assert (project / "uv.lock").read_bytes() == original
    assert blocked == []
    assert not (project / deps.STATE_DIR / "environment.json").exists()


@pytest.mark.skipif(
    not pinned_uv_available(), reason="Requires the project's pinned uv binary"
)
def test_real_uv_rejects_stale_public_lock_offline(project, mirror):
    path = project / "pyproject.toml"
    path.write_text(
        path.read_text().replace(
            "portability-demo==1.0.0", "portability-demo==2.0.0"
        )
    )
    with pytest.raises(deps.DependencyError):
        deps.check_public(project)
