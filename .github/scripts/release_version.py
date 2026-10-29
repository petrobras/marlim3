"""Release version helpers: keep _version.py in step with Release-* PR labels and check it against a tag."""

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path


VERSION_FILE = Path(__file__).resolve().parents[2] / "_version.py"
VERSION_LINE = re.compile(r"^__version__ = '(\d+)\.(\d+)\.(\d+)'$", re.MULTILINE)
LEVELS = {"Release-Major": "major", "Release-Minor": "minor", "Release-Patch": "patch"}


def parse(version):
    match = re.fullmatch(r"v?(\d+)\.(\d+)\.(\d+)", version)
    if not match:
        raise ValueError(f"not a vX.Y.Z version: {version!r}")
    return tuple(int(part) for part in match.groups())


def to_str(version):
    return ".".join(map(str, version))


def bump(version, level):
    major, minor, patch = version
    if level == "major":
        return major + 1, 0, 0
    if level == "minor":
        return major, minor + 1, 0
    return major, minor, patch + 1


def level_from_labels(labels):
    levels = {LEVELS[label] for label in labels if label in LEVELS}
    if len(levels) > 1:
        raise ValueError(f"more than one release label: {sorted(labels)}")
    return levels.pop() if levels else None


def version_from_text(text):
    match = VERSION_LINE.search(text)
    if not match:
        raise ValueError("__version__ = 'X.Y.Z' not found")
    return tuple(int(part) for part in match.groups())


def read_version(path):
    return version_from_text(path.read_text(encoding="utf-8"))


def write_version(version, path):
    text = path.read_text(encoding="utf-8")
    path.write_text(VERSION_LINE.sub(f"__version__ = '{to_str(version)}'", text), encoding="utf-8")


def git(*args):
    return subprocess.run(["git", *args], capture_output=True, text=True, check=True).stdout


def release_tags():
    tags = []
    for tag in git("tag", "--list", "v*").split():
        try:
            tags.append(parse(tag))
        except ValueError:
            continue
    return tags


def next_release(level, base_version, tags):
    """Bump from the highest of the base branch version and the existing tags, so a tag is never reused."""
    return bump(max([base_version, *tags]), level)


def allowed_versions(level, base_version, tags):
    """Versions a PR may carry: the labelled bump, or (unlabelled) no change or a single-step bump."""
    if level:
        return [next_release(level, base_version, tags)]
    return [base_version] + [next_release(lvl, base_version, tags) for lvl in ("patch", "minor", "major")]


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    for name, help_text in (("sync", "write the version required by the PR labels to _version.py"),
                            ("check-pr", "fail if _version.py does not fit the PR labels")):
        cmd = commands.add_parser(name, help=help_text)
        cmd.add_argument("labels_json", help="JSON list of the PR label names")
        cmd.add_argument("--base-ref", default="origin/main", help="git ref of the PR base branch")
        if name == "sync":
            cmd.add_argument("--restore", action="store_true",
                             help="without a release label, restore the base version (label removed)")
    commands.add_parser("show", help="print the version in _version.py")
    commands.add_parser("check-tag", help="fail if the tag differs from _version.py").add_argument("tag")
    args = parser.parse_args(argv)

    if args.command in ("sync", "check-pr"):
        level = level_from_labels(json.loads(args.labels_json))
        base = version_from_text(git("show", f"{args.base_ref}:_version.py"))
        allowed = allowed_versions(level, base, release_tags())
        current = read_version(VERSION_FILE)
        if args.command == "sync":
            # Recomputing from the base branch makes relabelling (e.g. Patch -> Minor) idempotent.
            target = allowed[0] if level or args.restore else current
            write_version(target, VERSION_FILE)
            print(to_str(target))
        elif current not in allowed:
            expected = " or ".join(to_str(v) for v in allowed)
            sys.exit(f"_version.py is {to_str(current)}, expected {expected} "
                     f"({'Release-' + level.capitalize() + ' label' if level else 'no release label'}, "
                     f"main at {to_str(base)}). Let a Release-* label set it, or in PRs from forks "
                     "set it by hand to the expected value.")
        else:
            print(f"_version.py {to_str(current)} fits the PR labels")
    elif args.command == "show":
        print(to_str(read_version(VERSION_FILE)))
    else:
        expected, found = parse(args.tag), read_version(VERSION_FILE)
        if expected != found:
            sys.exit(f"Tag {args.tag} does not match _version.py ({to_str(found)}). "
                     "Bump _version.py with a Release-* label on the PR instead of tagging by hand.")
        print(f"_version.py matches {args.tag}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
