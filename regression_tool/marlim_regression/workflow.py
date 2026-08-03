from __future__ import annotations

from dataclasses import asdict, dataclass
import json
from pathlib import Path
import threading


@dataclass(frozen=True)
class WorkflowState:
    validated: bool = False
    compiled: bool = False

    @property
    def ready(self) -> bool:
        return self.validated and self.compiled

    def to_dict(self) -> dict[str, bool]:
        return asdict(self) | {"ready": self.ready}


class WorkflowStateStore:
    """Persist the explicit UI confirmations without touching the project tree."""

    def __init__(self, path: Path):
        self.path = path
        self._lock = threading.RLock()

    def load(self) -> WorkflowState:
        with self._lock:
            if not self.path.is_file():
                return WorkflowState()
            try:
                value = json.loads(self.path.read_text(encoding="utf-8"))
            except (OSError, ValueError, json.JSONDecodeError):
                return WorkflowState()
            return WorkflowState(
                validated=bool(value.get("validated", False)),
                compiled=bool(value.get("compiled", False)),
            )

    def update(self, *, validated: bool | None = None, compiled: bool | None = None) -> WorkflowState:
        with self._lock:
            current = self.load()
            next_state = WorkflowState(
                validated=current.validated if validated is None else validated,
                compiled=current.compiled if compiled is None else compiled,
            )
            self.path.parent.mkdir(parents=True, exist_ok=True)
            temporary = self.path.with_suffix(".tmp")
            temporary.write_text(json.dumps(asdict(next_state), indent=2), encoding="utf-8")
            temporary.replace(self.path)
            return next_state
