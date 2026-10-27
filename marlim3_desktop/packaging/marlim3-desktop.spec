# -*- mode: python ; coding: utf-8 -*-

import os
import re
import subprocess
import sys
from pathlib import Path

from PyInstaller.depend.bindepend import get_imports
from PyInstaller.utils.hooks import (
    collect_all,
    collect_data_files,
    collect_submodules,
    copy_metadata,
)


root = Path(SPECPATH).resolve().parents[1]
desktop_root = root / "marlim3_desktop"
icons_dir = desktop_root / "assets" / "icons"
windows_icon = icons_dir / "app-icon.ico"
macos_icon = icons_dir / "app-icon.icns"
runtime_icon = icons_dir / "app-icon.png"
logo = root / "assets" / "branding" / "logo.svg"

version_namespace = {}
exec((root / "_version.py").read_text(encoding="utf-8"), version_namespace)
app_version = version_namespace["__version__"]

required_assets = (windows_icon, macos_icon, runtime_icon, logo)
missing_assets = [str(path) for path in required_assets if not path.is_file()]
if missing_assets:
    raise FileNotFoundError(f"Missing desktop assets: {', '.join(missing_assets)}")

datas = []
binaries = []
hiddenimports = []

streamlit_datas, streamlit_binaries, streamlit_hiddenimports = collect_all("streamlit")
datas += streamlit_datas
binaries += streamlit_binaries
hiddenimports += streamlit_hiddenimports
hiddenimports += [
    "jsonschema",
    "lxml",
    "matplotlib",
    "networkx",
    "numpy",
    "pandas",
    "plotly",
    "plotly.graph_objects",
    "PySide6.QtCore",
    "PySide6.QtGui",
    "PySide6.QtNetwork",
    "PySide6.QtSvg",
    "PySide6.QtWebChannel",
    "PySide6.QtWebView",
    "PySide6.QtWidgets",
    "requests",
    "seaborn",
    "xmltodict",
]
if sys.platform != "darwin":
    hiddenimports += [
        "PySide6.QtWebEngineCore",
        "PySide6.QtWebEngineWidgets",
    ]

for package in ("streamlit", "PySide6", "plotly", "pandas", "jsonschema", "marlim3"):
    try:
        datas += copy_metadata(package)
    except Exception:
        pass

datas += collect_data_files("marlim3")
hiddenimports += collect_submodules("marlim3")
datas += [
    (str(root / "gui" / "app.py"), "gui"),
    (str(root / "gui" / ".streamlit"), "gui/.streamlit"),
    (str(root / "demos"), "demos"),
    (str(runtime_icon), "marlim3_desktop/assets/icons"),
    (str(logo), "assets/branding"),
]

engine_name = "Marlim3.exe" if sys.platform.startswith("win") else "Marlim3"
engine_path = root / "marlim3" / engine_name
if not engine_path.is_file():
    raise FileNotFoundError(f"Missing native Marlim3 engine: {engine_path}")
binaries.append((str(engine_path), "marlim3"))


def collect_linux_runtime_libraries():
    import PySide6

    pyside_dir = Path(PySide6.__file__).resolve().parent
    qt_dir = pyside_dir / "Qt"
    roots = (
        qt_dir / "lib" / "libQt6WebEngineCore.so.6",
        qt_dir / "plugins" / "platforms" / "libqxcb.so",
        qt_dir / "plugins" / "platforms" / "libqwayland.so",
        qt_dir / "plugins" / "platforms" / "libqoffscreen.so",
    )
    system_libraries = {
        "ld-linux-x86-64.so.2",
        "libanl.so.1",
        "libc.so.6",
        "libdl.so.2",
        "libgcc_s.so.1",
        "libm.so.6",
        "libmvec.so.1",
        "libnsl.so.1",
        "libpthread.so.0",
        "libresolv.so.2",
        "librt.so.1",
        "libutil.so.1",
    }
    pending = [path for path in roots if path.is_file()]
    processed = set()
    portable = {}
    missing = set()
    while pending:
        binary = pending.pop()
        resolved_binary = binary.resolve()
        if resolved_binary in processed:
            continue
        processed.add(resolved_binary)
        for library_name, library_path in get_imports(str(resolved_binary)):
            if library_name in system_libraries:
                continue
            if not library_path:
                missing.add(library_name)
                continue
            source = Path(library_path)
            resolved_source = source.resolve()
            if pyside_dir in resolved_source.parents:
                continue
            portable[library_name] = source
            pending.append(resolved_source)
    if missing:
        raise RuntimeError(
            "Missing Linux desktop runtime libraries: " + ", ".join(sorted(missing))
        )
    baseline_text = os.environ.get("MARLIM3_GLIBC_BASELINE")
    baseline = (
        tuple(int(part) for part in baseline_text.split("."))
        if baseline_text
        else None
    )
    print("Bundled Linux desktop runtime libraries:")
    for name, source in sorted(portable.items()):
        print(f"  {name}: {source}")
        if baseline is not None:
            result = subprocess.run(
                ["objdump", "-T", str(source.resolve())],
                capture_output=True,
                text=True,
                check=False,
            )
            versions = []
            for line in result.stdout.splitlines():
                if "*UND*" not in line:
                    continue
                versions.extend(
                    tuple(int(part) for part in match.groups())
                    for match in re.finditer(r"GLIBC_(\d+)\.(\d+)", line)
                )
            if versions and max(versions) > baseline:
                required = ".".join(str(part) for part in max(versions))
                raise RuntimeError(
                    f"{name} requires glibc {required}, newer than "
                    f"the configured {baseline_text} baseline"
                )
    return [(str(source), ".") for _, source in sorted(portable.items())]


if sys.platform.startswith("linux"):
    binaries += collect_linux_runtime_libraries()

if sys.platform == "darwin":
    import PySide6

    pyside_dir = Path(PySide6.__file__).resolve().parent
    webview_plugin = pyside_dir / "Qt" / "plugins" / "webview" / "libqtwebview_darwin.dylib"
    if not webview_plugin.is_file():
        raise FileNotFoundError(f"Missing macOS QtWebView plugin: {webview_plugin}")
    binaries.append((str(webview_plugin), "PySide6/Qt/plugins/webview"))

a = Analysis(
    [str(desktop_root / "__main__.py")],
    pathex=[str(root)],
    binaries=binaries,
    datas=datas,
    hiddenimports=hiddenimports,
    hookspath=[str(desktop_root / "packaging" / "hooks")],
    runtime_hooks=[],
    excludes=[
        "IPython",
        "_tkinter",
        "jedi",
        "nbformat",
        "openpyxl",
        "pytest",
        "scipy",
        "sklearn",
        "streamlit.hello",
        "streamlit.testing",
        "tkinter",
    ] + (
        [
            "PySide6.QtWebEngineCore",
            "PySide6.QtWebEngineWidgets",
        ]
        if sys.platform == "darwin"
        else []
    ),
    noarchive=False,
    optimize=1,
    hooksconfig={"matplotlib": {"backends": ["Agg"]}},
)

if sys.platform.startswith("linux"):
    def required_linux_qt_data(entry):
        destination = entry[0].replace("\\", "/")
        name = Path(destination).name
        if name == "qtwebengine_devtools_resources.pak":
            return False
        if "/qtwebengine_locales/" in destination:
            return name in {"en-US.pak", "pt-BR.pak"}
        if name.startswith("qtwebengine_") and name.endswith(".qm"):
            return False
        return True

    a.datas = type(a.datas)(filter(required_linux_qt_data, a.datas))

pyz = PYZ(a.pure)

if sys.platform == "darwin":
    exe = EXE(
        pyz,
        a.scripts,
        [],
        exclude_binaries=True,
        name="Marlim3",
        debug=False,
        bootloader_ignore_signals=False,
        strip=False,
        upx=False,
        console=False,
        disable_windowed_traceback=False,
        argv_emulation=False,
        icon=str(macos_icon),
    )
    coll = COLLECT(
        exe,
        a.binaries,
        a.datas,
        strip=False,
        upx=False,
        name="Marlim3",
    )
    app = BUNDLE(
        coll,
        name="Marlim3.app",
        icon=str(macos_icon),
        bundle_identifier="br.com.petrobras.marlim3",
        info_plist={
            "CFBundleName": "Marlim3",
            "CFBundleDisplayName": "Marlim3",
            "CFBundleShortVersionString": app_version,
            "CFBundleVersion": app_version,
            "NSHighResolutionCapable": True,
        },
    )
else:
    if sys.platform.startswith("win"):
        exe = EXE(
            pyz,
            a.scripts,
            a.binaries,
            a.datas,
            [],
            name="Marlim3-Desktop",
            debug=False,
            bootloader_ignore_signals=False,
            strip=False,
            upx=False,
            runtime_tmpdir=None,
            console=False,
            disable_windowed_traceback=False,
            icon=str(windows_icon),
        )
    else:
        exe = EXE(
            pyz,
            a.scripts,
            a.binaries,
            a.datas,
            [],
            name="Marlim3-Desktop",
            debug=False,
            bootloader_ignore_signals=False,
            strip=False,
            upx=False,
            runtime_tmpdir=None,
            console=False,
            disable_windowed_traceback=False,
        )
