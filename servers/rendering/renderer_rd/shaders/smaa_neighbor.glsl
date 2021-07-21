#[vertex]

#version 450

#VERSION_DEFINES

layout(location = 0) out vec2 texture_coord;
layout(location = 1) out vec4 offset;

#include "smaa_settings.inc.glsl"
#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0
#include "smaa.inc.glsl"

void main() {
	gl_Position = vec4(gl_VertexIndex == 1 ? 3.0 : -1.0,
			gl_VertexIndex == 2 ? 3.0 : -1.0,
			0.0,
			1.0);

	texture_coord = vec2(gl_VertexIndex == 1 ? 2.0 : 0.0,
			gl_VertexIndex == 2 ? 2.0 : 0.0);

	SMAANeighborhoodBlendingVS(texture_coord, offset);
}

#[fragment]

#version 450

#VERSION_DEFINES

layout(set = 0, binding = 0) uniform sampler2D color_img;
layout(set = 0, binding = 4) uniform sampler2D blend_tex;

layout(location = 0) out vec4 frag_color;
layout(location = 0) in vec2 texture_coord;
layout(location = 1) in vec4 offset;

#include "smaa_settings.inc.glsl"
#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1
#include "smaa.inc.glsl"

void main() {
	frag_color = SMAANeighborhoodBlendingPS(texture_coord, offset, color_img, blend_tex);
}
