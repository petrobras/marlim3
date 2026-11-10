from pathlib import Path
import tempfile
import unittest

from marlim_regression.workflow import WorkflowStateStore


class WorkflowStateTests(unittest.TestCase):
    def test_ready_requires_both_confirmations_and_resets_independently(self):
        with tempfile.TemporaryDirectory() as temporary:
            store = WorkflowStateStore(Path(temporary) / "workflow.json")
            self.assertFalse(store.load().ready)
            self.assertFalse(store.update(validated=True).ready)
            self.assertTrue(store.update(compiled=True).ready)
            state = store.update(validated=False)
            self.assertFalse(state.ready)
            self.assertTrue(state.compiled)
            self.assertFalse(store.load().validated)


if __name__ == "__main__":
    unittest.main()
