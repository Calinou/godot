# Godot Windows installer build files

`godot.nsi` is an [NSIS](https://nsis.sourceforge.io/) installer file that can
be used to build a Windows installer. The generated installer is able to run
without Administrator privileges and can optionally add Godot to the user's
`PATH` environment variable.

`godot.nsi` was developed and tested with NSIS 3.06.1. Using older NSIS versions
may not work correctly.

To use NSIS on Linux, install the NSIS command-line compiler from your
distribution's repositories:

- **Debian/Ubuntu:** `sudo apt-get install nsis`
- **Fedora:** `sudo dnf install mingw32-nsis`

## Building

- Place a Godot editor executable in this folder and rename it to `godot.exe`.
- Run `makensis` (or the graphical `makensisw`) on the `godot.nsi` file.

If everything succeeds, a `godot-setup-x86_64.exe` installer will be generated
in this folder.
