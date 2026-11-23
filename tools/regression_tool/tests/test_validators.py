from datetime import datetime, timezone
import hashlib
from pathlib import Path
import tempfile
import unittest

from marlim_regression.domain import ProcessResult, ValidationRule
from marlim_regression.validators import ValidatorRegistry


class ValidatorTests(unittest.TestCase):
    def process(self) -> ProcessResult:
        now = datetime.now(timezone.utc).isoformat()
        return ProcessResult(["fixture"], ".", 0, "answer=42", "", now, now, 0.1)

    def test_core_validators(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            value = root / "result.txt"; value.write_text("stable", encoding="utf-8")
            digest = hashlib.sha256(b"stable").hexdigest()
            rules = [
                ValidationRule("exit_code", 0), ValidationRule("no_crash", True),
                ValidationRule("stdout_contains", "answer"), ValidationRule("stdout_regex", pattern=r"answer=\d+"),
                ValidationRule("file_exists", path="result.txt"), ValidationRule("file_hash", expected=digest, path="result.txt"),
                ValidationRule("max_duration", 1.0),
            ]
            results = ValidatorRegistry().validate(rules, self.process(), root)
        self.assertTrue(all(item.passed for item in results), results)

    def test_directory_escape_is_rejected(self):
        with tempfile.TemporaryDirectory() as temporary:
            with self.assertRaises(ValueError):
                ValidatorRegistry().validate([ValidationRule("file_exists", path="../outside")], self.process(), Path(temporary))


if __name__ == "__main__":
    unittest.main()

