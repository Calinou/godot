#[vertex]

#version 450

#VERSION_DEFINES

vec2 positions[3] = vec2[](
		vec2(-1.0, -1.0),
		vec2(3.0, -1.0),
		vec2(-1.0, 3.0));

vec2 texture_coordinates[3] = vec2[](
		vec2(0.0, 0.0),
		vec2(2.0, 0.0),
		vec2(0.0, 2.0));

layout(location = 0) out vec2 texture_coord;
layout(location = 1) out vec4[3] offsets;

#include "smaa_settings.inc.glsl"
#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0
#include "smaa.inc.glsl"

void main() {
	texture_coord = texture_coordinates[gl_VertexIndex];
	SMAAEdgeDetectionVS(texture_coord, offsets);
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}

#[fragment]

#version 450

#VERSION_DEFINES

layout(set = 0, binding = 0) uniform sampler2D color_img;

layout(location = 0) out vec4 frag_color;
layout(location = 0) in vec2 texture_coord;
layout(location = 1) in vec4[3] offsets;

#include "smaa_settings.inc.glsl"
#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1
#include "smaa.inc.glsl"

void main() {
	// Use luma version (compromise between performance and quality).
	fragColor = vec4(SMAALumaEdgeDetectionPS(texture_coord, offsets, color_img), 0.0, 0.0);
}
