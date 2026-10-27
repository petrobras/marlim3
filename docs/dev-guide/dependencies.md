# Dependency maintenance

Marlim3 keeps one public `uv.lock`. GitHub Actions generates or reproduces the
accepted lock using PyPI. Developers outside Petrobras use PyPI normally;
developers inside Petrobras use a locally configured JFrog index for preliminary
resolution and installation. Corporate URLs and credentials never belong in
`pyproject.toml` or the official lock.

The maintenance helper requires **Python 3.12+ and uv 0.13.1**. Install these
through the package sources approved for your environment. The helper uses only
the Python standard library, so it works before project dependencies are installed.
Use your Python 3.12+ interpreter for the `python` commands below.

The helper supports the current standalone project with static dependency
declarations and `tool.uv.required-version`. Workspaces, direct URL/path sources
and additional project-level uv resolution settings require explicit support;
they are rejected rather than silently ignored.

## Public development

Existing, consistent dependencies can still be installed normally:

```bash
uv sync --locked --group dev
uv run --locked python -m pytest
```

`--locked` prevents automatic lock updates during normal development. A local
`uv lock` result is preliminary; the public lock workflow must reproduce it before
it is accepted. An addition or removal can also be recorded with `uv add --frozen`
or `uv remove --frozen`, but those commands do not validate availability.

## Petrobras configuration

Enable corporate mode locally, using the actual simple-index URL supplied by
Petrobras. The following values are placeholders, not a verified Petrobras endpoint:

```bash
export MARLIM_DEPENDENCY_MODE=corporate
export MARLIM_JFROG_INDEX='https://registry.example/artifactory/api/pypi/repository/simple'
export UV_INDEX_JFROG_USERNAME='your-user'
export UV_INDEX_JFROG_PASSWORD='your-token'
```

On PowerShell, use `$env:MARLIM_DEPENDENCY_MODE = 'corporate'` and equivalent
assignments for the other variables. Do not put credentials in an index URL or
commit local configuration. If an approved certificate is required, configure
`SSL_CERT_FILE` or the supported system-certificate settings; do not disable TLS
verification. The helper retains transport/authentication settings but clears
index overrides and options that disable hash verification.

The helper deliberately requires explicit corporate configuration rather than
trying to infer the environment from a failed public connection. Existing user
`uv.toml` files are not read by the helper: supply the endpoint and authentication
above, or the native uv credential store. Outside corporate mode, its default is PyPI.

## Add, remove or upgrade dependencies

Run the same maintenance commands in either mode:

```bash
python scripts/dependencies.py add package-name --group docs
python scripts/dependencies.py remove package-name --group docs
python scripts/dependencies.py upgrade package-name
python scripts/dependencies.py upgrade package-name==desired-version
```

Add/remove operations edit a temporary manifest and resolve it using your mode's
index. Only the requested manifest change is copied back. Upgrade operations
resolve against that index and write `dependency-update.toml` with explicit
version requests. The official lock is preserved. A failed preliminary resolution
leaves the official manifest, request and lock unchanged.

An upgrade can require changes to transitive packages. Multiple universal versions
require an explicit compatible target. Upgrades must still satisfy the declared
constraints. Do not delete the existing lock or request a blanket upgrade unless
updating all dependencies is intentional.

### Test a change before public generation

A changed manifest or pending upgrade is not yet consistent with the official
lock. To work locally while waiting for Actions:

```bash
python scripts/dependencies.py preview --group dev --group docs
python scripts/dependencies.py run --preview -- python -m pytest
```

This uses a separate, ignored `.venv-preview` and temporary lock. Inside Petrobras,
all preliminary resolution/downloads use JFrog. Preview results are provisional:
the final public lock must be installed and tested again. To test documentation
without installing Marlim3, use `preview --only-group docs --no-install-project`.

## Obtain the public lock

1. Commit the changed `pyproject.toml` and/or `dependency-update.toml` on your
   feature branch, push it and open/update the PR against `petrobras/marlim3`.
2. Find the **Public uv lock** Actions run. It operates on the PR head, uses the
   pinned uv version and public configuration, and uploads an artifact named
   `public-uv-lock-<head-sha>` containing the lock, version summary and provenance.
3. Apply that run's result on the same feature branch:

   ```bash
   python scripts/dependencies.py apply-lock --run ACTIONS_RUN_ID
   ```

4. Review the diff, install/test the official result, commit `uv.lock` and the
   removal of the consumed upgrade request, then push the feature branch again.

The helper requires an authenticated `gh` CLI. It checks the run's repository,
workflow, head/fork identity, artifact contents, input hashes, output hash and
lock consistency before writing. It refuses default/detached branches, stale
inputs, expired artifacts, unexpected archive files and non-public sources.
`apply-lock --repository OWNER/REPO` is available when the workflow runs in your
own fork. It never commits, pushes, changes `main`, or sends comments automatically.

The first generation run may **fail its final readiness check intentionally**:
the artifact is uploaded first, and the failure means the author still needs to
apply the result. Other build/test jobs also stay blocked while the versioned lock
is stale or an upgrade request is pending. A generation error produces no usable
artifact. New contributors may need the repository's normal Actions approval.

The workflow also supports manual dispatch once it exists on the repository's
default branch. A newly introduced workflow cannot be dispatched in an old fork
that does not yet contain it on its default branch; use the upstream PR run.

## Install and test the official result

Outside Petrobras, native `uv sync --locked` remains available. Inside Petrobras:

```bash
python scripts/dependencies.py sync --group dev --group docs
python scripts/dependencies.py run -- python -m pytest
python scripts/dependencies.py run -- python schema_to_markdown.py --batch
```

Corporate synchronization validates the public lock offline, exports selected
versions/markers/hashes into a temporary requirements file, and uses
`uv pip sync --require-hashes --only-binary :all: --strict` through JFrog alone.
Hashed runtime packages are reinstalled to avoid accepting an unverified existing
environment solely because its package versions match.
It then installs the editable project with `--no-deps` when requested. Exact
installs fail if an approved version/wheel is absent or a mirror artifact has a
different hash. They do not substitute versions or fall back to PyPI.

Build-isolation requirements for the editable project are downloaded through the
same index but are not pinned by the runtime lock. Source-distribution builds are
not enabled for corporate runtime installs. If they become necessary, their build
inputs and artifact equivalence need explicit validation.

The helper records prepared inputs locally and refuses to run after dependency
files change. A failed install invalidates its previous readiness record. Commands
run from the selected environment with automatic uv syncing disabled, including
nested desktop commands. Avoid ordinary `uv sync`/`uv run` in corporate mode;
those can reintroduce corporate origins into the lock.

For a desktop build, prepare `--no-dev --group gui --group desktop`, then run the
build through the helper. When invoked directly in corporate mode, the desktop
builder delegates dependency installation to the helper.

## CI, documentation and rollout

The build pipeline checks every package's source and artifact URL/hash, including
unselected groups, and verifies lock consistency before installation. The public
lock workflow compares the committed file with its generated result. Configure
**Generate and verify public uv lock** and **Public dependency lock policy** as
required checks in branch protection to enforce this policy at merge time.

Read the Docs installs its `docs` group from the same lock. Its public installation
retains platform build tools (`--inexact`) in the Read the Docs virtualenv while
pinning all exported project documentation dependencies. Normal project and
corporate environments are exact installations.

The repository tests include a local PEP 503 mirror with public-network access
blocked, successful hashed installation, missing-version rejection, repacked-wheel
rejection and stale-lock rejection. This is not validation of Petrobras's actual
JFrog configuration. Confirm its uv version, endpoint, authentication, CA trust,
matching public wheel hashes, supported platforms and availability policy before
corporate rollout. Access to GitHub/Actions artifact downloads must also be approved.
The Python dependency flow does not provide native executables or other network
resources required by the application.
