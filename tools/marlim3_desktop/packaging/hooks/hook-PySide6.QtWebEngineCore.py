"""Collect the Qt WebEngine helper process and runtime resources."""

from PyInstaller.utils.hooks.qt import add_qt6_dependencies, pyside6_library_info


hiddenimports, binaries, datas = add_qt6_dependencies(__file__)
webengine_binaries, webengine_datas = pyside6_library_info.collect_qtwebengine_files()
binaries += webengine_binaries
datas += webengine_datas
hiddenimports += ["PySide6.QtPrintSupport"]
