# Marlim3 Desktop

Marlim3 Desktop provides the Marlim3 Streamlit interface as a standalone
desktop application. Download the file for your operating system and run it
without installing Python, Streamlit, Qt, or a compiler.

## Supported platforms

| Platform | Download |
|----------|----------|
| Linux x64 | `Marlim3-desktop-linux-x64` |
| Windows x64 | `Marlim3-desktop-windows-x64.exe` |
| macOS Apple Silicon | `Marlim3-desktop-macos-arm64.dmg` |

The Linux application requires glibc 2.34 or newer. Ubuntu 22.04 or newer and
Debian 12 or newer satisfy this requirement.

## Linux

Make the downloaded file executable, then open it:

```bash
chmod +x Marlim3-desktop-linux-x64
./Marlim3-desktop-linux-x64
```

## Windows

Double-click `Marlim3-desktop-windows-x64.exe`.

Windows may display a security warning because early Marlim3 Desktop releases
are not code-signed. Only run files downloaded from the official Marlim3
release page.

## macOS

Open `Marlim3-desktop-macos-arm64.dmg` and drag `Marlim3.app` into the
Applications folder.

Unsigned releases may be blocked on first launch. If this happens, try to open
Marlim3 once, then use **System Settings → Privacy & Security → Open Anyway**.

## Starting the application

The application is distributed as a self-contained file. Startup can take
several seconds while its bundled runtime is prepared. A loading window
remains visible until the Marlim3 interface is ready.

Closing the main window also stops the local Marlim3 interface and releases its
resources.

## Verifying a download

Each release includes a matching SHA-256 file. On Linux:

```bash
sha256sum -c Marlim3-desktop-linux-x64.sha256
```

Use the equivalent SHA-256 verification tool for Windows or macOS.

## Diagnostic logs

If the application cannot start, it offers to open its diagnostic log folder.
Logs are stored in:

- Windows: `%LOCALAPPDATA%\Marlim3\logs`
- macOS: `~/Library/Logs/Marlim3`
- Linux: `~/.local/state/marlim3/logs`

Simulation files and results remain on the local computer.
