layout(push_constant, binding = 1, std430) uniform Params {
	ivec2 size;
	float z_far;
	float z_near;

	bool orthogonal;
	float blur_size;
	float blur_scale;
	int blur_steps;

	bool blur_near_active;
	float blur_near_begin;
	float blur_near_end;
	bool blur_far_active;

	float blur_far_begin;
	float blur_far_end;
	bool second_pass;
	bool half_size;

	bool use_jitter;
	float jitter_seed;
	uint pad[2];
}
params;

//used to work around downsampling filter
#define DEPTH_GAP 0.0

const float GOLDEN_ANGLE = 2.39996323;

//note: uniform pdf rand [0;1[
float hash12n(vec2 p) {
	p = fract(p * vec2(5.3987, 5.4421));
	p += dot(p.yx, p.xy + vec2(21.5351, 14.3137));
	return fract(p.x * p.y * 95.4307);
}

float remap(float value, vec2 from, vec2 to) {
	return to.x + (value - from.x) * (to.y - to.x) / (from.y - from.x);
}

float linearstep(float lo, float hi, float x) {
	return (clamp(x, lo, hi) - lo) / (hi - lo);
}

// apply some denoising when upscaling
// adaptation from "GLSL Fast Image Denoiser" by Sir Bird / Zerofile (https://www.shadertoy.com/view/7d2SDD)

#define DENOISE

#if defined(MODE_COMPOSITE_BOKEH) && defined(DENOISE)

#define SAMPLES 5  // higher values seem to result in artifacts and too much blurring
#define DISTRIBUTION_BIAS 0.5 // between 0. and 1.
#define PIXEL_MULTIPLIER  2.0 // between 1. and 3. (keep low)
#define INVERSE_HUE_TOLERANCE 20.0 // (2. - 30.)

#define fastPow(a,b) pow(max(a, 0.0), b) // @morimea

const mat2 sample2D = mat2(cos(GOLDEN_ANGLE), sin(GOLDEN_ANGLE), -sin(GOLDEN_ANGLE), cos(GOLDEN_ANGLE));

vec4 sirBirdDenoise(sampler2D imageTexture, in vec2 uv, in vec2 samplePixel) {
    const float invSamples       = 1.0 / float(SAMPLES);
    const float sampleRadius     = sqrt(float(SAMPLES));
    const float sampleTrueRadius = 0.5 / (sampleRadius * sampleRadius);
    vec4        sampleCenter     = texture(imageTexture, uv);
    vec3        sampleCenterNorm = normalize(sampleCenter.rgb);
    float       sampleCenterSat  = length(sampleCenter.rgb);

    float  influenceSum = 0.0;
    float brightnessSum = 0.0;

    vec2 pixelRotated = vec2(0.0, 1.0);
    vec4 denoisedColor = vec4(0.0, 0.0, 0.0, sampleCenter.a * invSamples);

    for (float x = 0.0; x < float(SAMPLES); x++) {
        pixelRotated *= sample2D;

        vec2  pixelOffset    = PIXEL_MULTIPLIER * sqrt(x) * pixelRotated * 0.5;
        float pixelInfluence = 1.0 - sampleTrueRadius * fastPow(dot(pixelOffset, pixelOffset), DISTRIBUTION_BIAS);
        pixelOffset *= samplePixel;

        vec4 thisDenoisedColor = texture(imageTexture, uv + pixelOffset);

        pixelInfluence *= pixelInfluence * pixelInfluence;
        /*
            HUE + SATURATION FILTER
        */
        pixelInfluence *=
            fastPow(0.5 + 0.5 * dot(sampleCenterNorm, normalize(thisDenoisedColor.rgb)), INVERSE_HUE_TOLERANCE)
            * fastPow(1.0 - abs(length(thisDenoisedColor.rgb) - sampleCenterSat), 8.0);

        influenceSum += pixelInfluence;
        denoisedColor.rgb += thisDenoisedColor.rgb * pixelInfluence;
        denoisedColor.a += invSamples * thisDenoisedColor.a;
    }

	denoisedColor.rgb /= influenceSum;
    return denoisedColor;
}

#endif
