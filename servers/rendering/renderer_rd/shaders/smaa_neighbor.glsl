#[vertex]

#version 450

#VERSION_DEFINES

#include "smaa_neighbor_inc.glsl"

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec4 offset;

#define SMAA_INCLUDE_PS 0
#include "thirdparty/smaa/SMAA.hlsl"

void main() {
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord;
	SMAANeighborhoodBlendingVS(TexCoord, offset);
}

#[fragment]

#version 450

#VERSION_DEFINES

#include "smaa_neighbor_inc.glsl"

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec4 offset;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;
layout(set = 0, binding = 3) uniform sampler2D SMAA_Input;

#define SMAA_INCLUDE_VS 0
#include "thirdparty/smaa/SMAA.hlsl"

void main() {
	FragColor = SMAANeighborhoodBlendingPS(vTexCoord, offset, SMAA_Input, Source);
}
