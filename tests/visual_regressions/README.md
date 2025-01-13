# godot-rendering-tests

This is a Godot project that stores and runs a collection of 2D and 3D rendering tests.
It is used to check for regressions in Godot's rendering backends, and can also be
used to test performance on various GPUs.

**Interested in adding new test cases?** See [CONTRIBUTING.md](CONTRIBUTING.md).

## Comparing results

1. Run the project once to have images be saved to `results/`.
2. Copy the `results/` folder to the same folder, but with a different suffix
   (e.g. `results_master/`).
3. Install [dssim](https://github.com/kornelski/dssim), which is a tool compares
   images and returns a similarity score (lower is more similar).
4. Run the following command in a terminal:

   ```bash
   (for img in results/*/*.png; do dssim $img ${img//results/results_master}; done) | sort
   ```

   Images that differ the most from the images in `results_master/` are displayed at the bottom of the list.
