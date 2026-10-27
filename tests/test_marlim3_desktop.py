import json
import os
import subprocess
import sys
import types
from pathlib import Path
from unittest.mock import Mock

os.environ.setdefault("MARLIM3_SKIP_EXECUTABLE_RESOLUTION", "1")

from marlim3_desktop import build, checkpoints, launcher, logs, paths, processes


def test_desktop_paths_resolve_from_bundle_override(monkeypatch, tmp_path):
    monkeypatch.setenv("MARLIM3_DESKTOP_ROOT", str(tmp_path))

    assert paths.get_bundle_root() == tmp_path.resolve()
    assert paths.get_gui_app_path() == tmp_path / "gui" / "app.py"
    assert paths.get_engine_path().parent == tmp_path / "marlim3"
    assert paths.get_app_icon_path() == (
        tmp_path / "marlim3_desktop" / "assets" / "icons" / "app-icon.png"
    )
    assert paths.get_logo_path() == tmp_path / "assets" / "branding" / "logo.svg"


def test_desktop_uses_prepared_environment_without_sync(monkeypatch):
    monkeypatch.setenv("MARLIM_DEPENDENCIES_PREPARED", "1")
    monkeypatch.setattr(build, "require_tool", lambda name: None)
    run = Mock()
    monkeypatch.setattr(build, "run", run)
    build.sync_dependencies()
    run.assert_not_called()


def test_desktop_corporate_sync_uses_dependency_helper(monkeypatch):
    monkeypatch.delenv("MARLIM_DEPENDENCIES_PREPARED", raising=False)
    monkeypatch.setenv("MARLIM_DEPENDENCY_MODE", "corporate")
    monkeypatch.setattr(build, "require_tool", lambda name: None)
    run = Mock()
    monkeypatch.setattr(build, "run", run)
    build.sync_dependencies()
    command = run.call_args.args[0]
    assert command[0] == sys.executable
    assert Path(command[1]) == build.ROOT / "scripts" / "dependencies.py"
    assert command[2:] == [
        "sync", "--mode", "corporate", "--no-dev",
        "--group", "gui", "--group", "desktop",
    ]


def test_pyinstaller_does_not_resolve_dependencies_again(monkeypatch, tmp_path):
    monkeypatch.setattr(build, "DIST_DIR", tmp_path / "dist")
    monkeypatch.setattr(build, "require_tool", lambda name: None)
    run = Mock()
    monkeypatch.setattr(build, "run", run)
    build.build_pyinstaller_bundle()
    assert run.call_args.args[0][:3] == ["uv", "run", "--no-sync"]


def test_configure_environment_sets_desktop_defaults(monkeypatch, tmp_path):
    for name in (
        "MARLIM3_EXEC",
        "MARLIM3_SKIP_EXECUTABLE_RESOLUTION",
        "GIO_MODULE_DIR",
        "GIO_EXTRA_MODULES",
        "QT_QUICK_BACKEND",
        "QT_WIDGETS_RHI",
        "QT_XCB_GL_INTEGRATION",
        "QTWEBENGINE_CHROMIUM_FLAGS",
    ):
        monkeypatch.delenv(name, raising=False)

    engine = tmp_path / "marlim3" / paths.executable_name()
    launcher.configure_environment(tmp_path, engine)

    assert os.environ["MARLIM3_EXEC"] == str(engine)
    assert os.environ["MARLIM3_SKIP_EXECUTABLE_RESOLUTION"] == "1"
    if launcher.sys.platform.startswith("linux"):
        assert os.environ["GIO_MODULE_DIR"] == str(tmp_path / "gio-modules")
        assert os.environ["GIO_EXTRA_MODULES"] == ""
        assert os.environ["QT_QUICK_BACKEND"] == "software"
        assert os.environ["QT_WIDGETS_RHI"] == "0"
        assert os.environ["QT_XCB_GL_INTEGRATION"] == "none"
        assert "--disable-gpu" in os.environ["QTWEBENGINE_CHROMIUM_FLAGS"]


def test_qt_webengine_is_prepared_before_qapplication(monkeypatch):
    calls = []

    class FakeQApplication:
        @staticmethod
        def instance():
            calls.append("instance")
            return None

        @staticmethod
        def setAttribute(attribute):
            calls.append(("attribute", attribute))

    fake_qt = types.SimpleNamespace(AA_ShareOpenGLContexts="share-opengl")
    monkeypatch.setitem(sys.modules, "PySide6", types.ModuleType("PySide6"))
    monkeypatch.setitem(
        sys.modules,
        "PySide6.QtCore",
        types.SimpleNamespace(Qt=fake_qt),
    )
    monkeypatch.setitem(
        sys.modules,
        "PySide6.QtWidgets",
        types.SimpleNamespace(QApplication=FakeQApplication),
    )
    monkeypatch.setitem(
        sys.modules,
        "PySide6.QtWebEngineWidgets",
        types.ModuleType("PySide6.QtWebEngineWidgets"),
    )

    launcher.configure_qt_webengine()

    assert calls == ["instance", ("attribute", "share-opengl")]


def test_qt_webview_is_initialized(monkeypatch):
    calls = []

    class FakeQtWebView:
        @staticmethod
        def initialize():
            calls.append("initialize")

    monkeypatch.setitem(sys.modules, "PySide6", types.ModuleType("PySide6"))
    monkeypatch.setitem(
        sys.modules,
        "PySide6.QtWebView",
        types.SimpleNamespace(QtWebView=FakeQtWebView),
    )

    launcher.configure_qt_webview()

    assert calls == ["initialize"]


def test_windows_build_does_not_use_pyinstaller_bootloader_splash():
    spec = Path("marlim3_desktop/packaging/marlim3-desktop.spec").read_text(
        encoding="utf-8"
    )

    assert "Splash(" not in spec
    assert "PySide6.QtWebView" in spec
    assert "libqtwebview_darwin.dylib" in spec


def test_streamlit_flags_are_headless_and_localhost():
    flags = launcher.streamlit_flag_options(8765)

    assert flags["server.address"] == "127.0.0.1"
    assert flags["server.port"] == 8765
    assert flags["server.headless"] is True
    assert flags["server.fileWatcherType"] == "none"


def test_validation_environment_uses_xcb_with_linux_display(monkeypatch):
    monkeypatch.setattr(build.platform, "system", lambda: "Linux")
    monkeypatch.setenv("DISPLAY", ":99")
    monkeypatch.delenv("QT_QPA_PLATFORM", raising=False)

    env = build.validation_environment()

    assert env["QT_QPA_PLATFORM"] == "xcb"
    assert env["QTWEBENGINE_DISABLE_SANDBOX"] == "1"


def test_validation_environment_uses_offscreen_without_display(monkeypatch):
    monkeypatch.setattr(build.platform, "system", lambda: "Linux")
    monkeypatch.delenv("DISPLAY", raising=False)
    monkeypatch.delenv("QT_QPA_PLATFORM", raising=False)

    env = build.validation_environment()

    assert env["QT_QPA_PLATFORM"] == "offscreen"


def test_validation_environment_does_not_force_offscreen_on_macos(monkeypatch):
    monkeypatch.setattr(build.platform, "system", lambda: "Darwin")
    monkeypatch.delenv("DISPLAY", raising=False)
    monkeypatch.setenv("QT_QPA_PLATFORM", "offscreen")

    env = build.validation_environment()

    assert "QT_QPA_PLATFORM" not in env


def test_server_command_uses_desktop_module_on_posix(monkeypatch):
    monkeypatch.setattr(launcher.sys, "frozen", False, raising=False)
    monkeypatch.setattr(launcher.sys, "platform", "linux")
    monkeypatch.setattr(launcher.os, "getpid", lambda: 4567)

    assert launcher.server_command("127.0.0.1", 9012) == [
        sys.executable,
        "-m",
        "marlim3_desktop",
        "--serve",
        "--host",
        "127.0.0.1",
        "--port",
        "9012",
        "--parent-pid",
        "4567",
    ]


def test_server_command_does_not_monitor_parent_on_windows(monkeypatch):
    monkeypatch.setattr(launcher.sys, "frozen", False, raising=False)
    monkeypatch.setattr(launcher.sys, "platform", "win32")

    command = launcher.server_command("127.0.0.1", 9012)

    assert "--parent-pid" not in command
    assert command[:3] == [sys.executable, "-m", "marlim3_desktop"]


def test_wait_for_server_checks_health_endpoint(monkeypatch):
    response = Mock()
    response.status = 200
    response.read.return_value = b"ok"
    response.__enter__ = Mock(return_value=response)
    response.__exit__ = Mock(return_value=False)
    urlopen = Mock(return_value=response)
    process = Mock()
    process.poll.return_value = None
    monkeypatch.setattr(launcher.urllib.request, "urlopen", urlopen)

    launcher.wait_for_server("http://127.0.0.1:9012", process, timeout=0.1)

    urlopen.assert_called_once_with(
        "http://127.0.0.1:9012/_stcore/health",
        timeout=1.0,
    )


def test_wait_for_server_preserves_premature_exit(monkeypatch):
    process = Mock()
    process.poll.return_value = 7

    try:
        launcher.wait_for_server("http://127.0.0.1:9012", process, timeout=0.1)
    except launcher.ServerExitedError as exc:
        assert "exit code 7" in str(exc)
    else:
        raise AssertionError("Expected a premature Streamlit exit")


def test_windows_hidden_process_flags(monkeypatch):
    monkeypatch.setattr(processes.sys, "platform", "win32")

    flags = processes.hidden_process_flags(new_process_group=True)

    assert flags & processes.CREATE_NO_WINDOW
    assert flags & processes.CREATE_NEW_PROCESS_GROUP


def test_windows_parent_monitor_never_uses_os_kill(monkeypatch):
    monkeypatch.setattr(processes.sys, "platform", "win32")
    kill = Mock(side_effect=AssertionError("os.kill must not be used on Windows"))
    monkeypatch.setattr(processes.os, "kill", kill)

    processes.monitor_posix_parent(1234)

    kill.assert_not_called()


def test_checkpoint_report_is_structured(tmp_path):
    report = tmp_path / "checkpoints.jsonl"
    recorder = checkpoints.CheckpointRecorder(report)

    recorder.record("launcher_started")
    recorder.record("runtime_configured")
    events = [
        json.loads(line)
        for line in report.read_text(encoding="utf-8").splitlines()
    ]

    assert [event["checkpoint"] for event in events] == [
        "launcher_started",
        "runtime_configured",
    ]
    assert events[0]["progress"] == 5


def test_log_directories_are_platform_specific(monkeypatch, tmp_path):
    monkeypatch.setattr(logs.sys, "platform", "win32")
    monkeypatch.setenv("LOCALAPPDATA", str(tmp_path))
    assert logs.get_log_directory() == tmp_path / "Marlim3" / "logs"

    monkeypatch.setattr(logs.sys, "platform", "darwin")
    monkeypatch.setattr(logs.Path, "home", lambda: tmp_path)
    assert logs.get_log_directory() == tmp_path / "Library" / "Logs" / "Marlim3"

    monkeypatch.setattr(logs.sys, "platform", "linux")
    monkeypatch.setenv("XDG_STATE_HOME", str(tmp_path / "state"))
    assert logs.get_log_directory() == tmp_path / "state" / "marlim3" / "logs"


def test_runtime_config_points_to_shared_logo(monkeypatch, tmp_path):
    monkeypatch.setenv("MARLIM3_DESKTOP_ROOT", str(tmp_path))

    config = launcher.build_runtime_config(port=9012)

    assert config["url"] == "http://127.0.0.1:9012"
    assert Path(config["logo"]) == tmp_path / "assets" / "branding" / "logo.svg"
    assert Path(config["app_icon"]).parts[-4:] == (
        "marlim3_desktop",
        "assets",
        "icons",
        "app-icon.png",
    )
