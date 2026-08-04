from pathlib import Path
import tempfile
import unittest

from marlim_regression.domain import TestCase as RegressionCase
from marlim_regression.storage import StorageRepository, discover_project_demos


class StorageTests(unittest.TestCase):
    def test_crud_and_schema_migration(self):
        with tempfile.TemporaryDirectory() as temporary:
            repository = StorageRepository(Path(temporary) / "tool.db")
            test = RegressionCase("case A", "input.mr3", suite="smoke")
            repository.save_test(test)
            self.assertEqual(repository.get_test(test.id).name, "case A")
            self.assertEqual([item.id for item in repository.list_tests(suite="smoke")], [test.id])
            repository.delete_test(test.id)
            self.assertIsNone(repository.get_test(test.id))
            with repository.connect() as db:
                self.assertEqual(db.execute("SELECT version FROM schema_version").fetchone()[0], 2)

    def test_discovers_non_translated_demo_models_without_persisting_them(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            demos = root / "demos"
            (demos / "pt-br").mkdir(parents=True)
            (demos / "english.mr3").write_text('"pvtFile": "PVTSIM-MARLIM.tab"', encoding="utf-8")
            (demos / "legacy.json").write_text("{}", encoding="utf-8")
            (demos / "PVTSIM-MARLIM.tab").write_text("pvt", encoding="utf-8")
            (demos / "notes.txt").write_text("ignore", encoding="utf-8")
            (demos / "pt-br" / "portugues.mr3").write_text("modelo", encoding="utf-8")
            repository = StorageRepository(root / "tool.db")
            discovered = discover_project_demos(root)

            self.assertEqual(len(discovered), 2)
            self.assertEqual(
                {test.model_path for test in discovered},
                {"demos/english.mr3", "demos/legacy.json"},
            )
            self.assertEqual(repository.list_tests(), [])
            english = next(test for test in discovered if test.model_path.endswith("english.mr3"))
            self.assertEqual(english.auxiliary_files, ["demos/PVTSIM-MARLIM.tab"])
            (demos / "added.mr3").write_text("model", encoding="utf-8")
            self.assertEqual(len(discover_project_demos(root)), 3)


if __name__ == "__main__":
    unittest.main()
