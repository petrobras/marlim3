from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from marlim_regression.config import project_data_root


class RuntimePathTests(unittest.TestCase):
    def test_project_state_is_external_and_scoped_per_checkout(self):
        with tempfile.TemporaryDirectory() as temporary:
            base = Path(temporary) / "user-state"
            with patch("marlim_regression.config.user_data_home", return_value=base):
                first = project_data_root(Path(temporary) / "checkout-a" / "marlim3")
                second = project_data_root(Path(temporary) / "checkout-b" / "marlim3")

        self.assertEqual(first.parent, base)
        self.assertEqual(second.parent, base)
        self.assertNotEqual(first, second)
        self.assertTrue(first.name.startswith("marlim3-"))


if __name__ == "__main__":
    unittest.main()
