#!/usr/bin/env python3
"""Maintain a public uv lock while installing through an optional local mirror.

Requires Python 3.12+ and the project's pinned uv version. No project packages
are needed to run this command. GitHub writes are always left to the author.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import stat
import subprocess
import sys
import tempfile
import tomllib
import zipfile
from contextlib import contextmanager
from pathlib import Path
from urllib.parse import urlsplit


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_INDEX = "https://pypi.org/simple"
UV_VERSION = "0.13.1"
REQUEST_FILE = "dependency-update.toml"
WORKFLOW = ".github/workflows/public-uv-lock.yml"
REPOSITORY = "petrobras/marlim3"
STATE_DIR = ".marlim-dependencies"
MAX_ARTIFACT_SIZE = 20 * 1024 * 1024
PACKAGE_SPEC = re.compile(
    r"[A-Za-z0-9][A-Za-z0-9._-]*(?:==[A-Za-z0-9][A-Za-z0-9.!+_-]*)?\Z"
)
SHA = re.compile(r"[0-9a-f]{40}\Z")
REPO_NAME = re.compile(r"[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+\Z")


class DependencyError(RuntimeError):
    """An actionable failure that must not change the official lock."""


def canonical_name(name: str) -> str:
    return re.sub(r"[-_.]+", "-", name).lower()


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def regular_file(path: Path) -> bytes:
    if path.is_symlink() or not path.is_file():
        raise DependencyError(f"Expected a regular file: {path.name}")
    return path.read_bytes()


def input_hashes(root: Path) -> dict[str, str | None]:
    return {
        name: digest(regular_file(root / name))
        if (root / name).exists()
        else None
        for name in ("pyproject.toml", "uv.lock", REQUEST_FILE)
    }


def atomic_write(path: Path, data: bytes) -> None:
    if path.is_symlink():
        raise DependencyError(
            f"Refusing to overwrite a symbolic link: {path.name}"
        )
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(
        dir=path.parent, prefix=f".{path.name}.", delete=False
    ) as file:
        temporary = Path(file.name)
        file.write(data)
    try:
        mode = stat.S_IMODE(path.stat().st_mode) if path.exists() else 0o644
        temporary.chmod(mode)
        temporary.replace(path)
    finally:
        temporary.unlink(missing_ok=True)


def read_toml(data: bytes, label: str) -> dict:
    try:
        return tomllib.loads(data.decode("utf-8"))
    except (UnicodeError, tomllib.TOMLDecodeError) as error:
        raise DependencyError(f"Invalid {label}: {error}") from error


def project_requirements(data: dict, project: dict) -> list:
    requirements = list(project.get("dependencies", []))
    requirements.extend(data.get("build-system", {}).get("requires", []))
    for group in project.get("optional-dependencies", {}).values():
        requirements.extend(group)
    groups = data.get("dependency-groups", {})
    for group in groups.values():
        for entry in group:
            if isinstance(entry, dict) and set(entry) == {"include-group"}:
                if entry["include-group"] not in groups:
                    raise DependencyError(
                        "A dependency group includes an unknown group."
                    )
            else:
                requirements.append(entry)
    return requirements


def validate_project(root: Path) -> dict:
    data = read_toml(regular_file(root / "pyproject.toml"), "pyproject.toml")
    project = data.get("project", {})
    if not isinstance(project.get("name"), str):
        raise DependencyError(
            "A named, standalone Python project is required."
        )
    if set(project.get("dynamic", [])) - {"version"}:
        raise DependencyError(
            "Dependency metadata must be declared statically in pyproject.toml."
        )
    settings = data.get("tool", {}).get("uv", {})
    if set(settings) - {"required-version"}:
        raise DependencyError(
            "Project uv resolution settings need explicit support; keep mirror configuration local."
        )
    if (
        settings.get("required-version", f"=={UV_VERSION}")
        != f"=={UV_VERSION}"
    ):
        raise DependencyError(f"The project must require uv =={UV_VERSION}.")
    requirements = project_requirements(data, project)
    for requirement in requirements:
        if (
            not isinstance(requirement, str)
            or "@" in requirement
            or "://" in requirement
        ):
            raise DependencyError(
                "Dependencies and build requirements must be public index packages, not URLs or paths."
            )
        if not re.match(
            r"^[A-Za-z0-9][A-Za-z0-9._-]*(?:\[|\s|[<>=!~;]|$)", requirement
        ):
            raise DependencyError(
                "Invalid public index dependency declaration."
            )
    return data


def validate_source(source: dict, name: str, project_name: str) -> bool:
    if not isinstance(source, dict):
        raise DependencyError(f"Package {name!r} has an invalid source.")
    if source in ({"editable": "."}, {"virtual": "."}) and canonical_name(
        name
    ) == canonical_name(project_name):
        return True
    if (
        set(source) == {"registry"}
        and isinstance(source["registry"], str)
        and source["registry"].rstrip("/") == PUBLIC_INDEX
    ):
        return False
    raise DependencyError(
        f"Package {name!r} has a non-public or unsupported source."
    )


def validate_artifacts(package: dict, name: str) -> None:
    wheels = package.get("wheels", [])
    if not isinstance(wheels, list):
        raise DependencyError(f"Package {name!r} has invalid wheels.")
    artifacts = wheels + ([package["sdist"]] if "sdist" in package else [])
    if "registry" in package.get("source", {}) and not artifacts:
        raise DependencyError(f"Package {name!r} has no hashed artifacts.")
    for artifact in artifacts:
        if not isinstance(artifact, dict) or not isinstance(
            artifact.get("url"), str
        ):
            raise DependencyError(
                f"Package {name!r} has invalid artifact metadata."
            )
        url = urlsplit(artifact["url"])
        if (
            url.scheme != "https"
            or url.netloc != "files.pythonhosted.org"
            or not url.path.startswith("/packages/")
            or url.query
            or url.fragment
        ):
            raise DependencyError(
                f"Package {name!r} has a non-public artifact URL."
            )
        if not isinstance(artifact.get("hash"), str) or not re.fullmatch(
            r"sha256:[0-9a-f]{64}", artifact["hash"]
        ):
            raise DependencyError(
                f"Package {name!r} is missing a SHA-256 artifact hash."
            )


def validate_lock(data: bytes, project_name: str) -> dict:
    lock = read_toml(data, "uv.lock")
    packages = lock.get("package")
    if not isinstance(packages, list) or not packages:
        raise DependencyError(
            "The lock must contain project and package entries."
        )
    projects = 0
    for package in packages:
        if not isinstance(package, dict) or not isinstance(
            package.get("name"), str
        ):
            raise DependencyError("Invalid lock package entry.")
        if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]*", package["name"]):
            raise DependencyError("Invalid package name in lock.")
        name = package.get("name", "")
        projects += int(
            validate_source(package.get("source", {}), name, project_name)
        )
        validate_artifacts(package, name)
    if projects != 1:
        raise DependencyError(
            "The lock must contain exactly one local root project."
        )
    return lock


def read_request(root: Path) -> list[str]:
    path = root / REQUEST_FILE
    if not path.exists():
        return []
    request = read_toml(regular_file(path), REQUEST_FILE)
    upgrades = request.get("upgrade")
    if (
        set(request) != {"schema", "upgrade"}
        or request["schema"] != 1
        or not isinstance(upgrades, list)
        or not upgrades
        or len(upgrades) > 100
    ):
        raise DependencyError(
            "Invalid upgrade request; expected schema = 1 and an upgrade list."
        )
    names = set()
    for spec in upgrades:
        if not isinstance(spec, str) or not PACKAGE_SPEC.fullmatch(spec):
            raise DependencyError(
                "Upgrade requests accept package names or package==version only."
            )
        name = canonical_name(spec.split("==", 1)[0])
        if name in names:
            raise DependencyError("Duplicate package in upgrade request.")
        names.add(name)
    return upgrades


def command(args: list[str], *, root: Path, env: dict | None = None) -> str:
    result = subprocess.run(
        args,
        cwd=root,
        env=env,
        text=True,
        encoding="utf-8",
        errors="replace",
        capture_output=True,
        check=False,
    )
    if result.returncode:
        raise DependencyError(
            f"{Path(args[0]).name} failed (exit {result.returncode}):\n{result.stderr[-6000:]}"
        )
    return result.stdout


def uv_env(*, corporate: bool = False) -> dict[str, str]:
    # --no-config does not clear environment variables. Whitelist transport and
    # authentication settings, never index overrides or disabled hash checks.
    allowed = {
        "UV_CACHE_DIR",
        "UV_NATIVE_TLS",
        "UV_SYSTEM_CERTS",
        "UV_KEYRING_PROVIDER",
        "UV_HTTP_TIMEOUT",
        "UV_HTTP_RETRIES",
    }
    env = {
        key: value
        for key, value in os.environ.items()
        if key != "VIRTUAL_ENV"
        and not key.startswith("PIP_")
        and (
            not key.startswith("UV_")
            or key in allowed
            or (corporate and key.startswith("UV_INDEX_JFROG_"))
        )
    }
    env["UV_PYTHON"] = sys.executable
    env["UV_PYTHON_DOWNLOADS"] = "never"
    return env


def uv(args: list[str], root: Path, *, env: dict | None = None) -> str:
    return command(
        ["uv", "--no-config", *args], root=root, env=env or uv_env()
    )


def check_uv(root: Path) -> None:
    version = uv(["--version"], root).split()
    if len(version) < 2 or version[1] != UV_VERSION:
        raise DependencyError(
            f"Install uv {UV_VERSION} using your approved package source before running this command."
        )


def index_for(mode: str, index: str | None) -> str:
    if mode == "public":
        if index:
            raise DependencyError(
                "An alternate index is only accepted in corporate mode."
            )
        return PUBLIC_INDEX
    value = index or os.environ.get("MARLIM_JFROG_INDEX", "")
    url = urlsplit(value)
    local = url.hostname in {"localhost", "127.0.0.1", "::1"}
    if (
        not url.netloc
        or url.username
        or url.password
        or url.query
        or url.fragment
        or (url.scheme != "https" and not (url.scheme == "http" and local))
    ):
        raise DependencyError(
            "Set MARLIM_JFROG_INDEX to an HTTPS simple-index URL without credentials; use UV_INDEX_JFROG_* for authentication."
        )
    return "jfrog=" + value.rstrip("/")


@contextmanager
def workspace(root: Path):
    with tempfile.TemporaryDirectory(
        prefix="marlim-dependencies-"
    ) as directory:
        temporary = Path(directory)
        for name in (
            "pyproject.toml",
            "uv.lock",
            "setup.py",
            "_version.py",
            "README.md",
            "LICENSE.md",
            REQUEST_FILE,
        ):
            path = root / name
            if path.exists():
                (temporary / name).write_bytes(regular_file(path))
        yield temporary


def upgrade_args(upgrades: list[str]) -> list[str]:
    return [
        argument
        for spec in upgrades
        for argument in ("--upgrade-package", spec)
    ]


def selection(args: argparse.Namespace) -> list[str]:
    flags = [
        argument for group in args.group for argument in ("--group", group)
    ]
    flags.extend(
        argument
        for group in args.only_group
        for argument in ("--only-group", group)
    )
    if args.no_dev:
        flags.append("--no-dev")
    return flags


def check_public(root: Path, *, pending: bool = True) -> None:
    project = validate_project(root)["project"]
    validate_lock(regular_file(root / "uv.lock"), project["name"])
    upgrades = read_request(root)
    if pending and upgrades:
        raise DependencyError(
            "An upgrade request is pending. Apply the public Actions artifact before installing or merging."
        )
    with workspace(root) as temporary:
        uv(
            ["lock", "--check", "--offline", "--default-index", PUBLIC_INDEX],
            temporary,
        )


def write_upgrade_request(
    root: Path, temporary: Path, requested: list[str]
) -> None:
    packages = read_toml(
        regular_file(temporary / "uv.lock"), "temporary lock"
    )["package"]
    pins = []
    for spec in requested:
        name = canonical_name(spec.split("==")[0])
        versions = {
            package["version"]
            for package in packages
            if canonical_name(package["name"]) == name
        }
        if len(versions) != 1:
            raise DependencyError(
                f"Specify a single compatible version for {name}; the universal lock has multiple or no candidates."
            )
        pins.append(f"{name}=={versions.pop()}")
    text = (
        "schema = 1\nupgrade = [\n"
        + "".join(f"    {json.dumps(pin)},\n" for pin in pins)
        + "]\n"
    )
    atomic_write(root / REQUEST_FILE, text.encode())


def change_dependencies(args: argparse.Namespace, root: Path) -> None:
    validate_project(root)
    for package in args.packages:
        if (
            not re.match(
                r"^[A-Za-z0-9][A-Za-z0-9._-]*(?:\[|\s|[<>=!~;]|$)", package
            )
            or "@" in package
            or "://" in package
        ):
            raise DependencyError(
                "Specify public package requirements, not options, URLs, or paths."
            )
    original = input_hashes(root)
    env = uv_env(corporate=args.mode == "corporate")
    index = index_for(args.mode, args.index)
    with workspace(root) as temporary:
        flags = (
            ["--group", args.dependency_group] if args.dependency_group else []
        )
        if args.action in {"add", "remove"}:
            uv([args.action, "--frozen", *flags, *args.packages], temporary)
            validate_project(temporary)
        requested = read_request(root)
        if args.action == "upgrade":
            for spec in args.packages:
                if not PACKAGE_SPEC.fullmatch(spec):
                    raise DependencyError(
                        "Upgrade accepts package names or package==version."
                    )
            requested = [
                spec
                for spec in requested
                if canonical_name(spec.split("==")[0])
                not in {
                    canonical_name(spec.split("==")[0])
                    for spec in args.packages
                }
            ] + args.packages
        uv(
            ["lock", "--default-index", index, *upgrade_args(requested)],
            temporary,
            env=env,
        )
        if input_hashes(root) != original:
            raise DependencyError(
                "Dependency files changed during resolution; no changes were applied."
            )
        if args.action == "upgrade":
            write_upgrade_request(root, temporary, requested)
        else:
            atomic_write(
                root / "pyproject.toml",
                regular_file(temporary / "pyproject.toml"),
            )
    print(
        "Preliminary resolution passed. Commit the manifest/request to your PR; Actions will generate the public lock."
    )


def git(root: Path, *args: str) -> str:
    return command(["git", *args], root=root).strip()


def version_summary(before: bytes | None, after: bytes) -> str:
    def versions(data):
        if data is None:
            return {}
        try:
            packages = read_toml(data, "lock").get("package", [])
        except DependencyError:
            return {}
        names = {package["name"] for package in packages}
        return {
            name: ", ".join(
                sorted(
                    {
                        package.get("version", "local")
                        for package in packages
                        if package["name"] == name
                    }
                )
            )
            for name in names
        }

    old, new = versions(before), versions(after)
    lines = [
        "# Public dependency lock",
        "",
        "| Package | Before | After |",
        "| --- | --- | --- |",
    ]
    for name in sorted(old.keys() | new.keys()):
        if old.get(name) != new.get(name):
            lines.append(
                f"| {name} | {old.get(name, '—')} | {new.get(name, '—')} |"
            )
    if old == new:
        lines.append("| — | No version changes | No version changes |")
    return "\n".join(lines) + "\n"


def public_baseline(
    root: Path, project_name: str, args: argparse.Namespace
) -> bytes:
    baseline = (
        regular_file(root / "uv.lock") if (root / "uv.lock").exists() else None
    )
    try:
        if baseline is None:
            raise DependencyError("Missing public lock")
        validate_lock(baseline, project_name)
    except DependencyError:
        if not args.base_sha or not SHA.fullmatch(args.base_sha):
            raise DependencyError(
                "A public base commit is required to repair a missing or non-public lock."
            )
        # Fetch only a validated public repository/commit; never use a URL from
        # the rejected lock. No credentials are embedded or persisted.
        command(
            [
                "git",
                "fetch",
                "--no-tags",
                f"https://github.com/{args.repository}.git",
                args.base_sha,
            ],
            root=root,
        )
        result = subprocess.run(
            ["git", "show", f"{args.base_sha}:uv.lock"],
            cwd=root,
            capture_output=True,
            check=False,
        )
        if result.returncode:
            raise DependencyError("Cannot read the public base lock.")
        baseline = result.stdout
        validate_lock(baseline, project_name)
    return baseline


def generate_lock(args: argparse.Namespace, root: Path) -> None:
    project = validate_project(root)["project"]
    inputs = input_hashes(root)
    upgrades = read_request(root)
    head = args.head_sha or git(root, "rev-parse", "HEAD")
    if not SHA.fullmatch(head) or git(root, "rev-parse", "HEAD") != head:
        raise DependencyError(
            "The checkout does not match the requested head commit."
        )
    for repository in (args.repository, args.head_repository):
        if not REPO_NAME.fullmatch(repository):
            raise DependencyError("Invalid GitHub repository identity.")
    baseline = public_baseline(root, project["name"], args)
    with workspace(root) as temporary:
        (temporary / "uv.lock").write_bytes(baseline)
        uv(
            ["lock", "--default-index", PUBLIC_INDEX, *upgrade_args(upgrades)],
            temporary,
        )
        result = regular_file(temporary / "uv.lock")
        validate_lock(result, project["name"])
        uv(
            ["lock", "--check", "--offline", "--default-index", PUBLIC_INDEX],
            temporary,
        )
    if input_hashes(root) != inputs or git(root, "rev-parse", "HEAD") != head:
        raise DependencyError("Inputs changed while generating the lock.")
    output = args.output.resolve()
    if output == root or output in root.parents:
        raise DependencyError("Use a separate artifact output directory.")
    output.mkdir(parents=True, exist_ok=True)
    provenance = {
        "schema": 1,
        "repository": args.repository,
        "head_repository": args.head_repository,
        "head_sha": head,
        "uv_version": UV_VERSION,
        "inputs": inputs,
        "lock_sha256": digest(result),
        "changed": inputs["uv.lock"] != digest(result),
    }
    atomic_write(output / "uv.lock", result)
    atomic_write(
        output / "provenance.json",
        (json.dumps(provenance, indent=2) + "\n").encode(),
    )
    atomic_write(
        output / "summary.md", version_summary(baseline, result).encode()
    )
    print(f"Public lock artifact written to {output}")


def check_generated(root: Path, directory: Path) -> None:
    provenance = json.loads(regular_file(directory / "provenance.json"))
    result = regular_file(directory / "uv.lock")
    if provenance["lock_sha256"] != digest(result) or provenance[
        "inputs"
    ] != input_hashes(root):
        raise DependencyError(
            "Generated artifact does not match these inputs."
        )
    if (
        result != regular_file(root / "uv.lock")
        or (root / REQUEST_FILE).exists()
    ):
        raise DependencyError(
            "Apply the public-uv-lock artifact, commit uv.lock, and remove "
            "the consumed upgrade request. The current lock is not ready for merge."
        )
    check_public(root)


def environment_python(environment: Path) -> Path:
    return environment / (
        "Scripts/python.exe" if os.name == "nt" else "bin/python"
    )


def sync_environment(
    args: argparse.Namespace, root: Path, *, preview: bool = False
) -> None:
    project = validate_project(root)
    inputs = input_hashes(root)
    env = uv_env(corporate=args.mode == "corporate")
    index = index_for(args.mode, args.index)
    environment = args.environment or root / (
        ".venv-preview" if preview else ".venv"
    )
    if args.environment and preview:
        raise DependencyError(
            "Preview uses .venv-preview; do not override its environment."
        )
    if args.inexact and args.mode == "corporate":
        raise DependencyError(
            "Corporate locked installs must use an exact environment."
        )
    if environment.is_symlink():
        raise DependencyError("Refusing a symbolic-link project environment.")
    environment = environment.resolve()
    with workspace(root) as temporary:
        if preview:
            uv(
                [
                    "lock",
                    "--default-index",
                    index,
                    *upgrade_args(read_request(root)),
                ],
                temporary,
                env=env,
            )
        else:
            check_public(root)
        (
            root
            / STATE_DIR
            / ("preview.json" if preview else "environment.json")
        ).unlink(missing_ok=True)
        if preview or args.mode == "public":
            env["UV_PROJECT_ENVIRONMENT"] = str(environment)
            flags = ["--no-install-project", *selection(args)]
            if args.inexact:
                flags.append("--inexact")
            uv(
                ["sync", "--locked", "--default-index", index, *flags],
                temporary,
                env=env,
            )
        else:
            requirements = temporary / "requirements.txt"
            uv(
                [
                    "export",
                    "--locked",
                    "--offline",
                    "--format",
                    "requirements.txt",
                    "--no-emit-project",
                    *selection(args),
                    "--output-file",
                    str(requirements),
                    "--default-index",
                    PUBLIC_INDEX,
                ],
                temporary,
            )
            uv(
                [
                    "venv",
                    "--allow-existing",
                    str(environment),
                    "--python",
                    sys.executable,
                ],
                temporary,
                env=env,
            )
            uv(
                [
                    "pip",
                    "sync",
                    "--python",
                    str(environment_python(environment)),
                    "--default-index",
                    index,
                    "--require-hashes",
                    "--reinstall",
                    "--only-binary",
                    ":all:",
                    "--strict",
                    str(requirements),
                ],
                temporary,
                env=env,
            )
        if (
            not args.no_install_project
            and not args.only_group
            and "build-system" in project
        ):
            env["MARLIM3_SKIP_BUILD"] = "1"
            # Runtime dependencies have already been installed from the lock.
            # Build-isolation requirements are fetched through the same index.
            uv(
                [
                    "pip",
                    "install",
                    "--python",
                    str(environment_python(environment)),
                    "--default-index",
                    index,
                    "--no-deps",
                    "--editable",
                    str(root),
                ],
                root,
                env=env,
            )
    if inputs != input_hashes(root):
        raise DependencyError(
            "Dependency inputs changed during installation; sync again before running."
        )
    state = {
        "schema": 1,
        "inputs": inputs,
        "mode": args.mode,
        "index_sha256": digest(index.encode()),
        "preview": preview,
        "environment": str(environment),
    }
    atomic_write(
        root / STATE_DIR / ("preview.json" if preview else "environment.json"),
        (json.dumps(state, indent=2) + "\n").encode(),
    )
    print(
        f"{'Preview' if preview else 'Locked'} environment prepared at {environment}; official uv.lock was preserved."
    )


def run_prepared(args: argparse.Namespace, root: Path) -> int:
    path = (
        root
        / STATE_DIR
        / ("preview.json" if args.preview else "environment.json")
    )
    if not path.exists():
        raise DependencyError(
            "Prepare the environment with sync or preview before using run."
        )
    state = json.loads(regular_file(path))
    if state.get("inputs") != input_hashes(root):
        raise DependencyError(
            "Dependency inputs changed; prepare the environment again."
        )
    if not args.preview:
        check_public(root)
    environment = Path(state["environment"])
    python = environment_python(environment)
    if not python.is_file():
        raise DependencyError(
            "Prepare the environment before running commands."
        )
    executable = (
        args.command[1:] if args.command[:1] == ["--"] else args.command
    )
    if not executable:
        raise DependencyError(
            "Provide a command to run, for example: run -- python -m pytest"
        )
    env = os.environ.copy()
    env.update(
        VIRTUAL_ENV=str(environment),
        UV_PROJECT_ENVIRONMENT=str(environment),
        UV_NO_SYNC="1",
        UV_LOCKED="1",
        UV_PYTHON=str(python),
        UV_PYTHON_DOWNLOADS="never",
        MARLIM_DEPENDENCIES_PREPARED="1",
    )
    env["PATH"] = str(python.parent) + os.pathsep + env.get("PATH", "")
    return subprocess.run(
        executable, cwd=root, env=env, check=False
    ).returncode


def origin_repository(root: Path) -> str:
    remote = git(root, "remote", "get-url", "origin")
    if remote.startswith("git@github.com:"):
        repository = remote.removeprefix("git@github.com:")
    else:
        url = urlsplit(remote)
        if url.hostname != "github.com" or url.query or url.fragment:
            raise DependencyError(
                "The origin remote must identify a GitHub repository."
            )
        repository = url.path.lstrip("/")
    repository = repository.removesuffix(".git").rstrip("/")
    if not REPO_NAME.fullmatch(repository):
        raise DependencyError("Cannot identify the origin GitHub repository.")
    return repository


def artifact_payload(archive: Path) -> tuple[bytes, dict]:
    try:
        with zipfile.ZipFile(archive) as file:
            members = file.infolist()
            if (
                len(members) != 3
                or {member.filename for member in members}
                != {"uv.lock", "provenance.json", "summary.md"}
                or sum(member.file_size for member in members)
                > MAX_ARTIFACT_SIZE
            ):
                raise DependencyError(
                    "Unexpected or oversized artifact contents."
                )
            for member in members:
                kind = stat.S_IFMT(member.external_attr >> 16)
                if kind not in {0, stat.S_IFREG}:
                    raise DependencyError(
                        "Artifact members must be regular files."
                    )
            data = file.read("uv.lock")
            provenance = json.loads(file.read("provenance.json"))
    except (zipfile.BadZipFile, UnicodeError, json.JSONDecodeError) as error:
        raise DependencyError("Invalid artifact archive.") from error
    return data, provenance


def apply_payload(
    root: Path,
    data: bytes,
    provenance: dict,
    *,
    repository: str,
    head_repository: str,
    head_sha: str,
) -> None:
    inputs = input_hashes(root)
    expected = {
        "schema": 1,
        "repository": repository,
        "head_repository": head_repository,
        "head_sha": head_sha,
        "uv_version": UV_VERSION,
        "inputs": inputs,
        "lock_sha256": digest(data),
    }
    if any(provenance.get(key) != value for key, value in expected.items()):
        raise DependencyError(
            "Artifact provenance does not match the current checkout and dependency inputs."
        )
    project = validate_project(root)["project"]
    validate_lock(data, project["name"])
    with workspace(root) as temporary:
        (temporary / "uv.lock").write_bytes(data)
        uv(
            ["lock", "--check", "--offline", "--default-index", PUBLIC_INDEX],
            temporary,
        )
    if (
        input_hashes(root) != inputs
        or git(root, "rev-parse", "HEAD") != head_sha
    ):
        raise DependencyError(
            "Checkout changed while validating the artifact."
        )
    atomic_write(root / "uv.lock", data)
    (root / REQUEST_FILE).unlink(missing_ok=True)


def apply_lock(args: argparse.Namespace, root: Path) -> None:
    if not REPO_NAME.fullmatch(args.repository) or args.run <= 0:
        raise DependencyError("Provide a valid repository and Actions run ID.")
    if git(root, "branch", "--show-current") in {"main", "develop", ""}:
        raise DependencyError(
            "Apply the lock on a feature branch, not a default or detached branch."
        )
    head = git(root, "rev-parse", "HEAD")
    head_repository = origin_repository(root)
    run = json.loads(
        command(
            ["gh", "api", f"repos/{args.repository}/actions/runs/{args.run}"],
            root=root,
        )
    )
    if (
        run.get("head_sha") != head
        or run.get("head_repository", {}).get("full_name") != head_repository
        or run.get("path", "").split("@")[0] != WORKFLOW
        or run.get("status") != "completed"
        or run.get("event") not in {"pull_request", "workflow_dispatch"}
    ):
        raise DependencyError(
            "The Actions run does not match this branch, repository, or lock workflow."
        )
    # A generation run intentionally fails its final readiness check until the
    # author applies the artifact. Never use the overall conclusion as identity.
    response = json.loads(
        command(
            [
                "gh",
                "api",
                f"repos/{args.repository}/actions/runs/{args.run}/artifacts?per_page=100",
            ],
            root=root,
        )
    )
    name = f"public-uv-lock-{head}"
    artifacts = [
        artifact
        for artifact in response.get("artifacts", [])
        if artifact.get("name") == name and not artifact.get("expired")
    ]
    if (
        len(artifacts) != 1
        or artifacts[0].get("size_in_bytes", MAX_ARTIFACT_SIZE + 1)
        > MAX_ARTIFACT_SIZE
    ):
        raise DependencyError(
            "The run has no unique, unexpired public lock artifact."
        )
    artifact_id = artifacts[0].get("id")
    if not isinstance(artifact_id, int) or artifact_id <= 0:
        raise DependencyError("Invalid artifact identity.")
    with tempfile.TemporaryDirectory(
        prefix="marlim-lock-download-"
    ) as directory:
        archive = Path(directory) / "artifact.zip"
        with archive.open("wb") as output:
            result = subprocess.run(
                [
                    "gh",
                    "api",
                    f"repos/{args.repository}/actions/artifacts/{artifact_id}/zip",
                ],
                cwd=root,
                stdout=output,
                stderr=subprocess.PIPE,
                check=False,
            )
        if result.returncode or archive.stat().st_size > MAX_ARTIFACT_SIZE:
            raise DependencyError(
                "Could not download a bounded artifact archive."
            )
        data, provenance = artifact_payload(archive)
        apply_payload(
            root,
            data,
            provenance,
            repository=args.repository,
            head_repository=head_repository,
            head_sha=head,
        )
    print(
        "Public lock applied. Review the diff, sync and test, then commit and push your feature branch."
    )


def parser() -> argparse.ArgumentParser:
    result = argparse.ArgumentParser(description=__doc__)
    result.add_argument("--project", type=Path, default=ROOT)
    sub = result.add_subparsers(dest="action", required=True)
    for name in ("add", "remove", "upgrade", "sync", "preview"):
        child = sub.add_parser(name)
        child.add_argument(
            "--mode",
            choices=("public", "corporate"),
            default=os.environ.get("MARLIM_DEPENDENCY_MODE", "public"),
        )
        child.add_argument(
            "--index",
            help="Corporate simple-index URL; defaults to MARLIM_JFROG_INDEX",
        )
        if name in {"add", "remove", "upgrade"}:
            child.add_argument("packages", nargs="+")
            if name in {"add", "remove"}:
                child.add_argument("--group", dest="dependency_group")
        else:
            child.add_argument("--group", action="append", default=[])
            child.add_argument("--only-group", action="append", default=[])
            child.add_argument("--no-dev", action="store_true")
            child.add_argument("--no-install-project", action="store_true")
            child.add_argument(
                "--environment",
                type=Path,
                help="Override the install environment, e.g. the Read the Docs virtualenv",
            )
            child.add_argument(
                "--inexact",
                action="store_true",
                help="Retain platform build tools in a public environment",
            )
    sub.add_parser("check")
    child = sub.add_parser("generate-lock")
    child.add_argument("--output", type=Path, required=True)
    child.add_argument("--repository", default=REPOSITORY)
    child.add_argument("--head-repository", required=True)
    child.add_argument("--head-sha")
    child.add_argument("--base-sha")
    child = sub.add_parser("check-generated")
    child.add_argument("--artifact-dir", type=Path, required=True)
    child = sub.add_parser("apply-lock")
    child.add_argument("--run", type=int, required=True)
    child.add_argument("--repository", default=REPOSITORY)
    child = sub.add_parser("run")
    child.add_argument("--preview", action="store_true")
    child.add_argument("command", nargs=argparse.REMAINDER)
    return result


def main(argv: list[str] | None = None) -> int:
    args = parser().parse_args(argv)
    root = args.project.resolve()
    try:
        if getattr(args, "mode", "public") not in {"public", "corporate"}:
            raise DependencyError(
                "MARLIM_DEPENDENCY_MODE must be public or corporate."
            )
        check_uv(root)
        handlers = {
            "add": lambda: change_dependencies(args, root),
            "remove": lambda: change_dependencies(args, root),
            "upgrade": lambda: change_dependencies(args, root),
            "sync": lambda: sync_environment(args, root),
            "preview": lambda: sync_environment(args, root, preview=True),
            "run": lambda: run_prepared(args, root),
            "check": lambda: check_public(root),
            "generate-lock": lambda: generate_lock(args, root),
            "check-generated": lambda: check_generated(
                root, args.artifact_dir
            ),
            "apply-lock": lambda: apply_lock(args, root),
        }
        return handlers[args.action]() or 0
    except (
        DependencyError,
        OSError,
        ValueError,
        KeyError,
        TypeError,
    ) as error:
        print(f"Dependency workflow error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
