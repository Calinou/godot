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
layout(location = 1) out vec2 pixel_coord;
layout(location = 2) out vec4[3] offsets;

#include "smaa_settings.inc.glsl"
#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0
#include "smaa.inc.glsl"

void main() {
	texture_coord = texture_coordinates[gl_VertexIndex];
	SMAABlendingWeightCalculationVS(texture_coord, pixel_coord, offsets);
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}

#[fragment]

#version 450

#VERSION_DEFINES

layout(set = 0, binding = 1) uniform sampler2D edges_tex;
layout(set = 0, binding = 2) uniform sampler2D area_tex;
layout(set = 0, binding = 3) uniform sampler2D search_tex;

layout(location = 0) out vec4 frag_color;
layout(location = 0) in vec2 texture_coord;
layout(location = 1) in vec2 pixel_coord;
layout(location = 2) in vec4[3] offsets;

#include "smaa_settings.inc.glsl"

#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1
#include "smaa.inc.glsl"

void main() {
	frag_color = SMAABlendingWeightCalculationPS(texture_coord, pixel_coord, offsets, edgesTex, areaTex, searchTex, vec4(0.0));
}
