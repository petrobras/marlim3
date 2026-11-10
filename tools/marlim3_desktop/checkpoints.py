"""Structured startup checkpoints used by the splash and smoke tests."""

from __future__ import annotations

import json
import time
from pathlib import Path
from typing import Callable


PROGRESS = {
    "launcher_started": 5,
    "splash_created": 10,
    "runtime_configured": 15,
    "server_process_started": 25,
    "health_endpoint_ready": 70,
    "webengine_created": 85,
    "webengine_load_finished": 95,
    "main_window_visible": 100,
    "shutdown_requested": 100,
    "server_stopped": 100,
    "children_stopped": 100,
}

REQUIRED_SMOKE_CHECKPOINTS = tuple(PROGRESS)


class CheckpointRecorder:
    def __init__(
        self,
        report_path: Path | None = None,
        on_checkpoint: Callable[[str, int], None] | None = None,
    ) -> None:
        self.report_path = report_path
        self.on_checkpoint = on_checkpoint
        self.names: list[str] = []

    def record(self, name: str) -> None:
        if name not in PROGRESS:
            raise ValueError(f"Unknown desktop checkpoint: {name}")
        if name in self.names:
            return
        self.names.append(name)
        event = {
            "checkpoint": name,
            "progress": PROGRESS[name],
            "timestamp": time.time(),
        }
        if self.report_path is not None:
            self.report_path.parent.mkdir(parents=True, exist_ok=True)
            with self.report_path.open("a", encoding="utf-8") as stream:
                stream.write(json.dumps(event, sort_keys=True) + "\n")
        if self.on_checkpoint is not None:
            self.on_checkpoint(name, PROGRESS[name])

    def missing_smoke_checkpoints(self) -> list[str]:
        return [name for name in REQUIRED_SMOKE_CHECKPOINTS if name not in self.names]
