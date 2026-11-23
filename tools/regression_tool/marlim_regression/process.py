from __future__ import annotations

from datetime import datetime, timezone
import os
from pathlib import Path
import signal
import subprocess
import threading
import time
from typing import Callable

from .domain import ProcessResult


class ProcessExecutor:
    """Direct-process executor with live output capture and tree cancellation."""

    def __init__(self) -> None:
        self._process: subprocess.Popen[str] | None = None
        self._lock = threading.Lock()

    @staticmethod
    def _timestamp() -> str:
        return datetime.now(timezone.utc).isoformat()

    def cancel(self) -> bool:
        with self._lock:
            process = self._process
        if process is None or process.poll() is not None:
            return False
        try:
            if os.name == "nt":
                process.send_signal(signal.CTRL_BREAK_EVENT)
            else:
                os.killpg(process.pid, signal.SIGTERM)
        except OSError:
            process.terminate()
        return True

    def run(
        self,
        command: list[str],
        cwd: Path,
        environment: dict[str, str] | None = None,
        timeout: float | None = None,
        on_output: Callable[[str, str], None] | None = None,
    ) -> ProcessResult:
        if not command or any(not isinstance(part, str) for part in command):
            raise ValueError("Command must be a non-empty list of strings")
        cwd = cwd.resolve()
        cwd.mkdir(parents=True, exist_ok=True)
        env = os.environ.copy()
        env.update(environment or {})
        started_at = self._timestamp()
        started = time.monotonic()
        creationflags = subprocess.CREATE_NEW_PROCESS_GROUP if os.name == "nt" else 0
        popen_kwargs = {"start_new_session": True} if os.name != "nt" else {}
        process = subprocess.Popen(
            command,
            cwd=str(cwd),
            env=env,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            errors="replace",
            creationflags=creationflags,
            **popen_kwargs,
        )
        with self._lock:
            self._process = process
        stdout_chunks: list[str] = []
        stderr_chunks: list[str] = []

        def consume(stream: object, name: str, chunks: list[str]) -> None:
            if stream is None:
                return
            for chunk in iter(stream.readline, ""):
                chunks.append(chunk)
                if on_output:
                    on_output(name, chunk)

        readers = [
            threading.Thread(target=consume, args=(process.stdout, "stdout", stdout_chunks), daemon=True),
            threading.Thread(target=consume, args=(process.stderr, "stderr", stderr_chunks), daemon=True),
        ]
        for reader in readers:
            reader.start()

        timed_out = False
        cancelled = False
        try:
            process.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            timed_out = True
            self.cancel()
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
        finally:
            for reader in readers:
                reader.join(timeout=5)
            with self._lock:
                cancelled = self._process is process and process.returncode is not None and process.returncode in (-signal.SIGINT, -signal.SIGTERM)
                self._process = None
        stdout = "".join(stdout_chunks)
        stderr = "".join(stderr_chunks)
        finished_at = self._timestamp()
        duration = time.monotonic() - started
        code = process.returncode
        crashed = bool(code is not None and code < 0 and not timed_out and not cancelled)
        generated = [str(path.relative_to(cwd)) for path in cwd.rglob("*") if path.is_file()]
        return ProcessResult(
            command=list(command), cwd=str(cwd), exit_code=code, stdout=stdout, stderr=stderr,
            started_at=started_at, finished_at=finished_at, duration_seconds=duration,
            timed_out=timed_out, cancelled=cancelled and not timed_out, crashed=crashed,
            generated_files=generated,
        )
