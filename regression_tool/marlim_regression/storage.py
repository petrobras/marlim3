from __future__ import annotations

from dataclasses import asdict
import json
from pathlib import Path
import sqlite3
from typing import Iterable
import uuid

from .domain import RunRecord, TestCase


SCHEMA_VERSION = 2


def discover_project_demos(project_root: Path) -> list[TestCase]:
    """Read the current project demos directly without copying them into local state."""
    demos_root = project_root / "demos"
    if not demos_root.is_dir():
        return []
    pvt_file = demos_root / "PVTSIM-MARLIM.tab"
    tests: list[TestCase] = []
    for path in sorted(demos_root.rglob("*")):
        if (
            not path.is_file()
            or path.suffix.lower() not in {".mr3", ".json"}
            or "pt-br" in {part.lower() for part in path.relative_to(demos_root).parts}
        ):
            continue
        model_path = path.relative_to(project_root).as_posix()
        requires_pvt = (
            path.suffix.lower() == ".mr3"
            and "PVTSIM-MARLIM.tab" in path.read_text(encoding="utf-8", errors="replace")
        )
        auxiliary = [pvt_file.relative_to(project_root).as_posix()] if requires_pvt and pvt_file.is_file() else []
        tests.append(TestCase(
            path.stem,
            model_path,
            id=str(uuid.uuid5(uuid.NAMESPACE_URL, f"marlim3-demo:{model_path}")),
            suite="demos",
            auxiliary_files=auxiliary,
        ))
    return tests


class StorageRepository:
    def __init__(self, path: Path):
        self.path = path
        path.parent.mkdir(parents=True, exist_ok=True)
        self._migrate()

    def connect(self) -> sqlite3.Connection:
        connection = sqlite3.connect(self.path)
        connection.row_factory = sqlite3.Row
        return connection

    def _migrate(self) -> None:
        with self.connect() as db:
            db.execute("CREATE TABLE IF NOT EXISTS schema_version (version INTEGER NOT NULL)")
            current = db.execute("SELECT version FROM schema_version ORDER BY version DESC LIMIT 1").fetchone()
            version = int(current[0]) if current else 0
            if version < 1:
                db.executescript("""
                    CREATE TABLE tests (
                        id TEXT PRIMARY KEY,
                        name TEXT NOT NULL,
                        suite TEXT NOT NULL,
                        active INTEGER NOT NULL,
                        payload TEXT NOT NULL,
                        updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
                    );
                    CREATE TABLE runs (
                        id TEXT PRIMARY KEY,
                        test_id TEXT NOT NULL,
                        status TEXT NOT NULL,
                        started_at TEXT NOT NULL,
                        payload TEXT NOT NULL,
                        FOREIGN KEY(test_id) REFERENCES tests(id)
                    );
                    CREATE INDEX runs_test_started ON runs(test_id, started_at DESC);
                    DELETE FROM schema_version;
                    INSERT INTO schema_version(version) VALUES (1);
                """)
                version = 1
            if version < 2:
                db.executescript("""
                    CREATE TABLE metadata (
                        key TEXT PRIMARY KEY,
                        value TEXT NOT NULL
                    );
                    DELETE FROM schema_version;
                    INSERT INTO schema_version(version) VALUES (2);
                """)

    def metadata(self, key: str) -> str | None:
        with self.connect() as db:
            row = db.execute("SELECT value FROM metadata WHERE key=?", (key,)).fetchone()
        return str(row[0]) if row else None

    def set_metadata(self, key: str, value: str) -> None:
        with self.connect() as db:
            db.execute(
                "INSERT INTO metadata(key,value) VALUES(?,?) "
                "ON CONFLICT(key) DO UPDATE SET value=excluded.value",
                (key, value),
            )

    def save_test(self, test: TestCase) -> None:
        payload = json.dumps(test.to_dict(), ensure_ascii=False)
        with self.connect() as db:
            db.execute(
                "INSERT INTO tests(id,name,suite,active,payload) VALUES(?,?,?,?,?) "
                "ON CONFLICT(id) DO UPDATE SET name=excluded.name,suite=excluded.suite,active=excluded.active,payload=excluded.payload,updated_at=CURRENT_TIMESTAMP",
                (test.id, test.name, test.suite, int(test.active), payload),
            )

    def list_tests(self, query: str = "", suite: str | None = None, active: bool | None = None) -> list[TestCase]:
        clauses, params = [], []
        if query:
            clauses.append("(name LIKE ? OR payload LIKE ?)")
            params.extend([f"%{query}%", f"%{query}%"])
        if suite:
            clauses.append("suite = ?")
            params.append(suite)
        if active is not None:
            clauses.append("active = ?")
            params.append(int(active))
        sql = "SELECT payload FROM tests" + (" WHERE " + " AND ".join(clauses) if clauses else "") + " ORDER BY suite,name"
        with self.connect() as db:
            return [TestCase.from_dict(json.loads(row[0])) for row in db.execute(sql, params)]

    def get_test(self, test_id: str) -> TestCase | None:
        with self.connect() as db:
            row = db.execute("SELECT payload FROM tests WHERE id=?", (test_id,)).fetchone()
        return TestCase.from_dict(json.loads(row[0])) if row else None

    def delete_test(self, test_id: str) -> None:
        with self.connect() as db:
            db.execute("DELETE FROM runs WHERE test_id=?", (test_id,))
            db.execute("DELETE FROM tests WHERE id=?", (test_id,))

    def save_run(self, run: RunRecord) -> None:
        payload = json.dumps(asdict(run), ensure_ascii=False)
        with self.connect() as db:
            db.execute(
                "INSERT OR REPLACE INTO runs(id,test_id,status,started_at,payload) VALUES(?,?,?,?,?)",
                (run.id, run.test_id, run.status.value, run.process.started_at, payload),
            )

    def list_run_payloads(self, test_ids: Iterable[str] | None = None, limit: int = 100) -> list[dict]:
        ids = list(test_ids or [])
        params: list[object] = []
        where = ""
        if ids:
            where = f" WHERE test_id IN ({','.join('?' for _ in ids)})"
            params.extend(ids)
        params.append(limit)
        with self.connect() as db:
            return [json.loads(row[0]) for row in db.execute(f"SELECT payload FROM runs{where} ORDER BY started_at DESC LIMIT ?", params)]

    def latest_run_payloads(self, test_ids: Iterable[str] | None = None) -> list[dict]:
        ids = list(test_ids or [])
        where = ""
        params: list[object] = []
        if ids:
            where = f" WHERE test_id IN ({','.join('?' for _ in ids)})"
            params.extend(ids)
        sql = (
            "SELECT r.payload FROM runs r JOIN ("
            f"SELECT test_id, MAX(started_at) AS started_at FROM runs{where} GROUP BY test_id"
            ") latest ON latest.test_id=r.test_id AND latest.started_at=r.started_at "
            "ORDER BY r.started_at DESC"
        )
        with self.connect() as db:
            return [json.loads(row[0]) for row in db.execute(sql, params)]
