import argparse
import json
import subprocess
import sys
import time
import urllib.request


DEFAULT_ATTEMPTS = 20
DEFAULT_DELAY_SECONDS = 15
REQUEST_TIMEOUT_SECONDS = 10
PYPI_JSON_URL = "https://pypi.org/pypi/{project}/json"
PYPI_SIMPLE_URL = "https://pypi.org/simple/{project}/"
SIMPLE_JSON_ACCEPT = "application/vnd.pypi.simple.v1+json"


def normalize_project_name(project):
    return project.lower().replace("_", "-")


def load_json_url(url, headers=None):
    request = urllib.request.Request(url, headers=headers or {})
    with urllib.request.urlopen(request, timeout=REQUEST_TIMEOUT_SECONDS) as response:
        return json.load(response)


def release_has_files(project_data, version):
    return bool(project_data.get("releases", {}).get(version, []))


def simple_has_version(simple_data, project, version):
    expected_prefix = f"{normalize_project_name(project)}-{version}"
    filenames = [file.get("filename", "").lower() for file in simple_data.get("files", [])]
    return any(filename.startswith(expected_prefix) for filename in filenames)


def pypi_has_release(project, version):
    project_name = normalize_project_name(project)
    project_data = load_json_url(PYPI_JSON_URL.format(project=project_name))
    if not release_has_files(project_data, version):
        return False

    simple_data = load_json_url(
        PYPI_SIMPLE_URL.format(project=project_name),
        {"Accept": SIMPLE_JSON_ACCEPT},
    )
    return simple_has_version(simple_data, project_name, version)


def pip_can_resolve(project, version):
    requirement = f"{project}=={version}"
    command = (
        sys.executable,
        "-m",
        "pip",
        "install",
        "--dry-run",
        "--no-deps",
        "--no-cache-dir",
        requirement,
    )
    result = subprocess.run(
        command,
        capture_output=True,
        text=True,
    )
    return result.returncode == 0


def purge_pip_cache():
    subprocess.run(
        [sys.executable, "-m", "pip", "cache", "purge"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=False,
    )


def release_is_installable(project, version):
    if not pypi_has_release(project, version):
        return False
    if not pip_can_resolve(project, version):
        return False
    purge_pip_cache()
    return True


def wait_for_release(project, version, attempts, delay_seconds):
    for attempt in range(1, attempts + 1):
        if release_is_installable(project, version):
            print(f"{project}=={version} is installable from PyPI")
            return True
        if attempt < attempts:
            print(f"Attempt {attempt}: {project}=={version} is not installable yet")
            time.sleep(delay_seconds)
    print(f"{project}=={version} did not become installable from PyPI in time")
    return False


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("project")
    parser.add_argument("version")
    parser.add_argument("--attempts", type=int, default=DEFAULT_ATTEMPTS)
    parser.add_argument("--delay-seconds", type=int, default=DEFAULT_DELAY_SECONDS)
    return parser.parse_args()


def main():
    args = parse_args()
    if wait_for_release(args.project, args.version, args.attempts, args.delay_seconds):
        return 0
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
