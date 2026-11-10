from __future__ import annotations

import hashlib
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys

from .config import TOOL_ROOT


APP_ID = "regression-tool"
APP_NAME = "Regression Tool"
WINDOW_CLASS = "RegressionTool"
LINUX_ICON_NAME = "regression-tool"
ICON_NAMES = {
    "Linux": "app-icon-runtime.png",
    "Windows": "app-icon-runtime.ico",
    "Darwin": "app-icon.icns",
}


def icon_directory() -> Path:
    candidates = (
        TOOL_ROOT / "assets" / "icons",
        Path(sys.prefix) / "share" / "marlim3-regression-tool" / "icons",
    )
    for candidate in candidates:
        if all((candidate / name).is_file() for name in ICON_NAMES.values()):
            return candidate
    raise FileNotFoundError("The cross-platform Regression Tool icons were not found")


def native_icon_path(system: str | None = None) -> Path:
    current = system or platform.system()
    return icon_directory() / ICON_NAMES.get(current, "app-icon.png")


def browser_icon_path() -> Path:
    return icon_directory() / "app-icon-runtime.png"


def browser_icon_version() -> str:
    return hashlib.sha256(browser_icon_path().read_bytes()).hexdigest()[:12]


def favicon_path(system: str | None = None) -> Path:
    return native_icon_path("Windows") if (system or platform.system()) == "Windows" else browser_icon_path()


def chrome_window_flags(system: str | None = None) -> list[str]:
    return [f"--class={WINDOW_CLASS}"] if (system or platform.system()) == "Linux" else []


def _install_linux_theme_icons(data_home: Path) -> None:
    source_root = icon_directory() / "hicolor"
    target_root = data_home / "icons" / "hicolor"
    for source in source_root.glob("*x*/apps/*.png"):
        relative = source.relative_to(source_root)
        target = target_root / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        if not target.is_file() or target.read_bytes() != source.read_bytes():
            shutil.copy2(source, target)


def _refresh_linux_desktop_caches(data_home: Path) -> None:
    commands = (
        ("update-desktop-database", str(data_home / "applications")),
        ("gtk-update-icon-cache", "-f", "-t", str(data_home / "icons" / "hicolor")),
    )
    for command in commands:
        executable = shutil.which(command[0])
        if executable:
            subprocess.run(
                (executable, *command[1:]),
                check=False,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
            )


def _linux_data_homes() -> tuple[Path, ...]:
    host_home = Path.home() / ".local" / "share"
    configured = Path(os.environ.get("XDG_DATA_HOME", host_home))
    return tuple(dict.fromkeys((host_home, configured)))


def prepare_linux_desktop_icon(chrome: str) -> None:
    """Associate Chrome's app window class with the packaged icon on Linux desktops."""
    if platform.system() != "Linux":
        return
    try:
        content = "\n".join((
            "[Desktop Entry]",
            "Type=Application",
            f"Name={APP_NAME}",
            f"Exec={chrome}",
            f"Icon={LINUX_ICON_NAME}",
            f"StartupWMClass={WINDOW_CLASS}",
            "NoDisplay=true",
            "",
        ))
        for data_home in _linux_data_homes():
            _install_linux_theme_icons(data_home)
            desktop_file = data_home / "applications" / f"{APP_ID}.desktop"
            desktop_file.parent.mkdir(parents=True, exist_ok=True)
            if not desktop_file.is_file() or desktop_file.read_text(encoding="utf-8") != content:
                desktop_file.write_text(content, encoding="utf-8")
            legacy_file = data_home / "applications" / "io.github.pedrorigon.RegressionTool.desktop"
            legacy_file.unlink(missing_ok=True)
            _refresh_linux_desktop_caches(data_home)
    except OSError:
        # The favicon still supplies the application icon in restricted sessions.
        return
