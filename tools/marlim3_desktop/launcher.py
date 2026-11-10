"""Launch the bundled Streamlit GUI in a native Qt WebEngine window."""

from __future__ import annotations

import argparse
import ctypes
import json
import os
import platform
import socket
import subprocess
import sys
import time
import urllib.error
import urllib.request
from pathlib import Path

from . import APP_ID, APP_NAME, DESKTOP_FILE_NAME
from .checkpoints import CheckpointRecorder
from .logs import configure_logging
from .paths import (
    get_app_icon_path,
    get_bundle_root,
    get_engine_path,
    get_gui_app_path,
    get_logo_path,
)
from .processes import (
    WindowsJob,
    launch_server_process,
    start_parent_monitor,
    stop_process_tree,
)
from .splash import create_splash, show_startup_error, update_bootloader_splash


DEFAULT_HOST = "127.0.0.1"
SERVER_TIMEOUT = 45.0


class DesktopStartupError(RuntimeError):
    pass


class ServerExitedError(DesktopStartupError):
    pass


class ServerTimeoutError(DesktopStartupError):
    pass


def configure_platform_app_id() -> None:
    if sys.platform.startswith("win"):
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(APP_ID)


def find_free_port(host: str = DEFAULT_HOST) -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind((host, 0))
        return int(sock.getsockname()[1])


def streamlit_flag_options(port: int, host: str = DEFAULT_HOST) -> dict[str, object]:
    return {
        "server.address": host,
        "server.port": port,
        "server.headless": True,
        "server.fileWatcherType": "none",
        "browser.gatherUsageStats": False,
        "global.developmentMode": False,
    }


def configure_linux_environment(bundle_root: Path) -> None:
    gio_modules = bundle_root / "gio-modules"
    gio_modules.mkdir(exist_ok=True)
    os.environ["GIO_MODULE_DIR"] = str(gio_modules)
    os.environ["GIO_EXTRA_MODULES"] = ""
    os.environ.setdefault("QT_QUICK_BACKEND", "software")
    os.environ.setdefault("QT_WIDGETS_RHI", "0")
    os.environ.setdefault("QT_XCB_GL_INTEGRATION", "none")
    flags = os.environ.get("QTWEBENGINE_CHROMIUM_FLAGS", "").split()
    for flag in ("--disable-gpu", "--disable-gpu-compositing"):
        if flag not in flags:
            flags.append(flag)
    os.environ["QTWEBENGINE_CHROMIUM_FLAGS"] = " ".join(flags)


def configure_qt_webengine() -> None:
    from PySide6.QtCore import Qt
    from PySide6.QtWidgets import QApplication

    if QApplication.instance() is None:
        QApplication.setAttribute(Qt.AA_ShareOpenGLContexts)

    # Qt WebEngine must be imported before QApplication is created. Importing it
    # lazily after QApplication can abort the process on macOS.
    import PySide6.QtWebEngineWidgets  # noqa: F401


def configure_qt_webview() -> None:
    from PySide6.QtWebView import QtWebView

    QtWebView.initialize()


def configure_environment(bundle_root: Path, engine_path: Path) -> None:
    os.environ["MARLIM3_EXEC"] = str(engine_path)
    os.environ["MARLIM3_SKIP_EXECUTABLE_RESOLUTION"] = "1"
    os.environ.setdefault("STREAMLIT_BROWSER_GATHER_USAGE_STATS", "false")
    if sys.platform.startswith("linux"):
        configure_linux_environment(bundle_root)


def build_runtime_config(port: int | None = None, host: str = DEFAULT_HOST) -> dict[str, str | int]:
    root = get_bundle_root()
    chosen_port = port or 0
    return {
        "bundle_root": str(root),
        "gui_app": str(get_gui_app_path(root)),
        "engine": str(get_engine_path(root)),
        "app_icon": str(get_app_icon_path(root)),
        "logo": str(get_logo_path(root)),
        "host": host,
        "port": chosen_port,
        "url": f"http://{host}:{chosen_port}",
    }


def server_command(host: str, port: int) -> list[str]:
    args = ["--serve", "--host", host, "--port", str(port)]
    if not sys.platform.startswith("win"):
        args.extend(["--parent-pid", str(os.getpid())])
    if getattr(sys, "frozen", False):
        return [sys.executable, *args]
    return [sys.executable, "-m", "marlim3_desktop", *args]


def wait_for_server(
    url: str,
    process: subprocess.Popen,
    timeout: float = SERVER_TIMEOUT,
    process_events=None,
) -> None:
    health_url = f"{url}/_stcore/health"
    deadline = time.monotonic() + timeout
    last_error: Exception | None = None
    while time.monotonic() < deadline:
        return_code = process.poll()
        if return_code is not None:
            raise ServerExitedError(
                f"Streamlit exited before becoming ready (exit code {return_code})"
            )
        try:
            with urllib.request.urlopen(health_url, timeout=1.0) as response:
                if response.status == 200 and response.read().decode().strip() == "ok":
                    return
        except (OSError, urllib.error.URLError) as exc:
            last_error = exc
        if process_events is not None:
            process_events()
        time.sleep(0.2)
    raise ServerTimeoutError(
        f"Streamlit did not become ready within {timeout:.0f}s at {health_url}"
    ) from last_error


def run_streamlit_server(app_path: Path, port: int, host: str = DEFAULT_HOST) -> None:
    from streamlit.web import bootstrap

    options = streamlit_flag_options(port=port, host=host)
    bootstrap.load_config_options(options)
    bootstrap.run(str(app_path), False, [], options)


def start_server(
    host: str,
    log_path: Path,
    requested_port: int | None = None,
    process_events=None,
    on_started=None,
) -> tuple[subprocess.Popen, WindowsJob | None, str]:
    port = requested_port if requested_port is not None else find_free_port(host)
    url = f"http://{host}:{port}"
    process, job = launch_server_process(server_command(host, port), log_path)
    if on_started is not None:
        on_started()
    try:
        wait_for_server(url, process, process_events=process_events)
    except Exception:
        stop_process_tree(process, job)
        raise
    return process, job, url


def open_desktop_window(
    app,
    splash,
    url: str,
    recorder: CheckpointRecorder,
    close_after_ms: int | None = None,
) -> None:
    from PySide6.QtCore import QTimer, QUrl
    from PySide6.QtGui import QIcon
    from PySide6.QtWidgets import QMainWindow
    from PySide6.QtWebEngineWidgets import QWebEngineView

    icon = QIcon(str(get_app_icon_path()))
    if icon.isNull():
        raise DesktopStartupError(f"Invalid desktop icon: {get_app_icon_path()}")

    window = QMainWindow()
    window.setWindowTitle(APP_NAME)
    window.setMinimumSize(960, 640)
    window.setWindowIcon(icon)
    browser = QWebEngineView(window)
    window.setCentralWidget(browser)
    recorder.record("webengine_created")

    load_result = {"finished": False, "ok": False}
    load_attempt = {"count": 1}

    def loaded(ok: bool) -> None:
        if not ok and load_attempt["count"] < 5:
            load_attempt["count"] += 1
            QTimer.singleShot(350 * load_attempt["count"], lambda: browser.setUrl(QUrl(url)))
            return
        load_result["finished"] = True
        load_result["ok"] = ok
        if not ok:
            app.quit()
            return
        recorder.record("webengine_load_finished")
        window.showMaximized()
        recorder.record("main_window_visible")
        QTimer.singleShot(150, splash.close)
        if close_after_ms is not None:
            QTimer.singleShot(close_after_ms, window.close)

    browser.loadFinished.connect(loaded)
    browser.setUrl(QUrl(url))
    QTimer.singleShot(45_000, lambda: app.quit() if not load_result["finished"] else None)
    app.exec()
    browser.close()
    browser.deleteLater()
    if not load_result["finished"]:
        raise DesktopStartupError("Qt WebEngine timed out while loading the interface")
    if not load_result["ok"]:
        raise DesktopStartupError("Qt WebEngine could not load the Streamlit interface")


def open_macos_webview_window(
    app,
    splash,
    url: str,
    recorder: CheckpointRecorder,
    close_after_ms: int | None = None,
) -> None:
    from PySide6.QtCore import QSize, QTimer, QUrl
    from PySide6.QtGui import QIcon
    from PySide6.QtWebView import QWebView, QWebViewLoadingInfo

    icon = QIcon(str(get_app_icon_path()))
    if icon.isNull():
        raise DesktopStartupError(f"Invalid desktop icon: {get_app_icon_path()}")

    browser = QWebView()
    browser.setTitle(APP_NAME)
    browser.setMinimumSize(QSize(960, 640))
    browser.setIcon(icon)
    recorder.record("webengine_created")

    load_result = {"finished": False, "ok": False, "error": ""}
    load_attempt = {"count": 1}

    def retry_load() -> None:
        browser.setUrl(QUrl(url))

    def loading_changed(info: QWebViewLoadingInfo) -> None:
        status = info.status()
        if status == QWebViewLoadingInfo.LoadStatus.Failed:
            load_result["error"] = info.errorString()
            if load_attempt["count"] < 5:
                load_attempt["count"] += 1
                QTimer.singleShot(350 * load_attempt["count"], retry_load)
                return
            load_result["finished"] = True
            load_result["ok"] = False
            app.quit()
            return
        if status != QWebViewLoadingInfo.LoadStatus.Succeeded:
            return

        load_result["finished"] = True
        load_result["ok"] = True
        recorder.record("webengine_load_finished")
        browser.showMaximized()
        recorder.record("main_window_visible")
        QTimer.singleShot(150, splash.close)
        if close_after_ms is not None:
            QTimer.singleShot(close_after_ms, browser.close)
            QTimer.singleShot(close_after_ms + 100, app.quit)

    browser.loadingChanged.connect(loading_changed)
    browser.setUrl(QUrl(url))
    QTimer.singleShot(45_000, lambda: app.quit() if not load_result["finished"] else None)
    app.exec()
    browser.close()
    browser.deleteLater()
    if not load_result["finished"]:
        raise DesktopStartupError("macOS WebView timed out while loading the interface")
    if not load_result["ok"]:
        detail = f": {load_result['error']}" if load_result["error"] else ""
        raise DesktopStartupError(f"macOS WebView could not load the Streamlit interface{detail}")


def _parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Launch the Marlim3 desktop application.")
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int)
    parser.add_argument("--print-config", action="store_true")
    parser.add_argument("--serve", action="store_true", help=argparse.SUPPRESS)
    parser.add_argument("--parent-pid", type=int, help=argparse.SUPPRESS)
    parser.add_argument("--window-smoke-test", action="store_true", help=argparse.SUPPRESS)
    parser.add_argument("--checkpoint-file", type=Path, help=argparse.SUPPRESS)
    return parser.parse_args(argv)


def _validate_required_files(config: dict[str, str | int]) -> None:
    required = {
        "Streamlit app": Path(str(config["gui_app"])),
        "Marlim3 engine": Path(str(config["engine"])),
        "desktop icon": Path(str(config["app_icon"])),
        "Marlim3 logo": Path(str(config["logo"])),
    }
    for description, path in required.items():
        if not path.is_file():
            raise FileNotFoundError(f"{description} not found: {path}")


def _run(argv: list[str] | None = None) -> int:
    args = _parse_args(argv)
    logger, log_path = configure_logging()
    logger.info("Starting Marlim3 desktop on %s %s", platform.system(), platform.machine())
    config = build_runtime_config(args.port, args.host)

    if args.print_config:
        print(json.dumps(config, indent=2, sort_keys=True))
        return 0

    _validate_required_files(config)
    bundle_root = Path(str(config["bundle_root"]))
    app_path = Path(str(config["gui_app"]))
    engine_path = Path(str(config["engine"]))
    configure_environment(bundle_root, engine_path)

    if args.serve:
        if args.port is None:
            raise ValueError("--serve requires --port")
        start_parent_monitor(args.parent_pid)
        run_streamlit_server(app_path, args.port, args.host)
        return 0

    configure_platform_app_id()
    if sys.platform == "darwin":
        configure_qt_webview()
    else:
        configure_qt_webengine()
    from PySide6.QtGui import QIcon
    from PySide6.QtWidgets import QApplication

    app = QApplication.instance() or QApplication(sys.argv[:1])
    app.setApplicationDisplayName(APP_NAME)
    app.setApplicationName(APP_NAME)
    app.setDesktopFileName(DESKTOP_FILE_NAME)
    app.setQuitOnLastWindowClosed(True)
    app.setWindowIcon(QIcon(str(get_app_icon_path(bundle_root))))

    recorder = CheckpointRecorder(
        args.checkpoint_file,
        on_checkpoint=lambda name, progress: logger.info(
            "checkpoint=%s progress=%s", name, progress
        ),
    )
    recorder.record("launcher_started")
    update_bootloader_splash("Starting Marlim3...")
    splash = create_splash(
        app,
        get_logo_path(bundle_root),
        get_app_icon_path(bundle_root),
    )

    def update_startup_progress(name: str, progress: int) -> None:
        logger.info("checkpoint=%s progress=%s", name, progress)
        splash.set_progress(name, progress)

    recorder.on_checkpoint = update_startup_progress
    recorder.record("splash_created")
    recorder.record("runtime_configured")

    process = None
    job = None
    try:
        process, job, url = start_server(
            args.host,
            log_path,
            requested_port=args.port,
            process_events=app.processEvents,
            on_started=lambda: recorder.record("server_process_started"),
        )
        recorder.record("health_endpoint_ready")
        if sys.platform == "darwin":
            open_macos_webview_window(
                app,
                splash,
                url,
                recorder,
                close_after_ms=2500 if args.window_smoke_test else None,
            )
        else:
            open_desktop_window(
                app,
                splash,
                url,
                recorder,
                close_after_ms=2500 if args.window_smoke_test else None,
            )
        recorder.record("shutdown_requested")
    finally:
        if process is not None:
            stop_process_tree(process, job)
            recorder.record("server_stopped")
            recorder.record("children_stopped")

    if args.window_smoke_test:
        missing = recorder.missing_smoke_checkpoints()
        if missing:
            raise DesktopStartupError(
                f"Desktop smoke test missed checkpoints: {', '.join(missing)}"
            )
    return 0


def main(argv: list[str] | None = None) -> int:
    try:
        return _run(argv)
    except Exception as exc:
        logger, log_path = configure_logging()
        logger.exception("Desktop startup failed")
        arguments = argv or sys.argv[1:]
        if not any(
            flag in arguments
            for flag in ("--print-config", "--serve", "--window-smoke-test")
        ):
            try:
                show_startup_error(log_path, str(exc))
            except Exception:
                pass
        return 1
