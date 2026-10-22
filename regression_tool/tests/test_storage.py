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

    def test_discovers_manifest_models_without_persisting_them(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            demos = root / "demos"
            (demos / "pt-br").mkdir(parents=True)
            (demos / "pt-br" / "producao.mr3").write_text("modelo", encoding="utf-8")
            (demos / "pt-br" / "rede.json").write_text("{}", encoding="utf-8")
            (demos / "pt-br" / "trecho-0.json").write_text("{}", encoding="utf-8")
            (demos / "PVTSIM-MARLIM.tab").write_text("pvt", encoding="utf-8")
            (demos / "solto.json").write_text("{}", encoding="utf-8")
            manifest = [
                {"model": "pt-br/producao.mr3", "aux": ["PVTSIM-MARLIM.tab"]},
                {"model": "pt-br/rede.json", "mode": "REDE", "aux": ["trecho-0.json"]},
            ]
            (demos / "manifest.json").write_text(json.dumps(manifest), encoding="utf-8")
            repository = StorageRepository(root / "tool.db")
            discovered = {test.model_path: test for test in discover_project_demos(root)}

            self.assertEqual(set(discovered), {"demos/pt-br/producao.mr3", "demos/pt-br/rede.json"})
            self.assertEqual(repository.list_tests(), [])
            self.assertEqual(discovered["demos/pt-br/producao.mr3"].auxiliary_files, ["demos/PVTSIM-MARLIM.tab"])
            self.assertEqual(discovered["demos/pt-br/producao.mr3"].arguments, [])
            rede = discovered["demos/pt-br/rede.json"]
            self.assertEqual(rede.auxiliary_files, ["demos/pt-br/trecho-0.json"])
            self.assertEqual(rede.arguments[-2:], ["-s", "REDE"])

    def test_no_manifest_means_no_demos(self):
        with tempfile.TemporaryDirectory() as temporary:
            (Path(temporary) / "demos").mkdir()
            self.assertEqual(discover_project_demos(Path(temporary)), [])


if __name__ == "__main__":
    unittest.main()
