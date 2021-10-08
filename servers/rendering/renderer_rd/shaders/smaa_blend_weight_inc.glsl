// SMAA Blending Weight Calculation Shader (Second Pass)

layout(push_constant) uniform Push {
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
	float SMAA_THRESHOLD;
	float SMAA_MAX_SEARCH_STEPS;
	float SMAA_MAX_SEARCH_STEPS_DIAG;
	float SMAA_CORNER_ROUNDING;
}
params;

layout(std140, set = 0, binding = 0) uniform UBO {
	mat4 MVP;
}
global;

#define SMAA_RT_METRICS vec4(params.SourceSize.z, params.SourceSize.w, params.SourceSize.x, params.SourceSize.y)
#define SMAA_GLSL_4

float THRESHOLD = params.SMAA_THRESHOLD;
float MAX_SEARCH_STEPS = params.SMAA_MAX_SEARCH_STEPS;
float MAX_SEARCH_STEPS_DIAG = params.SMAA_MAX_SEARCH_STEPS_DIAG;
float CORNER_ROUNDING = params.SMAA_CORNER_ROUNDING;
#define SMAA_THRESHOLD THRESHOLD
#define SMAA_MAX_SEARCH_STEPS MAX_SEARCH_STEPS
#define SMAA_MAX_SEARCH_STEPS_DIAG MAX_SEARCH_STEPS_DIAG
#define SMAA_CORNER_ROUNDING CORNER_ROUNDING
