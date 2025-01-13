# Contributing to godot-rendering-tests

Thank you for your interest in contributing!

**Note:** This project only supports Godot's `master` branch (4.0's development
branch), not Godot 3.x. Attempting to open this project in Godot 3.x will result
in errors.

## Test scene guidelines

To make integration tests more effective, the guidelines below must be followed:

- Test scenes should be as minimal as possible. Do not include unnecessary functionality
  in test scenes.
  - For 3D tests, it is recommended to have a WorldEnvironment node as a root
    node with an empty Environment resource attached to it. To avoid visual
    discrepancy between the editor and running project, add a DirectionalLight3D
    node then hide it (unless you need the DirectionalLight3D to be visible in
    your test).
  - Only add scripts if it is absolutely required. Otherwise, try to do everything
    in the editor so that rendering tests do not rely on GDScript functionality
    to work.
- Create several test cases for different usage scenarios to ensure that failures can be tracked down individually.

Remember to follow the
[GDScript style guide](https://docs.godotengine.org/en/latest/tutorials/scripting/gdscript/gdscript_styleguide.html)
when writing new scripts. Adding type hints is recommended whenever possible.

## Adding new test cases

You can add new test cases by following these steps:

### Add a new scene

- Open the Godot editor and import this project.
- Create a new scene with a `snake_case` file name and `PascalCase` root node
  name.[^1] For example, if you have a test case for StandardMaterial3D refraction,
  name the file `test_standard_material_3d_refraction.tscn` and the root node
  `TestStandardMaterial3DRefraction`. Save the scene file in the `tests/3d/`
  folder if it's a 3D test case, or `tests/2d/` if it's a 2D test case.
  category.

For 3D test cases, the root node type should be WorldEnvironment. For 2D
test cases, the root node should be Node2D. For UI test cases, the
root node should be Control.

### Test the test case

- *TODO: Not fully implemented yet.*
