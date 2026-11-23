"""Cross-platform process management for the desktop launcher."""

from __future__ import annotations

import ctypes
import os
import signal
import subprocess
import sys
import threading
import time
from pathlib import Path


CREATE_NEW_PROCESS_GROUP = getattr(subprocess, "CREATE_NEW_PROCESS_GROUP", 0x00000200)
CREATE_NO_WINDOW = getattr(subprocess, "CREATE_NO_WINDOW", 0x08000000)


def hidden_process_flags(*, new_process_group: bool = False) -> int:
    if not sys.platform.startswith("win"):
        return 0
    flags = CREATE_NO_WINDOW
    if new_process_group:
        flags |= CREATE_NEW_PROCESS_GROUP
    return flags


class WindowsJob:
    """Own a Windows job that terminates assigned children when closed."""

    JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE = 0x00002000
    JOB_OBJECT_EXTENDED_LIMIT_INFORMATION_CLASS = 9

    def __init__(self) -> None:
        self.handle = None
        if not sys.platform.startswith("win"):
            return

        from ctypes import wintypes

        class IO_COUNTERS(ctypes.Structure):
            _fields_ = [
                ("ReadOperationCount", ctypes.c_ulonglong),
                ("WriteOperationCount", ctypes.c_ulonglong),
                ("OtherOperationCount", ctypes.c_ulonglong),
                ("ReadTransferCount", ctypes.c_ulonglong),
                ("WriteTransferCount", ctypes.c_ulonglong),
                ("OtherTransferCount", ctypes.c_ulonglong),
            ]

        class BASIC_LIMIT_INFORMATION(ctypes.Structure):
            _fields_ = [
                ("PerProcessUserTimeLimit", ctypes.c_longlong),
                ("PerJobUserTimeLimit", ctypes.c_longlong),
                ("LimitFlags", wintypes.DWORD),
                ("MinimumWorkingSetSize", ctypes.c_size_t),
                ("MaximumWorkingSetSize", ctypes.c_size_t),
                ("ActiveProcessLimit", wintypes.DWORD),
                ("Affinity", ctypes.c_size_t),
                ("PriorityClass", wintypes.DWORD),
                ("SchedulingClass", wintypes.DWORD),
            ]

        class EXTENDED_LIMIT_INFORMATION(ctypes.Structure):
            _fields_ = [
                ("BasicLimitInformation", BASIC_LIMIT_INFORMATION),
                ("IoInfo", IO_COUNTERS),
                ("ProcessMemoryLimit", ctypes.c_size_t),
                ("JobMemoryLimit", ctypes.c_size_t),
                ("PeakProcessMemoryUsed", ctypes.c_size_t),
                ("PeakJobMemoryUsed", ctypes.c_size_t),
            ]

        kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
        kernel32.CreateJobObjectW.restype = wintypes.HANDLE
        handle = kernel32.CreateJobObjectW(None, None)
        if not handle:
            raise ctypes.WinError(ctypes.get_last_error())

        info = EXTENDED_LIMIT_INFORMATION()
        info.BasicLimitInformation.LimitFlags = self.JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
        ok = kernel32.SetInformationJobObject(
            handle,
            self.JOB_OBJECT_EXTENDED_LIMIT_INFORMATION_CLASS,
            ctypes.byref(info),
            ctypes.sizeof(info),
        )
        if not ok:
            kernel32.CloseHandle(handle)
            raise ctypes.WinError(ctypes.get_last_error())
        self.handle = handle

    def assign(self, process: subprocess.Popen) -> None:
        if self.handle is None:
            return
        kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
        if not kernel32.AssignProcessToJobObject(self.handle, process._handle):
            raise ctypes.WinError(ctypes.get_last_error())

    def close(self) -> None:
        if self.handle is None:
            return
        ctypes.WinDLL("kernel32", use_last_error=True).CloseHandle(self.handle)
        self.handle = None


def monitor_posix_parent(parent_pid: int, interval: float = 0.5) -> None:
    if sys.platform.startswith("win"):
        return
    while True:
        try:
            os.kill(parent_pid, 0)
        except (OSError, ProcessLookupError):
            os._exit(0)
        time.sleep(interval)


def start_parent_monitor(parent_pid: int | None) -> None:
    if parent_pid is None or sys.platform.startswith("win"):
        return
    threading.Thread(
        target=monitor_posix_parent,
        args=(parent_pid,),
        daemon=True,
    ).start()


def stop_process_tree(process: subprocess.Popen, job: WindowsJob | None = None) -> None:
    if process.poll() is not None:
        if job is not None:
            job.close()
        _close_process_log(process)
        return

    if sys.platform.startswith("win"):
        if job is not None:
            job.close()
        else:
            process.terminate()
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait(timeout=5)
        _close_process_log(process)
        return

    try:
        os.killpg(process.pid, signal.SIGTERM)
    except ProcessLookupError:
        pass
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        process.wait(timeout=5)
    _close_process_log(process)


def _close_process_log(process: subprocess.Popen) -> None:
    stream = getattr(process, "_marlim_log_stream", None)
    if stream is not None and not stream.closed:
        stream.close()


def launch_server_process(
    command: list[str],
    log_path: Path,
) -> tuple[subprocess.Popen, WindowsJob | None]:
    log_stream = log_path.open("a", encoding="utf-8")
    kwargs: dict[str, object] = {
        "stdin": subprocess.DEVNULL,
        "stdout": log_stream,
        "stderr": subprocess.STDOUT,
        "text": True,
    }
    job = None
    if sys.platform.startswith("win"):
        kwargs["creationflags"] = hidden_process_flags(new_process_group=True)
    else:
        kwargs["start_new_session"] = True

    try:
        process = subprocess.Popen(command, **kwargs)
        process._marlim_log_stream = log_stream
        if sys.platform.startswith("win"):
            job = WindowsJob()
            job.assign(process)
        return process, job
    except Exception:
        log_stream.close()
        if job is not None:
            job.close()
        raise
