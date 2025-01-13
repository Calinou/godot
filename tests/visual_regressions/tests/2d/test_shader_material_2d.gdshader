shader_type canvas_item;

uniform vec3 color : source_color;

void fragment() {
	COLOR.rgb = color;
}
