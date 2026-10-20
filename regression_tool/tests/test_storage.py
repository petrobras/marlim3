import json
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

    def test_discovers_manifest_demo_models_without_persisting_them(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            demos = root / "demos"
            (demos / "case").mkdir(parents=True)
            (demos / "case" / "modelo.mr3").write_text("{}", encoding="utf-8")
            (demos / "case" / "english.mr3").write_text("{}", encoding="utf-8")
            (demos / "case" / "PVTSIM-MARLIM.tab").write_text("pvt", encoding="utf-8")
            (demos / "case" / "leitura.json").write_text("{}", encoding="utf-8")
            (demos / "legacy.mr3").write_text("{}", encoding="utf-8")
            (demos / "unlisted.mr3").write_text("{}", encoding="utf-8")
            manifest = [
                {"model": "case/modelo.mr3", "native": "case/english.mr3", "aux": ["PVTSIM-MARLIM.tab"]},
                {"model": "legacy.mr3"},
            ]
            (demos / "manifest.json").write_text(json.dumps(manifest), encoding="utf-8")
            repository = StorageRepository(root / "tool.db")
            discovered = discover_project_demos(root)

            self.assertEqual(
                [test.model_path for test in discovered],
                ["demos/case/english.mr3", "demos/legacy.mr3"],
            )
            self.assertEqual(repository.list_tests(), [])
            self.assertEqual(discovered[0].auxiliary_files, ["demos/case/PVTSIM-MARLIM.tab"])
            manifest.append({"model": "unlisted.mr3"})
            (demos / "manifest.json").write_text(json.dumps(manifest), encoding="utf-8")
            self.assertEqual(len(discover_project_demos(root)), 3)


if __name__ == "__main__":
    unittest.main()
