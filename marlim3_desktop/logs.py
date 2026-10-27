"""Desktop log setup and platform-specific log locations."""

from __future__ import annotations

import logging
import os
import sys
from logging.handlers import RotatingFileHandler
from pathlib import Path


def get_log_directory() -> Path:
    if sys.platform.startswith("win"):
        base = Path(os.environ.get("LOCALAPPDATA", Path.home() / "AppData" / "Local"))
        return base / "Marlim3" / "logs"
    if sys.platform == "darwin":
        return Path.home() / "Library" / "Logs" / "Marlim3"
    state_home = Path(os.environ.get("XDG_STATE_HOME", Path.home() / ".local" / "state"))
    return state_home / "marlim3" / "logs"


def configure_logging() -> tuple[logging.Logger, Path]:
    log_dir = get_log_directory()
    log_dir.mkdir(parents=True, exist_ok=True)
    log_path = log_dir / "desktop.log"
    logger = logging.getLogger("marlim3.desktop")
    logger.setLevel(logging.INFO)
    logger.propagate = False
    if not logger.handlers:
        handler = RotatingFileHandler(
            log_path,
            maxBytes=2 * 1024 * 1024,
            backupCount=4,
            encoding="utf-8",
        )
        handler.setFormatter(
            logging.Formatter("%(asctime)s %(levelname)s %(process)d %(message)s")
        )
        logger.addHandler(handler)
    return logger, log_path
