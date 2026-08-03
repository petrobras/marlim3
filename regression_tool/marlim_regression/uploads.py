"""Model upload handling shared by the single and batch registration flows."""

from __future__ import annotations

import base64
import binascii
from dataclasses import dataclass
from pathlib import Path
import uuid

from .config import DATA_ROOT
from .domain import TestCase, ensure_within


MODEL_SUFFIXES = frozenset({".mr3", ".json"})
MAX_MODEL_BYTES = 64 * 1024 * 1024


@dataclass(frozen=True)
class ModelUpload:
    """A decoded model file waiting to be registered as a test case."""

    filename: str
    content: bytes

    @property
    def test_name(self) -> str:
        """The registered name always mirrors the input file name."""
        return Path(self.filename).stem


def decode_upload(payload: dict) -> ModelUpload:
    """Validate and decode one uploaded model, rejecting unusable input early."""
    filename = Path(str(payload.get("name", ""))).name
    if not filename or Path(filename).suffix.lower() not in MODEL_SUFFIXES:
        raise ValueError(f"Select a .mr3 or .json model: {filename or 'unnamed file'}")
    try:
        content = base64.b64decode(str(payload.get("content_base64", "")), validate=True)
    except (ValueError, binascii.Error) as exc:
        raise ValueError(f"Invalid model content: {filename}") from exc
    if not content:
        raise ValueError(f"The model file is empty: {filename}")
    if len(content) > MAX_MODEL_BYTES:
        raise ValueError(f"The model exceeds the 64 MB limit: {filename}")
    return ModelUpload(filename, content)


class ModelUploadStore:
    """Persists uploaded models inside the user-private data directory."""

    def __init__(self, data_root: Path = DATA_ROOT):
        self.data_root = data_root

    def store(self, test_id: str, upload: ModelUpload) -> str:
        directory = ensure_within(self.data_root / "inputs" / "models" / test_id, self.data_root)
        destination = ensure_within(directory / upload.filename, directory)
        directory.mkdir(parents=True, exist_ok=True)
        destination.write_bytes(upload.content)
        return f"@data/{destination.relative_to(self.data_root).as_posix()}"

    def build_test_case(self, upload: ModelUpload, *, suite: str = "custom", test_id: str = "") -> TestCase:
        """Create a test case whose name is the model file name, as required by the UI."""
        identifier = test_id or str(uuid.uuid4())
        return TestCase(
            name=upload.test_name,
            model_path=self.store(identifier, upload),
            id=identifier,
            suite=suite.strip() or "custom",
        )
