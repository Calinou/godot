# Godot release process

This file documents the release process applied to every Godot release.

## Build environment

The [Godot build containers](https://github.com/godotengine/build-containers)
are used to produce official releases.

To ensure binaries run out of the box without security warnings, official
releases are codesigned for Windows using an extended validation certificate
owned by Prehensile Tales. For macOS, signing and notarization is used as
required by macOS 10.15 and later.

## Release steps

**Note:** "Pre-release" refers to any release that is tagged *alpha*, *beta* or *release candidate* (RC).
