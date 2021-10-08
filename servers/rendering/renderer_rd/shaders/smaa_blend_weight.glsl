#[vertex]

#version 450

#VERSION_DEFINES

#include "smaa_blend_weight_inc.glsl"

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec2 pixcoord;
layout(location = 2) out vec4 offset[3];

#define SMAA_INCLUDE_PS 0
#include "thirdparty/smaa/SMAA.hlsl"

void main() {
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord;
	SMAABlendingWeightCalculationVS(TexCoord, pixcoord, offset);
}

#[fragment]

#version 450

#VERSION_DEFINES

#include "smaa_blend_weight_inc.glsl"

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec2 pixcoord;
layout(location = 2) in vec4 offset[3];
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;
layout(set = 0, binding = 3) uniform sampler2D areaTex;
layout(set = 0, binding = 4) uniform sampler2D searchTex;

#define SMAA_INCLUDE_VS 0
#include "thirdparty/smaa/SMAA.hlsl"

void main() {
	vec4 subsampleIndices = vec4(0.0);
	FragColor = SMAABlendingWeightCalculationPS(vTexCoord, pixcoord, offset, Source, areaTex, searchTex, subsampleIndices);
}
