// SMAA Neighborhood Blending Shader (Third Pass)

layout(push_constant) uniform Push {
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
}
params;

layout(std140, set = 0, binding = 0) uniform UBO {
	mat4 MVP;
}
global;

#define SMAA_RT_METRICS vec4(params.SourceSize.z, params.SourceSize.w, params.SourceSize.x, params.SourceSize.y)
#define SMAA_GLSL_4
