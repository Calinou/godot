@tool
extends WorldEnvironment

# Place clustered elements within a horizontal plane of this size.
const PLANE_SIZE = 128

# The total number of clustered elements
# (OmniLights + SpotLights + Decals + ReflectionProbes) to be rendered.
# Increasing this value above 200 can cause crashes due to
# <https://github.com/godotengine/godot/issues/56657>.
const NUMBER_OF_ELEMENTS = 200


func _ready() -> void:
	# Set a fixed random seed to ensure determinism across runs.
	seed(0x60d07)

	@warning_ignore("integer_division")
	for __ in roundi(NUMBER_OF_ELEMENTS * 0.25):
		var omni_light := OmniLight3D.new()
		omni_light.light_color = Color.from_hsv(randf_range(0.0, 0.3333), 1.0, 1.0)
		omni_light.light_energy = 3.0
		omni_light.position = Vector3(randf_range(-PLANE_SIZE * 0.5, PLANE_SIZE * 0.5), 2.5, randf_range(-PLANE_SIZE * 0.5, PLANE_SIZE * 0.5))
		add_child(omni_light)

	@warning_ignore("integer_division")
	for __ in roundi(NUMBER_OF_ELEMENTS * 0.25):
		var spot_light := SpotLight3D.new()
		spot_light.light_color = Color.from_hsv(randf_range(0.5, 0.83333), 1.0, 1.0)
		spot_light.light_energy = 3.0
		spot_light.position = Vector3(randf_range(-PLANE_SIZE * 0.5, PLANE_SIZE * 0.5), 2.5, randf_range(-PLANE_SIZE * 0.5, PLANE_SIZE * 0.5))
		spot_light.rotation = Vector3(-1, 0, 0)
		add_child(spot_light)

	@warning_ignore("integer_division")
	for __ in roundi(NUMBER_OF_ELEMENTS * 0.25):
		var decal := Decal.new()
		decal.texture_albedo = preload("res://icon.png")
		decal.extents *= 2.0
		decal.modulate = Color(randf_range(-0.5, 4), randf_range(-0.5, 4), randf_range(-0.5, 4))
		decal.position = Vector3(randf_range(-PLANE_SIZE * 0.5, PLANE_SIZE * 0.5), 0.0, randf_range(-PLANE_SIZE * 0.5, PLANE_SIZE * 0.5))
		decal.rotation = Vector3(randf_range(0, 3), randf(), randf())
		add_child(decal)

	@warning_ignore("integer_division")
	for __ in roundi(NUMBER_OF_ELEMENTS * 0.25):
		var reflection_probe := ReflectionProbe.new()
		reflection_probe.extents *= 0.5
		reflection_probe.ambient_mode = ReflectionProbe.AMBIENT_COLOR
		reflection_probe.ambient_color = Color.from_hsv(0.0, 0.0, randf_range(-2, 3))
		reflection_probe.position = Vector3(randf_range(-PLANE_SIZE * 0.5, PLANE_SIZE * 0.5), 0.0, randf_range(-PLANE_SIZE * 0.5, PLANE_SIZE * 0.5))
		add_child(reflection_probe)
