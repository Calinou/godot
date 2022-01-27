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
		return -(1.0 - smoothstep(params.blur_near_end, params.blur_near_begin, depth)) * params.blur_size - DEPTH_GAP; //near blur is negative
	}

	if (params.blur_far_active && depth > params.blur_far_begin) {
		return smoothstep(params.blur_far_begin, params.blur_far_end, depth) * params.blur_size + DEPTH_GAP;
	}

	return 0.0;
}

#endif

#ifdef MODE_BOKEH_CIRCULAR

float remap(float value, vec2 from, vec2 to) {
	return to.x + (value - from.x) * (to.y - to.x) / (from.y - from.x);
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

	if (any(greaterThan(pos, params.size))) { //too large, do nothing
		return;
	}

	vec2 pixel_size = 1.0 / vec2(params.size);
	vec2 uv = vec2(pos) / vec2(params.size);

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

	if (params.half_size) {
		pixel_size *= 0.5; //resolution is doubled
	}

	uv += pixel_size * 0.5; //half pixel to read centers

	vec4 color = texture(color_texture, uv);
	float initial_blur = color.a;
	float accum = 1.0;
	float radius = params.blur_scale;

	//radius -= smoothstep(0.0, params.blur_size * -0.25, color.a) * smoothstep(-params.blur_size, params.blur_size * -0.75, color.a) * 0.5 * radius;
	
	// A smaller `blur_scaler` means linearly slower convergence of the circle's `radius` to `blur_size`
	// A bigger `blur_size` maps to a faster convergence of the circle's `radius` to `blur_size`
	float _blur_size_performance = max(0.4, remap(params.blur_size, vec2(12.5, 62.5), vec2(0.4, 1.6)));
	
	float base_radius_growth_rate = radius * _blur_size_performance;
	float radius_growth_rate = base_radius_growth_rate * (1.0 + 1.1 * abs(initial_blur) / params.blur_size);
	
	for (float ang = 0.0; radius < params.blur_size; ang += GOLDEN_ANGLE) {
		vec2 suv = uv + vec2(cos(ang), sin(ang)) * pixel_size * radius;
		vec4 sample_color = texture(color_texture, suv);
		float sample_size = abs(sample_color.a);
		if (sample_color.a > initial_blur) {
			sample_size = clamp(sample_size, 0.0, abs(initial_blur) * remap(params.blur_size, vec2(12.5, 62.5), vec2(1.0, 2.0)));
		}

		float m = smoothstep(radius - 0.5, radius + 0.5, sample_size);
		float _m = abs(m - 0.5) * 2.0;
		
// 		radius_growth_rate = mix(radius_growth_rate + 0.4 * radius_growth_rate * _m, base_radius_growth_rate, _m * pow(m, 3.0));

		// since the radius is growing increasingly faster, but we still want a uniform coverage, we'll account for that by increasing their influence
		float strength = remap(color.a, vec2(-params.blur_size, params.blur_size), vec2(radius_growth_rate / base_radius_growth_rate, 1.0));
		color += mix(color / accum, sample_color, m) * strength;
		accum += strength;
		
		radius_growth_rate *= (
			1.0 
			+ mix(0.01 * _m, 0.005, hash12n(uv) * 0.5)// * (1.0 - 0.5 * smoothstep(0.0, params.blur_size * -0.25, color.a / accum) * smoothstep(-params.blur_size, params.blur_size * -0.5, color.a / accum))
		);
		radius += radius_growth_rate / radius; // divide by sqrt(radius) to sample more points closer to center of circle * mix(inversesqrt(radius), 1.0 / radius, m);
	}

	color /= accum;

	imageStore(bokeh_image, pos, color);
#endif

#ifdef MODE_COMPOSITE_BOKEH

	uv += pixel_size * 0.5;
	vec4 color = imageLoad(color_image, pos);
	vec4 bokeh = texture(source_bokeh, uv);

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
