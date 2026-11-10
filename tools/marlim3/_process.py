"""Small cross-platform helpers for launching simulator processes."""

from __future__ import annotations

import subprocess
import sys


def process_group_kwargs(*, hidden: bool = True) -> dict[str, object]:
    if sys.platform.startswith("win"):
        flags = subprocess.CREATE_NEW_PROCESS_GROUP
        if hidden:
            flags |= subprocess.CREATE_NO_WINDOW
        return {"creationflags": flags}
    return {"start_new_session": True}


def hidden_process_kwargs() -> dict[str, object]:
    if sys.platform.startswith("win"):
        return {"creationflags": subprocess.CREATE_NO_WINDOW}
    return {}
