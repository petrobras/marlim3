from pathlib import Path
import unittest

from marlim_regression.build import ShadowSourceBuilder
from marlim_regression.config import ToolConfig


class ShadowBuildTests(unittest.TestCase):
    def test_shadow_contains_only_required_inputs(self):
        project = Path(__file__).resolve().parents[2]
        source_header = project / "src" / "include" / "versao.h"
        before = source_header.read_bytes() if source_header.exists() else None
        shadow = ShadowSourceBuilder(ToolConfig(project_root=str(project))).prepare_shadow()
        self.assertTrue((shadow / "CMakeLists.txt").is_file())
        self.assertTrue((shadow / "src" / "core" / "Num4Main.cpp").is_file())
        self.assertFalse((shadow / ".git").exists())
        after = source_header.read_bytes() if source_header.exists() else None
        self.assertEqual(before, after)


if __name__ == "__main__":
    unittest.main()
