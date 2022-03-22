#[compute]

#version 450

#VERSION_DEFINES

#define BLOCK_SIZE 8

layout(local_size_x = BLOCK_SIZE, local_size_y = BLOCK_SIZE, local_size_z = 1) in;

#ifdef MODE_GEN_BLUR_SIZE
layout(rgba16f, set = 0, binding = 0) uniform restrict image2D color_image;
layout(set = 1, binding = 0) uniform sampler2D source_depth;
#endif

#if defined(MODE_BOKEH_BOX) || defined(MODE_BOKEH_HEXAGONAL) || defined(MODE_BOKEH_CIRCULAR)
layout(set = 1, binding = 0) uniform sampler2D color_texture;
layout(rgba16f, set = 0, binding = 0) uniform restrict writeonly image2D bokeh_image;
#endif

#ifdef MODE_COMPOSITE_BOKEH
layout(rgba16f, set = 0, binding = 0) uniform restrict image2D color_image;
layout(set = 1, binding = 0) uniform sampler2D source_bokeh;
#endif

// based on https://www.shadertoy.com/view/Xd3GDl

#include "bokeh_dof_inc.glsl"

#ifdef MODE_GEN_BLUR_SIZE

float get_depth_at_pos(vec2 uv) {
	float depth = textureLod(source_depth, uv, 0.0).x;
	if (params.orthogonal) {
		depth = ((depth + (params.z_far + params.z_near) / (params.z_far - params.z_near)) * (params.z_far - params.z_near)) / 2.0;
	} else {
		depth = 2.0 * params.z_near * params.z_far / (params.z_far + params.z_near - depth * (params.z_far - params.z_near));
	}
	return depth;
}

float get_blur_size(float depth) {
	if (params.blur_near_active && depth < params.blur_near_begin) {
		return (linearstep(params.blur_near_end, params.blur_near_begin, depth) - 1.0) * params.blur_size - DEPTH_GAP; //near blur is negative
	}

	if (params.blur_far_active && depth > params.blur_far_begin) {
		return linearstep(params.blur_far_begin, params.blur_far_end, depth) * params.blur_size + DEPTH_GAP;
	}

	return 0.0;
}

#endif

#if defined(MODE_BOKEH_BOX) || defined(MODE_BOKEH_HEXAGONAL)

vec4 weighted_filter_dir(vec2 dir, vec2 uv, vec2 pixel_size) {
	dir *= pixel_size;
	vec4 color = texture(color_texture, uv);

	vec4 accum = color;
	float total = 1.0;

	float blur_scale = params.blur_size / float(params.blur_steps);

	if (params.use_jitter) {
		uv += dir * (hash12n(uv + params.jitter_seed) - 0.5);
	}

	for (int i = -params.blur_steps; i <= params.blur_steps; i++) {
		if (i == 0) {
			continue;
		}
		float radius = float(i) * blur_scale;
		vec2 suv = uv + dir * radius;
		radius = abs(radius);

		vec4 sample_color = texture(color_texture, suv);

		float limit = abs(min(sample_color.a, color.a)) - DEPTH_GAP;

		float m = smoothstep(radius - 0.5, radius + 0.5, limit);

		accum += mix(color, sample_color, m);

		total += 1.0;
	}

	return accum / total;
}

#endif

void main() {
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

	ivec2 checkPos = params.half_size ? 2 * pos : pos;
	if (any(greaterThan(checkPos, params.size))) { //too large, do nothing
		return;
	}

	vec2 pixel_size = 1.0 / vec2(params.size);
// 	if (params.half_size) {
// 		pixel_size *= 0.5;
// 	}

	vec2 uv = vec2(pos) * pixel_size;
	if (params.half_size) {
		uv *= 2.0;
	}

#ifdef MODE_GEN_BLUR_SIZE
	uv += pixel_size * 0.5;
	//precompute size in alpha channel
	float depth = get_depth_at_pos(uv);
	float size = get_blur_size(depth);

	vec4 color = imageLoad(color_image, pos);
	color.a = size;
	imageStore(color_image, pos, color);
#endif

#ifdef MODE_BOKEH_BOX

	//pixel_size*=0.5; //resolution is doubled
	if (params.second_pass || !params.half_size) {
		uv += pixel_size * 0.5; //half pixel to read centers
	} else {
		uv += pixel_size * 0.25; //half pixel to read centers from full res
	}

	vec2 dir = (params.second_pass ? vec2(0.0, 1.0) : vec2(1.0, 0.0));

	vec4 color = weighted_filter_dir(dir, uv, pixel_size);

	imageStore(bokeh_image, pos, color);

#endif

#ifdef MODE_BOKEH_HEXAGONAL

	//pixel_size*=0.5; //resolution is doubled
	if (params.second_pass || !params.half_size) {
		uv += pixel_size * 0.5; //half pixel to read centers
	} else {
		uv += pixel_size * 0.25; //half pixel to read centers from full res
	}

	vec2 dir = (params.second_pass ? normalize(vec2(1.0, 0.577350269189626)) : vec2(0.0, 1.0));

	vec4 color = weighted_filter_dir(dir, uv, pixel_size);

	if (params.second_pass) {
		dir = normalize(vec2(-1.0, 0.577350269189626));

		vec4 color2 = weighted_filter_dir(dir, uv, pixel_size);

		color.rgb = min(color.rgb, color2.rgb);
		color.a = (color.a + color2.a) * 0.5;
	}

	imageStore(bokeh_image, pos, color);

#endif

#ifdef MODE_BOKEH_CIRCULAR

	uv += pixel_size * 0.5; //half pixel to read centers

	vec4 color = texture(color_texture, uv);
	float initial_blur = color.a;
	float accum = 1.0;
	float radius = params.blur_scale;

	// A smaller `blur_scaler` means linearly slower convergence of the circle's `radius` to `blur_size`
	// A bigger `blur_size` maps to a faster convergence of the circle's `radius` to `blur_size`
	const float _blur_size_performance = max(0.4, remap(params.blur_size, vec2(12.5, 62.5), vec2(0.4, 1.6)));

	// this is used to get a uniform blur although sampling is not done uniformly
	const float base_radius_growth_rate = radius * _blur_size_performance;

	// decide how detailed the sampling should be for optimization purposes, based on `initial_blur` and `params.blur_size`
	float radius_growth_rate = base_radius_growth_rate * (1.0 + 3.0 * abs(initial_blur) / params.blur_size);

	// sample increasingly less towards the end, but for near DoF we don't want this that much, because it starts to look worse quicker than far DoF
	const float radius_growth_rate_acceleration = 1.0 + 0.01 * linearstep(params.blur_size * -1.5, params.blur_size * -0.01, initial_blur);

	// hash the initial angle to get rid of some moire patterns; multiplying by 0.5 seems to improve the fps a bit; also don't hash unfocused points
	float ang = hash12n(uv) * 0.5;// * step(0.0, abs(initial_blur));

	for (; radius < params.blur_size; ) {
		ang += GOLDEN_ANGLE;

		vec2 suv = uv + vec2(cos(ang), sin(ang)) * pixel_size * radius;
		vec4 sample_color = texture(color_texture, suv);
		float sample_size = abs(sample_color.a);

		// limit how much of the far blur will affect the near blur
		if (sample_color.a > initial_blur) {
			sample_size = clamp(sample_size, 0.0, abs(initial_blur) * remap(params.blur_size, vec2(12.5, 62.5), vec2(1.8, 2.1)));
		}
		
		float contribution = linearstep(radius - 0.5, radius + 0.5, sample_size);

		// since the radius_growth_rate is not constant, but we still want a uniform coverage, we'll account for that by increasing the sample's weight (in addition to it's contribution)
		float _radius_growth_rate = mix(radius_growth_rate, base_radius_growth_rate, linearstep(params.blur_size * 0.9, params.blur_size, radius));
		float weight = _radius_growth_rate / base_radius_growth_rate;

		color += mix(color / accum, sample_color, contribution) * weight;
		accum += weight;

		// add a growth rate to the growth rate for a performance/quality ratio increase
		radius_growth_rate *= radius_growth_rate_acceleration;
		radius += _radius_growth_rate / radius;
	}

	color /= accum;

	imageStore(bokeh_image, pos, color);
#endif

#ifdef MODE_COMPOSITE_BOKEH

	uv += pixel_size * 0.5;
	vec4 color = imageLoad(color_image, pos);

	vec4 bokeh = texture(source_bokeh, uv);

#ifdef DENOISE

	// since we're doing a third pass for upscaling, why not apply some quick blur for a bit of denoising
// 	vec2 blur_distance = remap(params.blur_size, vec2(12.5, 62.5), vec2(0.5, 0.75)) * (hash12n(uv) + 1.0, hash12n(uv.yx) + 1.0) * pixel_size;
// 	vec4 bokeh_blured = bokeh * 0.4
// 		+ texture(source_bokeh, uv + blur_distance) * 0.15
// 		+ texture(source_bokeh, uv - blur_distance) * 0.15
// 		+ texture(source_bokeh, uv + vec2(blur_distance.x, -blur_distance.y)) * 0.15
// 		+ texture(source_bokeh, uv + vec2(-blur_distance.x, blur_distance.y)) * 0.15
// 	bokeh.rgb = bokeh_blured.rgb;

	vec4 denoisedBokeh = sirBirdDenoise(source_bokeh, uv, pixel_size * (1.0 + abs(bokeh.a) / params.blur_size));
	bokeh.rgb = mix(
		bokeh.rgb, 
		denoisedBokeh.rgb, 
		1.0 * step(abs(bokeh.a) + 0.01, abs(denoisedBokeh.a))//2.0 * (clamp(abs(bokeh.a) + abs(denoisedBokeh.a), 0.0, 1.0))
	);

// 	radius = 1.0;
// 	for (; radius < params.blur_size * 0.1; ) {
// 		ang += GOLDEN_ANGLE;
//		vec2 suv = uv + vec2(cos(ang), sin(ang)) * pixel_size * radius;
// 	}

#endif

	float mix_amount;
	if (bokeh.a < color.a) {
		mix_amount = min(1.0, max(0.0, max(abs(color.a), abs(bokeh.a)) - DEPTH_GAP));
	} else {
		mix_amount = min(1.0, max(0.0, abs(color.a) - DEPTH_GAP));
	}

	color.rgb = mix(color.rgb, bokeh.rgb, mix_amount); //blend between hires and lowres

	color.a = 0; //reset alpha
	imageStore(color_image, pos, color);
#endif
}
