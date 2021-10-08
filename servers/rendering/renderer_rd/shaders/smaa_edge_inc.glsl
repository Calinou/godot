// SMAA Edge Detection Shaders (First Pass)

layout(push_constant) uniform Push {
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
	float SMAA_EDT;
	float SMAA_THRESHOLD;
	float SMAA_MAX_SEARCH_STEPS;
	float SMAA_MAX_SEARCH_STEPS_DIAG;
	float SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR;
}
params;

layout(std140, set = 0, binding = 0) uniform UBO {
	mat4 MVP;
}
global;

#define SMAA_RT_METRICS vec4(params.SourceSize.z, params.SourceSize.w, params.SourceSize.x, params.SourceSize.y)
#define SMAA_GLSL_4

float THRESHOLD = 0.05; // #pragma parameter SMAA_THRESHOLD "SMAA Threshold" 0.05 0.01 0.5 0.01
float MAX_SEARCH_STEPS = 32.0; // #pragma parameter SMAA_MAX_SEARCH_STEPS "SMAA Max Search Steps" 32.0 4.0 112.0 1.0
float MAX_SEARCH_STEPS_DIAG = 16.0; // #pragma parameter SMAA_MAX_SEARCH_STEPS_DIAG "SMAA Max Search Steps Diagonal" 16.0 4.0 20.0 1.0
float LOCAL_CONTRAST_ADAPTATION_FACTOR = 2.0; // #pragma parameter SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR "SMAA Local Contrast Adapt. Factor" 2.0 1.0 4.0 0.1

#define SMAA_THRESHOLD THRESHOLD
#define SMAA_MAX_SEARCH_STEPS MAX_SEARCH_STEPS
#define SMAA_MAX_SEARCH_STEPS_DIAG MAX_SEARCH_STEPS_DIAG
#define SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR LOCAL_CONTRAST_ADAPTATION_FACTOR
