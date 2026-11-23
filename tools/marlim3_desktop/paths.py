"""Runtime paths shared by the desktop launcher and its tests."""

from __future__ import annotations

import os
import sys
from pathlib import Path


def get_bundle_root() -> Path:
    override = os.environ.get("MARLIM3_DESKTOP_ROOT")
    if override:
        return Path(override).resolve()
    if getattr(sys, "frozen", False) and hasattr(sys, "_MEIPASS"):
        return Path(sys._MEIPASS).resolve()
    return Path(__file__).resolve().parents[1]


def executable_name() -> str:
    return "Marlim3.exe" if sys.platform.startswith("win") else "Marlim3"


def get_gui_app_path(bundle_root: Path | None = None) -> Path:
    return (bundle_root or get_bundle_root()) / "gui" / "app.py"


def get_engine_path(bundle_root: Path | None = None) -> Path:
    return (bundle_root or get_bundle_root()) / "marlim3" / executable_name()


def get_app_icon_path(bundle_root: Path | None = None) -> Path:
    return (
        (bundle_root or get_bundle_root())
        / "marlim3_desktop"
        / "assets"
        / "icons"
        / "app-icon.png"
    )


def get_logo_path(bundle_root: Path | None = None) -> Path:
    return (bundle_root or get_bundle_root()) / "assets" / "branding" / "logo.svg"
