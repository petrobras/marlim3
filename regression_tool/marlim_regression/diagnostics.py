from __future__ import annotations

from dataclasses import asdict
import json
from pathlib import Path
import zipfile

from .config import DATA_ROOT, ToolConfig
from .detection import DetectionReport
from .domain import ensure_within


def export_diagnostics(config: ToolConfig, detection: DetectionReport, destination: Path) -> Path:
    destination = ensure_within(destination, DATA_ROOT)
    destination.parent.mkdir(parents=True, exist_ok=True)
    sanitized = asdict(config)
    sanitized["environment"] = {key: "***" for key in config.environment}
    with zipfile.ZipFile(destination, "w", zipfile.ZIP_DEFLATED) as archive:
        archive.writestr("config.sanitized.json", json.dumps(sanitized, indent=2, ensure_ascii=False))
        archive.writestr("detection.json", json.dumps(detection.to_dict(), indent=2, ensure_ascii=False))
        for log in (DATA_ROOT / "logs").glob("*.log"):
            archive.write(log, f"logs/{log.name}")
    return destination
