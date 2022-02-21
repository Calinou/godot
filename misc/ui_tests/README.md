# Editor and project manager automated UI tests

This folder contains automated integration tests for Godot editor and project manager,
powered by [Nut.js](https://nutjs.dev/).

## Prerequisites

The prerequisites for running the tests are
[the same as Nut.js](https://nutjs.dev/docs/tutorial-first_steps/prerequisits).

You will need to install [Node.js](https://nodejs.org/) 14.x or 16.x (latest LTS as of writing).
**Node.js 17 and later are currently not supported by Nut.js!**

Godot's `master` branches requires Vulkan support to run in non-headless mode,
so a GPU with Vulkan support is required. Testing with OpenGL is not supported
yet, but could be exposed in the future.

Since image recognition is used to select the clickable areas on the screen,
those integration tests are independent of your screen resolution, size or count.
Just make sure not to alt-tab out of the Godot window while tests are running.

## Install dependencies

`cd` to this folder, then run the following command:

```bash
npm install
```

## Run tests

Tests expect a Godot editor binary to be located in this repository's top-level
`bin/` folder. Currently, only Linux 64-bit is handled by the script. Support
for other platforms is planned.

`cd` to this folder, then run the following command:

```bash
npm test

# The above line is equivalent to:
node index.js
```

For convenience, this script is designed to be run from any folder.
For example, if your current working directory is the repository root, you can run:

```bash
node misc/ui_tests/index.js
```
