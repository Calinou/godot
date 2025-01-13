extends Node

# Arrays of strings with scene file names (no extension).
var test_scenes_3d := []
var test_scenes_2d := []


func _ready() -> void:
	if not "--save-results" in OS.get_cmdline_user_args():
		# Run multiple Godot instances in parallel and save results for all possible rendering driver/method
		# combinations (depending on operating system).
		var rendering_drivers := PackedStringArray(["vulkan", "opengl3"])
		if OS.has_feature("windows"):
			rendering_drivers.push_back("d3d12")
		elif OS.has_feature("macos") or OS.has_feature("ios"):
			rendering_drivers.push_back("metal")

		var instances_started := 0
		for rendering_driver in rendering_drivers:
			if rendering_driver == "opengl3":
				instances_started += 1
				OS.create_instance(["--rendering-driver", "opengl3", "--", "--save-results"])
			else:
				for rendering_method in ["forward_plus", "mobile"]:
					instances_started += 1
					OS.create_instance(["--rendering-driver", rendering_driver, "--rendering-method", rendering_method, "--", "--save-results"])

		print("Exiting (%d other Godot instances will keep running in the background until all results are saved)." % instances_started)
		get_tree().quit()
	else:
		var results_folder := "res://results/%s_%s" % [RenderingServer.get_current_rendering_driver_name(), RenderingServer.get_current_rendering_method().replace("gl_compatibility", "compatibility")]
		# Code run by each instance that was started above.
		DirAccess.make_dir_recursive_absolute(results_folder.path_join("2d"))
		DirAccess.make_dir_recursive_absolute(results_folder.path_join("3d"))
		# Prevent Godot from importing the generated screenshots and Git from tracking them in version control.
		FileAccess.open("res://results/.gdignore", FileAccess.WRITE)
		var gitignore := FileAccess.open("res://results/.gitignore", FileAccess.WRITE)
		gitignore.store_string("*\n")

		# Run 2D scenes.
		var dir := DirAccess.open("res://tests/2d")
		if dir:
			dir.list_dir_begin()
			var file_name := dir.get_next()
			while file_name != "":
				if not dir.current_is_dir() and file_name.ends_with(".tscn"):
					test_scenes_2d.push_back(file_name.trim_suffix(".tscn"))

				file_name = dir.get_next()

			# Results may not be sorted alphabetically, so ensure this to improve determinism.
			test_scenes_2d.sort()
			print("Found %d 2D test scenes in the `tests/2d` folder." % test_scenes_2d.size())
		else:
			push_error("An error occurred when trying to access res://tests/2d.")

		for test_scene in test_scenes_2d:
			print("Loading 2D test scene: %s" % test_scene)
			var test: Node = load("res://tests/2d/%s.tscn" % test_scene).instantiate()
			add_child(test)

			await take_screenshot(results_folder.path_join("/2d/%s.png" % test_scene), test.get_meta("wait_frames", 0))

			test.queue_free()

		# Run 3D scenes.
		dir = DirAccess.open("res://tests/3d")
		if dir:
			dir.list_dir_begin()
			var file_name := dir.get_next()
			while file_name != "":
				if not dir.current_is_dir() and file_name.ends_with(".tscn"):
					test_scenes_3d.push_back(file_name.trim_suffix(".tscn"))

				file_name = dir.get_next()

			# Results may not be sorted alphabetically, so ensure this to improve determinism.
			test_scenes_3d.sort()
			print("Found %d 3D test scenes in the `tests/3d` folder." % test_scenes_3d.size())
		else:
			push_error("An error occurred when trying to access res://tests/3d.")

		for test_scene in test_scenes_3d:
			print("Loading 3D test scene: %s" % test_scene)
			var test: Node = load("res://tests/3d/%s.tscn" % test_scene).instantiate()
			add_child(test)

			await take_screenshot(results_folder.path_join("3d/%s.png" % test_scene), test.get_meta("wait_frames", 0))

			test.queue_free()

		# Done saving all images. Nothing more to do.
		get_tree().quit()


func take_screenshot(output_path: String, wait_frames: int) -> void:
	# Wait some frames to get an up-to-date screenshot.
	# This is required for TAA, volumetric fog and SDFGI to have the time to converge.
	for __ in wait_frames + 30:
		await get_tree().process_frame

	var viewport := get_viewport()
	var image: Image = viewport.get_texture().get_image()
	print("Saving screenshot: %s" % output_path)
	image.save_png(output_path)
