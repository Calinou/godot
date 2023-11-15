/**************************************************************************/
/*  texture_progress_bar_3d.cpp                                           */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "texture_progress_bar_3d.h"

#include "scene/main/viewport.h"
#include "scene/resources/theme.h"
#include "scene/scene_string_names.h"
#include "scene/theme/theme_db.h"

void TextureProgressBar3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_pixel_size", "size"), &TextureProgressBar3D::set_pixel_size);
	ClassDB::bind_method(D_METHOD("get_pixel_size"), &TextureProgressBar3D::get_pixel_size);

	ClassDB::bind_method(D_METHOD("set_value", "value"), &TextureProgressBar3D::set_value);
	ClassDB::bind_method(D_METHOD("set_value_no_signal", "value"), &TextureProgressBar3D::set_value_no_signal);
	ClassDB::bind_method(D_METHOD("get_value"), &TextureProgressBar3D::get_value);

	ClassDB::bind_method(D_METHOD("set_min", "minimum"), &TextureProgressBar3D::set_min);
	ClassDB::bind_method(D_METHOD("get_min"), &TextureProgressBar3D::get_min);

	ClassDB::bind_method(D_METHOD("set_max", "maximum"), &TextureProgressBar3D::set_max);
	ClassDB::bind_method(D_METHOD("get_max"), &TextureProgressBar3D::get_max);

	ClassDB::bind_method(D_METHOD("set_step", "step"), &TextureProgressBar3D::set_step);
	ClassDB::bind_method(D_METHOD("get_step"), &TextureProgressBar3D::get_step);

	ClassDB::bind_method(D_METHOD("set_as_ratio", "value"), &TextureProgressBar3D::set_as_ratio);
	ClassDB::bind_method(D_METHOD("get_as_ratio"), &TextureProgressBar3D::get_as_ratio);

	ClassDB::bind_method(D_METHOD("set_use_rounded_values", "enabled"), &TextureProgressBar3D::set_use_rounded_values);
	ClassDB::bind_method(D_METHOD("is_using_rounded_values"), &TextureProgressBar3D::is_using_rounded_values);

	ClassDB::bind_method(D_METHOD("set_exp_ratio", "enabled"), &TextureProgressBar3D::set_exp_ratio);
	ClassDB::bind_method(D_METHOD("is_ratio_exp"), &TextureProgressBar3D::is_ratio_exp);

	ClassDB::bind_method(D_METHOD("set_allow_greater", "allow"), &TextureProgressBar3D::set_allow_greater);
	ClassDB::bind_method(D_METHOD("is_greater_allowed"), &TextureProgressBar3D::is_greater_allowed);

	ClassDB::bind_method(D_METHOD("set_allow_lesser", "allow"), &TextureProgressBar3D::set_allow_lesser);
	ClassDB::bind_method(D_METHOD("is_lesser_allowed"), &TextureProgressBar3D::is_lesser_allowed);

	ClassDB::bind_method(D_METHOD("set_under_texture", "tex"), &TextureProgressBar3D::set_under_texture);
	ClassDB::bind_method(D_METHOD("get_under_texture"), &TextureProgressBar3D::get_under_texture);

	ClassDB::bind_method(D_METHOD("set_progress_texture", "tex"), &TextureProgressBar3D::set_progress_texture);
	ClassDB::bind_method(D_METHOD("get_progress_texture"), &TextureProgressBar3D::get_progress_texture);

	ClassDB::bind_method(D_METHOD("set_over_texture", "tex"), &TextureProgressBar3D::set_over_texture);
	ClassDB::bind_method(D_METHOD("get_over_texture"), &TextureProgressBar3D::get_over_texture);

	ClassDB::bind_method(D_METHOD("set_fill_mode", "mode"), &TextureProgressBar3D::set_fill_mode);
	ClassDB::bind_method(D_METHOD("get_fill_mode"), &TextureProgressBar3D::get_fill_mode);

	ClassDB::bind_method(D_METHOD("set_tint_under", "tint"), &TextureProgressBar3D::set_tint_under);
	ClassDB::bind_method(D_METHOD("get_tint_under"), &TextureProgressBar3D::get_tint_under);

	ClassDB::bind_method(D_METHOD("set_tint_progress", "tint"), &TextureProgressBar3D::set_tint_progress);
	ClassDB::bind_method(D_METHOD("get_tint_progress"), &TextureProgressBar3D::get_tint_progress);

	ClassDB::bind_method(D_METHOD("set_tint_over", "tint"), &TextureProgressBar3D::set_tint_over);
	ClassDB::bind_method(D_METHOD("get_tint_over"), &TextureProgressBar3D::get_tint_over);

	ClassDB::bind_method(D_METHOD("set_texture_progress_offset", "offset"), &TextureProgressBar3D::set_progress_offset);
	ClassDB::bind_method(D_METHOD("get_texture_progress_offset"), &TextureProgressBar3D::get_progress_offset);

	ClassDB::bind_method(D_METHOD("set_radial_initial_angle", "mode"), &TextureProgressBar3D::set_radial_initial_angle);
	ClassDB::bind_method(D_METHOD("get_radial_initial_angle"), &TextureProgressBar3D::get_radial_initial_angle);

	ClassDB::bind_method(D_METHOD("set_radial_center_offset", "mode"), &TextureProgressBar3D::set_radial_center_offset);
	ClassDB::bind_method(D_METHOD("get_radial_center_offset"), &TextureProgressBar3D::get_radial_center_offset);

	ClassDB::bind_method(D_METHOD("set_fill_degrees", "mode"), &TextureProgressBar3D::set_fill_degrees);
	ClassDB::bind_method(D_METHOD("get_fill_degrees"), &TextureProgressBar3D::get_fill_degrees);

	ClassDB::bind_method(D_METHOD("set_stretch_margin", "margin", "value"), &TextureProgressBar3D::set_stretch_margin);
	ClassDB::bind_method(D_METHOD("get_stretch_margin", "margin"), &TextureProgressBar3D::get_stretch_margin);

	ClassDB::bind_method(D_METHOD("set_nine_patch_stretch", "stretch"), &TextureProgressBar3D::set_nine_patch_stretch);
	ClassDB::bind_method(D_METHOD("get_nine_patch_stretch"), &TextureProgressBar3D::get_nine_patch_stretch);

	ClassDB::bind_method(D_METHOD("set_under_render_priority", "priority"), &TextureProgressBar3D::set_under_render_priority);
	ClassDB::bind_method(D_METHOD("get_under_render_priority"), &TextureProgressBar3D::get_under_render_priority);

	ClassDB::bind_method(D_METHOD("set_progress_render_priority", "priority"), &TextureProgressBar3D::set_progress_render_priority);
	ClassDB::bind_method(D_METHOD("get_progress_render_priority"), &TextureProgressBar3D::get_progress_render_priority);

	ClassDB::bind_method(D_METHOD("set_over_render_priority", "priority"), &TextureProgressBar3D::set_over_render_priority);
	ClassDB::bind_method(D_METHOD("get_over_render_priority"), &TextureProgressBar3D::get_over_render_priority);

	ClassDB::bind_method(D_METHOD("set_draw_flag", "flag", "enabled"), &TextureProgressBar3D::set_draw_flag);
	ClassDB::bind_method(D_METHOD("get_draw_flag", "flag"), &TextureProgressBar3D::get_draw_flag);

	ClassDB::bind_method(D_METHOD("set_billboard_mode", "mode"), &TextureProgressBar3D::set_billboard_mode);
	ClassDB::bind_method(D_METHOD("get_billboard_mode"), &TextureProgressBar3D::get_billboard_mode);

	ClassDB::bind_method(D_METHOD("set_alpha_cut_mode", "mode"), &TextureProgressBar3D::set_alpha_cut_mode);
	ClassDB::bind_method(D_METHOD("get_alpha_cut_mode"), &TextureProgressBar3D::get_alpha_cut_mode);

	ClassDB::bind_method(D_METHOD("set_alpha_scissor_threshold", "threshold"), &TextureProgressBar3D::set_alpha_scissor_threshold);
	ClassDB::bind_method(D_METHOD("get_alpha_scissor_threshold"), &TextureProgressBar3D::get_alpha_scissor_threshold);

	ClassDB::bind_method(D_METHOD("set_alpha_hash_scale", "threshold"), &TextureProgressBar3D::set_alpha_hash_scale);
	ClassDB::bind_method(D_METHOD("get_alpha_hash_scale"), &TextureProgressBar3D::get_alpha_hash_scale);

	ClassDB::bind_method(D_METHOD("set_alpha_antialiasing", "alpha_aa"), &TextureProgressBar3D::set_alpha_antialiasing);
	ClassDB::bind_method(D_METHOD("get_alpha_antialiasing"), &TextureProgressBar3D::get_alpha_antialiasing);

	ClassDB::bind_method(D_METHOD("set_alpha_antialiasing_edge", "edge"), &TextureProgressBar3D::set_alpha_antialiasing_edge);
	ClassDB::bind_method(D_METHOD("get_alpha_antialiasing_edge"), &TextureProgressBar3D::get_alpha_antialiasing_edge);

	ClassDB::bind_method(D_METHOD("set_texture_filter", "mode"), &TextureProgressBar3D::set_texture_filter);
	ClassDB::bind_method(D_METHOD("get_texture_filter"), &TextureProgressBar3D::get_texture_filter);

	ClassDB::bind_method(D_METHOD("generate_triangle_mesh"), &TextureProgressBar3D::generate_triangle_mesh);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "pixel_size", PROPERTY_HINT_RANGE, "0.001,10,0.0001,or_greater,suffix:m"), "set_pixel_size", "get_pixel_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fill_mode", PROPERTY_HINT_ENUM, "Left to Right,Right to Left,Top to Bottom,Bottom to Top,Clockwise,Counter Clockwise,Bilinear (Left and Right),Bilinear (Top and Bottom),Clockwise and Counter Clockwise"), "set_fill_mode", "get_fill_mode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "nine_patch_stretch"), "set_nine_patch_stretch", "get_nine_patch_stretch");

	ADD_GROUP("Stretch Margin", "stretch_margin_");
	ADD_PROPERTYI(PropertyInfo(Variant::INT, "stretch_margin_left", PROPERTY_HINT_RANGE, "0,16384,1,suffix:px"), "set_stretch_margin", "get_stretch_margin", SIDE_LEFT);
	ADD_PROPERTYI(PropertyInfo(Variant::INT, "stretch_margin_top", PROPERTY_HINT_RANGE, "0,16384,1,suffix:px"), "set_stretch_margin", "get_stretch_margin", SIDE_TOP);
	ADD_PROPERTYI(PropertyInfo(Variant::INT, "stretch_margin_right", PROPERTY_HINT_RANGE, "0,16384,1,suffix:px"), "set_stretch_margin", "get_stretch_margin", SIDE_RIGHT);
	ADD_PROPERTYI(PropertyInfo(Variant::INT, "stretch_margin_bottom", PROPERTY_HINT_RANGE, "0,16384,1,suffix:px"), "set_stretch_margin", "get_stretch_margin", SIDE_BOTTOM);

	ADD_GROUP("Textures", "texture_");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_under", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_under_texture", "get_under_texture");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_over", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_over_texture", "get_over_texture");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_progress", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_progress_texture", "get_progress_texture");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "texture_progress_offset", PROPERTY_HINT_NONE, "suffix:px"), "set_texture_progress_offset", "get_texture_progress_offset");

	ADD_GROUP("Tint", "tint_");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "tint_under"), "set_tint_under", "get_tint_under");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "tint_over"), "set_tint_over", "get_tint_over");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "tint_progress"), "set_tint_progress", "get_tint_progress");

	ADD_GROUP("Radial Fill", "radial_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radial_initial_angle", PROPERTY_HINT_RANGE, "0.0,360.0,0.1,slider,degrees"), "set_radial_initial_angle", "get_radial_initial_angle");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radial_fill_degrees", PROPERTY_HINT_RANGE, "0.0,360.0,0.1,slider,degrees"), "set_fill_degrees", "get_fill_degrees");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "radial_center_offset", PROPERTY_HINT_NONE, "suffix:px"), "set_radial_center_offset", "get_radial_center_offset");

	ADD_GROUP("Flags", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "billboard", PROPERTY_HINT_ENUM, "Disabled,Enabled,Y-Billboard"), "set_billboard_mode", "get_billboard_mode");
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "shaded"), "set_draw_flag", "get_draw_flag", FLAG_SHADED);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "double_sided"), "set_draw_flag", "get_draw_flag", FLAG_DOUBLE_SIDED);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "no_depth_test"), "set_draw_flag", "get_draw_flag", FLAG_DISABLE_DEPTH_TEST);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "fixed_size"), "set_draw_flag", "get_draw_flag", FLAG_FIXED_SIZE);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "alpha_cut", PROPERTY_HINT_ENUM, "Disabled,Discard,Opaque Pre-Pass,Alpha Hash"), "set_alpha_cut_mode", "get_alpha_cut_mode");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "alpha_scissor_threshold", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_alpha_scissor_threshold", "get_alpha_scissor_threshold");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "alpha_hash_scale", PROPERTY_HINT_RANGE, "0,2,0.01"), "set_alpha_hash_scale", "get_alpha_hash_scale");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "alpha_antialiasing_mode", PROPERTY_HINT_ENUM, "Disabled,Alpha Edge Blend,Alpha Edge Clip"), "set_alpha_antialiasing", "get_alpha_antialiasing");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "alpha_antialiasing_edge", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_alpha_antialiasing_edge", "get_alpha_antialiasing_edge");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "texture_filter", PROPERTY_HINT_ENUM, "Nearest,Linear,Nearest Mipmap,Linear Mipmap,Nearest Mipmap Anisotropic,Linear Mipmap Anisotropic"), "set_texture_filter", "get_texture_filter");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "render_priority", PROPERTY_HINT_RANGE, itos(RS::MATERIAL_RENDER_PRIORITY_MIN) + "," + itos(RS::MATERIAL_RENDER_PRIORITY_MAX) + ",1"), "set_render_priority", "get_render_priority");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "outline_render_priority", PROPERTY_HINT_RANGE, itos(RS::MATERIAL_RENDER_PRIORITY_MIN) + "," + itos(RS::MATERIAL_RENDER_PRIORITY_MAX) + ",1"), "set_outline_render_priority", "get_outline_render_priority");

	BIND_ENUM_CONSTANT(FLAG_SHADED);
	BIND_ENUM_CONSTANT(FLAG_DOUBLE_SIDED);
	BIND_ENUM_CONSTANT(FLAG_DISABLE_DEPTH_TEST);
	BIND_ENUM_CONSTANT(FLAG_FIXED_SIZE);
	BIND_ENUM_CONSTANT(FLAG_MAX);

	BIND_ENUM_CONSTANT(ALPHA_CUT_DISABLED);
	BIND_ENUM_CONSTANT(ALPHA_CUT_DISCARD);
	BIND_ENUM_CONSTANT(ALPHA_CUT_OPAQUE_PREPASS);
	BIND_ENUM_CONSTANT(ALPHA_CUT_HASH);
}

PackedStringArray TextureProgressBar3D::get_configuration_warnings() const {
	PackedStringArray warnings = Node::get_configuration_warnings();

	if (exp_ratio && min <= 0) {
		warnings.push_back(RTR("If \"Exp Edit\" is enabled, \"Min Value\" must be greater than 0."));
	}

	return warnings;
}

void TextureProgressBar3D::set_value(double p_val) {
	double prev_val = val;
	_set_value_no_signal(p_val);

	if (val != prev_val) {
		emit_signal(SNAME("value_changed"), p_val);
		_queue_update();
	}
}

void TextureProgressBar3D::_set_value_no_signal(double p_val) {
	if (!Math::is_finite(p_val)) {
		return;
	}

	if (step > 0) {
		p_val = Math::round((p_val - min) / step) * step + min;
	}

	if (rounded_values) {
		p_val = Math::round(p_val);
	}

	if (!allow_lesser && p_val < min) {
		p_val = min;
	}

	if (val == p_val) {
		return;
	}

	val = p_val;
}

void TextureProgressBar3D::set_value_no_signal(double p_val) {
	double prev_val = val;
	_set_value_no_signal(p_val);

	if (val != prev_val) {
		_queue_update();
	}
}

void TextureProgressBar3D::set_min(double p_min) {
	if (min == p_min) {
		return;
	}

	min = p_min;
	max = MAX(max, min);
	set_value(val);

	update_configuration_warnings();
}

double TextureProgressBar3D::get_min() const {
	return min;
}

void TextureProgressBar3D::set_max(double p_max) {
	double max_validated = MAX(p_max, min);
	if (max == max_validated) {
		return;
	}

	max = max_validated;
	set_value(val);
}

double TextureProgressBar3D::get_max() const {
	return max;
}

void TextureProgressBar3D::set_step(double p_step) {
	if (step == p_step) {
		return;
	}

	step = p_step;
}

double TextureProgressBar3D::get_step() const {
	return step;
}

void TextureProgressBar3D::set_as_ratio(double p_value) {
	double v;

	if (exp_ratio && get_min() >= 0) {
		double exp_min = get_min() == 0 ? 0.0 : Math::log(get_min()) / Math::log((double)2);
		double exp_max = Math::log(get_max()) / Math::log((double)2);
		v = Math::pow(2, exp_min + (exp_max - exp_min) * p_value);
	} else {
		double percent = (get_max() - get_min()) * p_value;
		if (get_step() > 0) {
			double steps = round(percent / get_step());
			v = steps * get_step() + get_min();
		} else {
			v = percent + get_min();
		}
	}
	v = CLAMP(v, get_min(), get_max());
	set_value(v);
}

double TextureProgressBar3D::get_as_ratio() const {
	if (Math::is_equal_approx(get_max(), get_min())) {
		// Avoid division by zero.
		return 1.0;
	}

	if (exp_ratio && get_min() >= 0) {
		double exp_min = get_min() == 0 ? 0.0 : Math::log(get_min()) / Math::log((double)2);
		double exp_max = Math::log(get_max()) / Math::log((double)2);
		float value = CLAMP(get_value(), min, max);
		double v = Math::log(value) / Math::log((double)2);

		return CLAMP((v - exp_min) / (exp_max - exp_min), 0, 1);
	} else {
		float value = CLAMP(get_value(), min, max);
		return CLAMP((value - get_min()) / (get_max() - get_min()), 0, 1);
	}
}

void TextureProgressBar3D::set_min(double p_min) {
}

double TextureProgressBar3D::get_min() const {
}

void TextureProgressBar3D::set_min(double p_min) {
}

double TextureProgressBar3D::get_min() const {
}

void TextureProgressBar3D::set_use_rounded_values(bool p_enable) {
	rounded_values = p_enable;
}

bool TextureProgressBar3D::is_using_rounded_values() const {
	return rounded_values;
}

void TextureProgressBar3D::set_exp_ratio(bool p_enable) {
	if (exp_ratio == p_enable) {
		return;
	}

	exp_ratio = p_enable;

	update_configuration_warnings();
}

bool TextureProgressBar3D::is_ratio_exp() const {
	return exp_ratio;
}

void TextureProgressBar3D::set_allow_greater(bool p_allow) {
	allow_greater = p_allow;
}

bool TextureProgressBar3D::is_greater_allowed() const {
	return allow_greater;
}

void TextureProgressBar3D::set_allow_lesser(bool p_allow) {
	allow_lesser = p_allow;
}

bool TextureProgressBar3D::is_lesser_allowed() const {
	return allow_lesser;
}

void TextureProgressBar3D::_validate_property(PropertyInfo &p_property) const {
	if (
			p_property.name == "material_override" ||
			p_property.name == "material_overlay" ||
			p_property.name == "lod_bias" ||
			p_property.name == "gi_mode" ||
			p_property.name == "gi_lightmap_scale") {
		p_property.usage = PROPERTY_USAGE_NO_EDITOR;
	}

	if (p_property.name == "cast_shadow" && alpha_cut == ALPHA_CUT_DISABLED) {
		// Alpha-blended materials can't cast shadows.
		p_property.usage = PROPERTY_USAGE_NO_EDITOR;
	}
}

void TextureProgressBar3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (!pending_update) {
				_im_update();
			}
			Viewport *viewport = get_viewport();
			ERR_FAIL_NULL(viewport);
			viewport->connect("size_changed", callable_mp(this, &TextureProgressBar3D::_font_changed));
		} break;
		case NOTIFICATION_EXIT_TREE: {
			Viewport *viewport = get_viewport();
			ERR_FAIL_NULL(viewport);
			viewport->disconnect("size_changed", callable_mp(this, &TextureProgressBar3D::_font_changed));
		} break;
		case NOTIFICATION_TRANSLATION_CHANGED: {
			String new_text = tr(text);
			if (new_text == xl_text) {
				return; // Nothing new.
			}
			xl_text = new_text;
			dirty_text = true;
			_queue_update();
		} break;
	}
}

void TextureProgressBar3D::_im_update() {
	_shape();

	triangle_mesh.unref();
	update_gizmos();

	pending_update = false;
}

void TextureProgressBar3D::_queue_update() {
	if (pending_update) {
		return;
	}

	pending_update = true;
	callable_mp(this, &TextureProgressBar3D::_im_update).call_deferred();
}

AABB TextureProgressBar3D::get_aabb() const {
	return aabb;
}

Ref<TriangleMesh> TextureProgressBar3D::generate_triangle_mesh() const {
	if (triangle_mesh.is_valid()) {
		return triangle_mesh;
	}

	Ref<Font> font = _get_font_or_default();
	if (font.is_null()) {
		return Ref<TriangleMesh>();
	}

	Vector<Vector3> faces;
	faces.resize(6);
	Vector3 *facesw = faces.ptrw();

	float total_h = 0.0;
	float max_line_w = 0.0;
	for (int i = 0; i < lines_rid.size(); i++) {
		total_h += TS->shaped_text_get_size(lines_rid[i]).y + line_spacing;
		max_line_w = MAX(max_line_w, TS->shaped_text_get_width(lines_rid[i]));
	}

	float vbegin = 0;
	switch (vertical_alignment) {
		case VERTICAL_ALIGNMENT_FILL:
		case VERTICAL_ALIGNMENT_TOP: {
			// Nothing.
		} break;
		case VERTICAL_ALIGNMENT_CENTER: {
			vbegin = (total_h - line_spacing) / 2.0;
		} break;
		case VERTICAL_ALIGNMENT_BOTTOM: {
			vbegin = (total_h - line_spacing);
		} break;
	}

	Vector2 offset = Vector2(0, vbegin);
	switch (horizontal_alignment) {
		case HORIZONTAL_ALIGNMENT_LEFT:
			break;
		case HORIZONTAL_ALIGNMENT_FILL:
		case HORIZONTAL_ALIGNMENT_CENTER: {
			offset.x = -max_line_w / 2.0;
		} break;
		case HORIZONTAL_ALIGNMENT_RIGHT: {
			offset.x = -max_line_w;
		} break;
	}

	Rect2 final_rect = Rect2(offset + lbl_offset, Size2(max_line_w, total_h));

	if (final_rect.size.x == 0 || final_rect.size.y == 0) {
		return Ref<TriangleMesh>();
	}

	real_t px_size = get_pixel_size();

	Vector2 vertices[4] = {

		(final_rect.position + Vector2(0, -final_rect.size.y)) * px_size,
		(final_rect.position + Vector2(final_rect.size.x, -final_rect.size.y)) * px_size,
		(final_rect.position + Vector2(final_rect.size.x, 0)) * px_size,
		final_rect.position * px_size,

	};

	static const int indices[6] = {
		0, 1, 2,
		0, 2, 3
	};

	for (int j = 0; j < 6; j++) {
		int i = indices[j];
		Vector3 vtx;
		vtx[0] = vertices[i][0];
		vtx[1] = vertices[i][1];
		facesw[j] = vtx;
	}

	triangle_mesh = Ref<TriangleMesh>(memnew(TriangleMesh));
	triangle_mesh->create(faces);

	return triangle_mesh;
}

void TextureProgressBar3D::_generate_glyph_surfaces(const Glyph &p_glyph, Vector2 &r_offset, const Color &p_modulate, int p_priority, int p_outline_size) {
	for (int j = 0; j < p_glyph.repeat; j++) {
		Vector2 gl_of;
		Vector2 gl_sz;
		Rect2 gl_uv;
		Size2 texs;
		RID tex;

		if (p_glyph.font_rid != RID()) {
			tex = TS->font_get_glyph_texture_rid(p_glyph.font_rid, Vector2i(p_glyph.font_size, p_outline_size), p_glyph.index);
			if (tex != RID()) {
				gl_of = (TS->font_get_glyph_offset(p_glyph.font_rid, Vector2i(p_glyph.font_size, p_outline_size), p_glyph.index) + Vector2(p_glyph.x_off, p_glyph.y_off)) * pixel_size;
				gl_sz = TS->font_get_glyph_size(p_glyph.font_rid, Vector2i(p_glyph.font_size, p_outline_size), p_glyph.index) * pixel_size;
				gl_uv = TS->font_get_glyph_uv_rect(p_glyph.font_rid, Vector2i(p_glyph.font_size, p_outline_size), p_glyph.index);
				texs = TS->font_get_glyph_texture_size(p_glyph.font_rid, Vector2i(p_glyph.font_size, p_outline_size), p_glyph.index);
			}
		} else {
			gl_sz = TS->get_hex_code_box_size(p_glyph.font_size, p_glyph.index) * pixel_size;
			gl_of = Vector2(0, -gl_sz.y);
		}

		bool msdf = TS->font_is_multichannel_signed_distance_field(p_glyph.font_rid);

		SurfaceKey key = SurfaceKey(tex.get_id(), p_priority, p_outline_size);
		if (!surfaces.has(key)) {
			SurfaceData surf;
			surf.material = RenderingServer::get_singleton()->material_create();
			// Set defaults for material, names need to match up those in StandardMaterial3D
			RS::get_singleton()->material_set_param(surf.material, "albedo", Color(1, 1, 1, 1));
			RS::get_singleton()->material_set_param(surf.material, "specular", 0.5);
			RS::get_singleton()->material_set_param(surf.material, "metallic", 0.0);
			RS::get_singleton()->material_set_param(surf.material, "roughness", 1.0);
			RS::get_singleton()->material_set_param(surf.material, "uv1_offset", Vector3(0, 0, 0));
			RS::get_singleton()->material_set_param(surf.material, "uv1_scale", Vector3(1, 1, 1));
			RS::get_singleton()->material_set_param(surf.material, "uv2_offset", Vector3(0, 0, 0));
			RS::get_singleton()->material_set_param(surf.material, "uv2_scale", Vector3(1, 1, 1));
			RS::get_singleton()->material_set_param(surf.material, "alpha_scissor_threshold", alpha_scissor_threshold);
			RS::get_singleton()->material_set_param(surf.material, "alpha_hash_scale", alpha_hash_scale);
			RS::get_singleton()->material_set_param(surf.material, "alpha_antialiasing_edge", alpha_antialiasing_edge);
			if (msdf) {
				RS::get_singleton()->material_set_param(surf.material, "msdf_pixel_range", TS->font_get_msdf_pixel_range(p_glyph.font_rid));
				RS::get_singleton()->material_set_param(surf.material, "msdf_outline_size", p_outline_size);
			}

			BaseMaterial3D::Transparency mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA;
			if (get_alpha_cut_mode() == ALPHA_CUT_DISCARD) {
				mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA_SCISSOR;
			} else if (get_alpha_cut_mode() == ALPHA_CUT_OPAQUE_PREPASS) {
				mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA_DEPTH_PRE_PASS;
			} else if (get_alpha_cut_mode() == ALPHA_CUT_HASH) {
				mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA_HASH;
			}

			RID shader_rid;
			StandardMaterial3D::get_material_for_2d(get_draw_flag(FLAG_SHADED), mat_transparency, get_draw_flag(FLAG_DOUBLE_SIDED), get_billboard_mode() == StandardMaterial3D::BILLBOARD_ENABLED, get_billboard_mode() == StandardMaterial3D::BILLBOARD_FIXED_Y, msdf, get_draw_flag(FLAG_DISABLE_DEPTH_TEST), get_draw_flag(FLAG_FIXED_SIZE), texture_filter, alpha_antialiasing_mode, &shader_rid);

			RS::get_singleton()->material_set_shader(surf.material, shader_rid);
			RS::get_singleton()->material_set_param(surf.material, "texture_albedo", tex);
			if (get_alpha_cut_mode() == ALPHA_CUT_DISABLED) {
				RS::get_singleton()->material_set_render_priority(surf.material, p_priority);
			} else {
				surf.z_shift = p_priority * pixel_size;
			}

			surfaces[key] = surf;
		}
		SurfaceData &s = surfaces[key];

		s.mesh_vertices.resize((s.offset + 1) * 4);
		s.mesh_normals.resize((s.offset + 1) * 4);
		s.mesh_tangents.resize((s.offset + 1) * 16);
		s.mesh_colors.resize((s.offset + 1) * 4);
		s.mesh_uvs.resize((s.offset + 1) * 4);

		s.mesh_vertices.write[(s.offset * 4) + 3] = Vector3(r_offset.x + gl_of.x, r_offset.y - gl_of.y - gl_sz.y, s.z_shift);
		s.mesh_vertices.write[(s.offset * 4) + 2] = Vector3(r_offset.x + gl_of.x + gl_sz.x, r_offset.y - gl_of.y - gl_sz.y, s.z_shift);
		s.mesh_vertices.write[(s.offset * 4) + 1] = Vector3(r_offset.x + gl_of.x + gl_sz.x, r_offset.y - gl_of.y, s.z_shift);
		s.mesh_vertices.write[(s.offset * 4) + 0] = Vector3(r_offset.x + gl_of.x, r_offset.y - gl_of.y, s.z_shift);

		for (int i = 0; i < 4; i++) {
			s.mesh_normals.write[(s.offset * 4) + i] = Vector3(0.0, 0.0, 1.0);
			s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 0] = 1.0;
			s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 1] = 0.0;
			s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 2] = 0.0;
			s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 3] = 1.0;
			s.mesh_colors.write[(s.offset * 4) + i] = p_modulate;
			s.mesh_uvs.write[(s.offset * 4) + i] = Vector2();

			if (aabb == AABB()) {
				aabb.position = s.mesh_vertices[(s.offset * 4) + i];
			} else {
				aabb.expand_to(s.mesh_vertices[(s.offset * 4) + i]);
			}
		}

		if (tex != RID()) {
			s.mesh_uvs.write[(s.offset * 4) + 3] = Vector2(gl_uv.position.x / texs.x, (gl_uv.position.y + gl_uv.size.y) / texs.y);
			s.mesh_uvs.write[(s.offset * 4) + 2] = Vector2((gl_uv.position.x + gl_uv.size.x) / texs.x, (gl_uv.position.y + gl_uv.size.y) / texs.y);
			s.mesh_uvs.write[(s.offset * 4) + 1] = Vector2((gl_uv.position.x + gl_uv.size.x) / texs.x, gl_uv.position.y / texs.y);
			s.mesh_uvs.write[(s.offset * 4) + 0] = Vector2(gl_uv.position.x / texs.x, gl_uv.position.y / texs.y);
		}

		s.indices.resize((s.offset + 1) * 6);
		s.indices.write[(s.offset * 6) + 0] = (s.offset * 4) + 0;
		s.indices.write[(s.offset * 6) + 1] = (s.offset * 4) + 1;
		s.indices.write[(s.offset * 6) + 2] = (s.offset * 4) + 2;
		s.indices.write[(s.offset * 6) + 3] = (s.offset * 4) + 0;
		s.indices.write[(s.offset * 6) + 4] = (s.offset * 4) + 2;
		s.indices.write[(s.offset * 6) + 5] = (s.offset * 4) + 3;

		s.offset++;
		r_offset.x += p_glyph.advance * pixel_size;
	}
}

void TextureProgressBar3D::_draw_progress_bar() {
	ERR_FAIL_COND(p_texture.is_null());

	Rect2 final_rect;
	Rect2 final_src_rect;
	if (!p_texture->get_rect_region(p_dst_rect, p_src_rect, final_rect, final_src_rect)) {
		return;
	}

	if (final_rect.size.x == 0 || final_rect.size.y == 0) {
		return;
	}

	// 2D:                                                     3D plane (axes match exactly when `axis == Vector3::AXIS_Z`):
	//   -X+                                                     -X+
	//  -                                                       +
	//  Y  +--------+       +--------+       +--------+         Y  +--------+
	//  +  | +--+   |       |        |  (2)  |        |         -  | 0--1   |
	//     | |ab|   |  (1)  | +--+   |  (3)  | 3--2   |            | |ab|   |
	//     | |cd|   |  -->  | |ab|   |  -->  | |cd|   |    <==>    | |cd|   |
	//     | +--+   |       | |cd|   |       | |ab|   |            | 3--2   |
	//     |        |       | +--+   |       | 0--1   |            |        |
	//     +--------+       +--------+       +--------+            +--------+

	// (1) Y-wise shift `final_rect` within `p_dst_rect` so after inverting Y
	// axis distances between top/bottom borders will be preserved (so for
	// example AtlasTextures with vertical margins will look the same in 2D/3D).
	final_rect.position.y = (p_dst_rect.position.y + p_dst_rect.size.y) - ((final_rect.position.y + final_rect.size.y) - p_dst_rect.position.y);

	Color color = _get_color_accum();

	real_t px_size = get_pixel_size();

	// (2) Order vertices (0123) bottom-top in 2D / top-bottom in 3D.
	Vector2 vertices[4] = {
		(final_rect.position + Vector2(0, final_rect.size.y)) * px_size,
		(final_rect.position + final_rect.size) * px_size,
		(final_rect.position + Vector2(final_rect.size.x, 0)) * px_size,
		final_rect.position * px_size,
	};

	Vector2 src_tsize = p_texture->get_size();

	// Properly setup UVs for impostor textures (AtlasTexture).
	Ref<AtlasTexture> atlas_tex = p_texture;
	if (atlas_tex != nullptr) {
		src_tsize[0] = atlas_tex->get_atlas()->get_width();
		src_tsize[1] = atlas_tex->get_atlas()->get_height();
	}

	// (3) Assign UVs (abcd) according to the vertices order (bottom-top in 2D / top-bottom in 3D).
	Vector2 uvs[4] = {
		final_src_rect.position / src_tsize,
		(final_src_rect.position + Vector2(final_src_rect.size.x, 0)) / src_tsize,
		(final_src_rect.position + final_src_rect.size) / src_tsize,
		(final_src_rect.position + Vector2(0, final_src_rect.size.y)) / src_tsize,
	};

	if (is_flipped_h()) {
		SWAP(uvs[0], uvs[1]);
		SWAP(uvs[2], uvs[3]);
	}

	if (is_flipped_v()) {
		SWAP(uvs[0], uvs[3]);
		SWAP(uvs[1], uvs[2]);
	}

	Vector3 normal;
	int ax = get_axis();
	normal[ax] = 1.0;

	Plane tangent;
	if (ax == Vector3::AXIS_X) {
		tangent = Plane(0, 0, -1, 1);
	} else {
		tangent = Plane(1, 0, 0, 1);
	}

	int x_axis = ((ax + 1) % 3);
	int y_axis = ((ax + 2) % 3);

	if (ax != Vector3::AXIS_Z) {
		SWAP(x_axis, y_axis);

		for (int i = 0; i < 4; i++) {
			//uvs[i] = Vector2(1.0,1.0)-uvs[i];
			//SWAP(vertices[i].x,vertices[i].y);
			if (ax == Vector3::AXIS_Y) {
				vertices[i].y = -vertices[i].y;
			} else if (ax == Vector3::AXIS_X) {
				vertices[i].x = -vertices[i].x;
			}
		}
	}

	AABB aabb_new;

	// Everything except position and UV is compressed.
	uint8_t *vertex_write_buffer = vertex_buffer.ptrw();
	uint8_t *attribute_write_buffer = attribute_buffer.ptrw();

	uint32_t v_normal;
	{
		Vector2 res = normal.octahedron_encode();
		uint32_t value = 0;
		value |= (uint16_t)CLAMP(res.x * 65535, 0, 65535);
		value |= (uint16_t)CLAMP(res.y * 65535, 0, 65535) << 16;

		v_normal = value;
	}
	uint32_t v_tangent;
	{
		Plane t = tangent;
		Vector2 res = t.normal.octahedron_tangent_encode(t.d);
		uint32_t value = 0;
		value |= (uint16_t)CLAMP(res.x * 65535, 0, 65535);
		value |= (uint16_t)CLAMP(res.y * 65535, 0, 65535) << 16;
		if (value == 4294901760) {
			// (1, 1) and (0, 1) decode to the same value, but (0, 1) messes with our compression detection.
			// So we sanitize here.
			value = 4294967295;
		}

		v_tangent = value;
	}

	uint8_t v_color[4] = {
		uint8_t(CLAMP(color.r * 255.0, 0.0, 255.0)),
		uint8_t(CLAMP(color.g * 255.0, 0.0, 255.0)),
		uint8_t(CLAMP(color.b * 255.0, 0.0, 255.0)),
		uint8_t(CLAMP(color.a * 255.0, 0.0, 255.0))
	};

	for (int i = 0; i < 4; i++) {
		Vector3 vtx;
		vtx[x_axis] = vertices[i][0];
		vtx[y_axis] = vertices[i][1];
		if (i == 0) {
			aabb_new.position = vtx;
			aabb_new.size = Vector3();
		} else {
			aabb_new.expand_to(vtx);
		}

		float v_uv[2] = { (float)uvs[i].x, (float)uvs[i].y };
		memcpy(&attribute_write_buffer[i * attrib_stride + mesh_surface_offsets[RS::ARRAY_TEX_UV]], v_uv, 8);

		float v_vertex[3] = { (float)vtx.x, (float)vtx.y, (float)vtx.z };

		memcpy(&vertex_write_buffer[i * vertex_stride + mesh_surface_offsets[RS::ARRAY_VERTEX]], &v_vertex, sizeof(float) * 3);
		memcpy(&vertex_write_buffer[i * normal_tangent_stride + mesh_surface_offsets[RS::ARRAY_NORMAL]], &v_normal, 4);
		memcpy(&vertex_write_buffer[i * normal_tangent_stride + mesh_surface_offsets[RS::ARRAY_TANGENT]], &v_tangent, 4);
		memcpy(&attribute_write_buffer[i * attrib_stride + mesh_surface_offsets[RS::ARRAY_COLOR]], v_color, 4);
	}

	RID mesh_new = get_mesh();
	RS::get_singleton()->mesh_surface_update_vertex_region(mesh_new, 0, 0, vertex_buffer);
	RS::get_singleton()->mesh_surface_update_attribute_region(mesh_new, 0, 0, attribute_buffer);

	RS::get_singleton()->mesh_set_custom_aabb(mesh_new, aabb_new);
	set_aabb(aabb_new);

	RS::get_singleton()->material_set_param(get_material(), "alpha_scissor_threshold", alpha_scissor_threshold);
	RS::get_singleton()->material_set_param(get_material(), "alpha_hash_scale", alpha_hash_scale);
	RS::get_singleton()->material_set_param(get_material(), "alpha_antialiasing_edge", alpha_antialiasing_edge);

	BaseMaterial3D::Transparency mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_DISABLED;
	if (get_draw_flag(FLAG_TRANSPARENT)) {
		if (get_alpha_cut_mode() == ALPHA_CUT_DISCARD) {
			mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA_SCISSOR;
		} else if (get_alpha_cut_mode() == ALPHA_CUT_OPAQUE_PREPASS) {
			mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA_DEPTH_PRE_PASS;
		} else if (get_alpha_cut_mode() == ALPHA_CUT_HASH) {
			mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA_HASH;
		} else {
			mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA;
		}
	}

	RID shader_rid;
	StandardMaterial3D::get_material_for_2d(get_draw_flag(FLAG_SHADED), mat_transparency, get_draw_flag(FLAG_DOUBLE_SIDED), get_billboard_mode() == StandardMaterial3D::BILLBOARD_ENABLED, get_billboard_mode() == StandardMaterial3D::BILLBOARD_FIXED_Y, false, get_draw_flag(FLAG_DISABLE_DEPTH_TEST), get_draw_flag(FLAG_FIXED_SIZE), get_texture_filter(), alpha_antialiasing_mode, &shader_rid);

	if (last_shader != shader_rid) {
		RS::get_singleton()->material_set_shader(get_material(), shader_rid);
		last_shader = shader_rid;
	}
	if (last_texture != p_texture->get_rid()) {
		RS::get_singleton()->material_set_param(get_material(), "texture_albedo", p_texture->get_rid());
		last_texture = p_texture->get_rid();
	}
	if (get_alpha_cut_mode() == ALPHA_CUT_DISABLED) {
		RS::get_singleton()->material_set_render_priority(get_material(), get_render_priority());
		RS::get_singleton()->mesh_surface_set_material(mesh, 0, get_material());
	}
}


void TextureProgressBar3D::set_render_priority(int p_priority) {
	ERR_FAIL_COND(p_priority < RS::MATERIAL_RENDER_PRIORITY_MIN || p_priority > RS::MATERIAL_RENDER_PRIORITY_MAX);
	if (render_priority != p_priority) {
		render_priority = p_priority;
		_queue_update();
	}
}

int TextureProgressBar3D::get_render_priority() const {
	return render_priority;
}

void TextureProgressBar3D::set_pixel_size(real_t p_amount) {
	if (pixel_size != p_amount) {
		pixel_size = p_amount;
		_queue_update();
	}
}

real_t TextureProgressBar3D::get_pixel_size() const {
	return pixel_size;
}

void TextureProgressBar3D::set_offset(const Point2 &p_offset) {
	if (lbl_offset != p_offset) {
		lbl_offset = p_offset;
		_queue_update();
	}
}

Point2 TextureProgressBar3D::get_offset() const {
	return lbl_offset;
}

void TextureProgressBar3D::set_draw_flag(DrawFlags p_flag, bool p_enable) {
	ERR_FAIL_INDEX(p_flag, FLAG_MAX);
	if (flags[p_flag] != p_enable) {
		flags[p_flag] = p_enable;
		_queue_update();
	}
}

bool TextureProgressBar3D::get_draw_flag(DrawFlags p_flag) const {
	ERR_FAIL_INDEX_V(p_flag, FLAG_MAX, false);
	return flags[p_flag];
}

void TextureProgressBar3D::set_billboard_mode(StandardMaterial3D::BillboardMode p_mode) {
	ERR_FAIL_INDEX(p_mode, 3);
	if (billboard_mode != p_mode) {
		billboard_mode = p_mode;
		_queue_update();
	}
}

StandardMaterial3D::BillboardMode TextureProgressBar3D::get_billboard_mode() const {
	return billboard_mode;
}

void TextureProgressBar3D::set_alpha_cut_mode(AlphaCutMode p_mode) {
	ERR_FAIL_INDEX(p_mode, ALPHA_CUT_MAX);
	if (alpha_cut != p_mode) {
		alpha_cut = p_mode;
		_queue_update();
		notify_property_list_changed();
	}
}

void TextureProgressBar3D::set_texture_filter(StandardMaterial3D::TextureFilter p_filter) {
	if (texture_filter != p_filter) {
		texture_filter = p_filter;
		_queue_update();
	}
}

StandardMaterial3D::TextureFilter TextureProgressBar3D::get_texture_filter() const {
	return texture_filter;
}

TextureProgressBar3D::AlphaCutMode TextureProgressBar3D::get_alpha_cut_mode() const {
	return alpha_cut;
}

void TextureProgressBar3D::set_alpha_hash_scale(float p_hash_scale) {
	if (alpha_hash_scale != p_hash_scale) {
		alpha_hash_scale = p_hash_scale;
		_queue_update();
	}
}

float TextureProgressBar3D::get_alpha_hash_scale() const {
	return alpha_hash_scale;
}

void TextureProgressBar3D::set_alpha_scissor_threshold(float p_threshold) {
	if (alpha_scissor_threshold != p_threshold) {
		alpha_scissor_threshold = p_threshold;
		_queue_update();
	}
}

float TextureProgressBar3D::get_alpha_scissor_threshold() const {
	return alpha_scissor_threshold;
}

void TextureProgressBar3D::set_alpha_antialiasing(BaseMaterial3D::AlphaAntiAliasing p_alpha_aa) {
	if (alpha_antialiasing_mode != p_alpha_aa) {
		alpha_antialiasing_mode = p_alpha_aa;
		_queue_update();
	}
}

BaseMaterial3D::AlphaAntiAliasing TextureProgressBar3D::get_alpha_antialiasing() const {
	return alpha_antialiasing_mode;
}

void TextureProgressBar3D::set_alpha_antialiasing_edge(float p_edge) {
	if (alpha_antialiasing_edge != p_edge) {
		alpha_antialiasing_edge = p_edge;
		_queue_update();
	}
}

float TextureProgressBar3D::get_alpha_antialiasing_edge() const {
	return alpha_antialiasing_edge;
}

TextureProgressBar3D::TextureProgressBar3D() {
	for (int i = 0; i < FLAG_MAX; i++) {
		flags[i] = (i == FLAG_DOUBLE_SIDED);
	}

	mesh = RenderingServer::get_singleton()->mesh_create();

	// Disable shadow casting by default to improve performance and avoid unintended visual artifacts.
	set_cast_shadows_setting(SHADOW_CASTING_SETTING_OFF);

	// TextureProgressBar3D can't contribute to GI in any way, so disable it to improve performance.
	set_gi_mode(GI_MODE_DISABLED);

	set_base(mesh);
}

TextureProgressBar3D::~TextureProgressBar3D() {
	for (int i = 0; i < lines_rid.size(); i++) {
		TS->free_rid(lines_rid[i]);
	}
	lines_rid.clear();

	TS->free_rid(text_rid);

	ERR_FAIL_NULL(RenderingServer::get_singleton());
	RenderingServer::get_singleton()->free(mesh);
	for (KeyValue<SurfaceKey, SurfaceData> E : surfaces) {
		RenderingServer::get_singleton()->free(E.value.material);
	}
	surfaces.clear();
}
