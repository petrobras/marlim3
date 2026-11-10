"""Qt splash screen and startup error dialogs."""

from __future__ import annotations

import os
import sys
from pathlib import Path


def update_bootloader_splash(text: str) -> None:
    if not sys.platform.startswith("win") or "_PYI_SPLASH_IPC" not in os.environ:
        return
    try:
        import pyi_splash

        if pyi_splash.is_alive():
            pyi_splash.update_text(text)
    except (ImportError, RuntimeError):
        pass


def close_bootloader_splash() -> None:
    if not sys.platform.startswith("win") or "_PYI_SPLASH_IPC" not in os.environ:
        return
    try:
        import pyi_splash

        if pyi_splash.is_alive():
            pyi_splash.close()
    except (ImportError, RuntimeError):
        pass


def create_splash(app, logo_path: Path, icon_path: Path):
    from PySide6.QtCore import QTimer, Qt
    from PySide6.QtGui import QColor, QIcon, QPainter, QPixmap
    from PySide6.QtWidgets import (
        QFrame,
        QHBoxLayout,
        QLabel,
        QProgressBar,
        QVBoxLayout,
        QWidget,
    )

    class DesktopSplash(QWidget):
        def __init__(self) -> None:
            super().__init__()
            self.setWindowFlags(
                Qt.FramelessWindowHint
                | Qt.WindowStaysOnTopHint
                | Qt.Tool
            )
            self.setAttribute(Qt.WA_TranslucentBackground)
            self.setFixedSize(620, 290)
            if icon_path.exists():
                self.setWindowIcon(QIcon(str(icon_path)))

            outer = QVBoxLayout(self)
            outer.setContentsMargins(12, 12, 12, 12)
            panel = QFrame()
            panel.setObjectName("panel")
            panel.setStyleSheet(
                """
                QFrame#panel {
                    background: #0c1424;
                    border: 1px solid #263a59;
                    border-radius: 24px;
                }
                QLabel { color: #f4f7fb; }
                QProgressBar {
                    background: #273451;
                    border: 0;
                    border-radius: 7px;
                    height: 14px;
                    text-align: center;
                    color: transparent;
                }
                QProgressBar::chunk {
                    border-radius: 7px;
                    background: qlineargradient(
                        x1:0, y1:0, x2:1, y2:0,
                        stop:0 #6558e8, stop:0.55 #735fe8, stop:1 #39c0e0
                    );
                }
                """
            )
            outer.addWidget(panel)

            layout = QVBoxLayout(panel)
            layout.setContentsMargins(30, 28, 30, 28)
            layout.setSpacing(18)

            header = QHBoxLayout()
            header.setSpacing(22)
            visual_path = icon_path if icon_path.exists() else logo_path
            if visual_path.exists():
                logo = QLabel()
                pixmap = QPixmap(str(visual_path))
                logo.setPixmap(
                    pixmap.scaled(
                        92,
                        92,
                        Qt.KeepAspectRatio,
                        Qt.SmoothTransformation,
                    )
                )
                logo.setFixedSize(100, 92)
                logo.setAlignment(Qt.AlignCenter)
                header.addWidget(logo)

            titles = QVBoxLayout()
            title = QLabel("Marlim3")
            title.setStyleSheet("font-size: 27px; font-weight: 700;")
            subtitle = QLabel("Starting the transient multiphase flow simulator")
            subtitle.setStyleSheet("font-size: 15px; color: #c8d4e5;")
            titles.addWidget(title)
            titles.addWidget(subtitle)
            titles.addStretch()
            header.addLayout(titles, 1)
            layout.addLayout(header)

            self.status = QLabel("Loading...")
            self.status.setStyleSheet("font-size: 15px; font-weight: 600;")
            layout.addWidget(self.status)
            self.progress = QProgressBar()
            self.progress.setRange(0, 100)
            self.progress.setValue(0)
            layout.addWidget(self.progress)
            self.target_progress = 0
            self.progress_timer = QTimer(self)
            self.progress_timer.setInterval(35)
            self.progress_timer.timeout.connect(self._advance_progress)
            self.progress_timer.start()

        def _advance_progress(self) -> None:
            current = self.progress.value()
            if current < self.target_progress:
                self.progress.setValue(current + 1)

        def paintEvent(self, event) -> None:
            painter = QPainter(self)
            painter.setRenderHint(QPainter.Antialiasing)
            painter.setBrush(QColor(0, 0, 0, 48))
            painter.setPen(Qt.NoPen)
            painter.drawRoundedRect(self.rect().adjusted(6, 8, -2, -2), 26, 26)
            super().paintEvent(event)

        def set_progress(self, checkpoint: str, value: int) -> None:
            labels = {
                "runtime_configured": "Preparing the application...",
                "server_process_started": "Starting the local interface...",
                "health_endpoint_ready": "Loading the interface...",
                "webengine_created": "Preparing the desktop window...",
                "webengine_load_finished": "Finishing startup...",
                "main_window_visible": "Ready",
            }
            self.status.setText(labels.get(checkpoint, "Loading..."))
            self.target_progress = value
            if value == 100:
                self.progress.setValue(100)
            app.processEvents()

        def center(self) -> None:
            screen = app.screenAt(self.cursor().pos()) or app.primaryScreen()
            geometry = screen.availableGeometry()
            self.move(geometry.center() - self.rect().center())

    splash = DesktopSplash()
    splash.center()
    splash.show()
    app.processEvents()
    close_bootloader_splash()
    return splash


def show_startup_error(log_path: Path, message: str) -> None:
    from PySide6.QtCore import QUrl
    from PySide6.QtGui import QDesktopServices
    from PySide6.QtWidgets import QApplication, QMessageBox

    app = QApplication.instance() or QApplication([])
    dialog = QMessageBox()
    dialog.setIcon(QMessageBox.Critical)
    dialog.setWindowTitle("Marlim3")
    dialog.setText("Marlim3 could not be started.")
    dialog.setInformativeText("A diagnostic log was created.")
    dialog.setDetailedText(message)
    open_button = dialog.addButton("Open log folder", QMessageBox.ActionRole)
    dialog.addButton(QMessageBox.Close)
    dialog.exec()
    if dialog.clickedButton() is open_button:
        QDesktopServices.openUrl(QUrl.fromLocalFile(str(log_path.parent)))
    app.processEvents()
