from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from marlim_regression.icons import APP_ID, WINDOW_CLASS, prepare_linux_desktop_icon


class IconIntegrationTests(unittest.TestCase):
    def test_linux_desktop_metadata_uses_packaged_icon_and_window_class(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            with (
                patch("marlim_regression.icons.platform.system", return_value="Linux"),
                patch("marlim_regression.icons._linux_data_homes", return_value=(root / "xdg",)),
                patch("marlim_regression.icons._refresh_linux_desktop_caches") as refresh_caches,
            ):
                prepare_linux_desktop_icon("/usr/bin/google-chrome")

            installed_icon = root / "xdg" / "icons" / "hicolor" / "48x48" / "apps" / "regression-tool.png"
            desktop_file = root / "xdg" / "applications" / f"{APP_ID}.desktop"
            self.assertTrue(installed_icon.is_file())
            content = desktop_file.read_text(encoding="utf-8")
            self.assertIn("Exec=/usr/bin/google-chrome", content)
            self.assertIn("Name=Regression Tool", content)
            self.assertIn("Icon=regression-tool", content)
            self.assertIn(f"StartupWMClass={WINDOW_CLASS}", content)
            refresh_caches.assert_called_once_with(root / "xdg")


if __name__ == "__main__":
    unittest.main()
