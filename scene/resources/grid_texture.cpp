/**************************************************************************/
/*  grid_texture.cpp                                                      */
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

#include "grid_texture.h"

#include "core/math/geometry_2d.h"

GridTexture::GridTexture() {
	_queue_update();
}

GridTexture::~GridTexture() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
	}
}

void GridTexture::set_checker_color_1(Color p_color) {
	if (p_color == checker_color_1) {
		return;
	}
	checker_color_1 = p_color;
	_queue_update();
	emit_changed();
}

Color GridTexture::get_checker_color_1() const {
	return checker_color_1;
}

void GridTexture::set_checker_color_2(Color p_color) {
	if (p_color == checker_color_2) {
		return;
	}
	checker_color_2 = p_color;
	_queue_update();
	emit_changed();
}

Color GridTexture::get_checker_color_2() const {
	return checker_color_2;
}

void GridTexture::set_grid_color(Color p_color) {
	if (p_color == grid_color) {
		return;
	}
	grid_color = p_color;
	_queue_update();
	emit_changed();
}

Color GridTexture::get_grid_color() const {
	return grid_color;
}

void GridTexture::set_grid_thickness(int p_thickness) {
	if (p_thickness == grid_thickness) {
		return;
	}
	grid_thickness = p_thickness;
	_queue_update();
	emit_changed();
}

int GridTexture::get_grid_thickness() const {
	return grid_thickness;
}

void GridTexture::set_grid_primary_thickness(int p_thickness) {
	if (p_thickness == grid_primary_thickness) {
		return;
	}
	grid_primary_thickness = p_thickness;
	_queue_update();
	emit_changed();
}

int GridTexture::get_grid_primary_thickness() const {
	return grid_primary_thickness;
}

void GridTexture::set_grid_steps(Vector2i p_steps) {
	if (p_steps == grid_steps) {
		return;
	}
	grid_steps = p_steps;
	_queue_update();
	emit_changed();
}

Vector2i GridTexture::get_grid_steps() const {
	return grid_steps;
}

void GridTexture::set_grid_primary_steps(int p_steps) {
	if (p_steps == grid_primary_steps) {
		return;
	}
	grid_primary_steps = p_steps;
	_queue_update();
	emit_changed();
}

int GridTexture::get_grid_primary_steps() const {
	return grid_primary_steps;
}

void GridTexture::set_generate_mipmaps(bool p_enable) {
	if (p_enable == generate_mipmaps) {
		return;
	}
	generate_mipmaps = p_enable;
	_queue_update();
	emit_changed();
}

bool GridTexture::is_generating_mipmaps() const {
	return generate_mipmaps;
}

void GridTexture::set_as_normal_map(bool p_as_normal_map) {
	if (p_as_normal_map == as_normal_map) {
		return;
	}
	as_normal_map = p_as_normal_map;
	_queue_update();
	emit_changed();
	notify_property_list_changed();
}

bool GridTexture::is_normal_map() const {
	return as_normal_map;
}

void GridTexture::set_bump_strength(float p_bump_strength) {
	if (p_bump_strength == bump_strength) {
		return;
	}
	bump_strength = p_bump_strength;
	_queue_update();
	emit_changed();
}

float GridTexture::get_bump_strength() const {
	return bump_strength;
}

void GridTexture::_queue_update() {
	if (update_pending) {
		return;
	}
	update_pending = true;
	callable_mp(this, &GridTexture::update_now).call_deferred();
}

void GridTexture::_update() {
	update_pending = false;

	Ref<Image> image;
	image.instantiate();

	Vector<uint8_t> data;
	data.resize(width * height * 4);
	{
		uint8_t *wd8 = data.ptrw();
		int grid_subdiv = 0;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				// TODO: Implement grid line thickness and primary thickness.
				bool grid_line = false;
				bool grid_primary_line = false;
				if (grid_steps.x >= width) {
					grid_line = true;
				} else if (grid_steps.x >= 1) {
					if (x % (width / grid_steps.x) == 0) {
						grid_line = true;
						grid_subdiv += 1;
					}
				}

				if (grid_steps.y >= height) {
					grid_line = true;
				} else if (grid_steps.y >= 1) {
					if (y % (height / grid_steps.y) == 0) {
						grid_line = true;
						grid_subdiv += 1;
					}
				}

				if (grid_subdiv % grid_primary_steps == 0) {
					// FIXME: Fix grid primary line detection.
					grid_primary_line = true;
				}

				// TODO: Implement checker color.
				Color checker_color = checker_color_1;
				// if (Math::pingpong(float(x + y), float(grid_steps.x)) > (grid_steps.x * 0.5)) {
				// 	checker_color = checker_color_2;
				// }

				const Color &color = grid_line ? grid_color : checker_color;
				wd8[(x + (y * width)) * 4 + 0] = uint8_t(CLAMP(color.r * 255.0, 0, 255));
				wd8[(x + (y * width)) * 4 + 1] = uint8_t(CLAMP(color.g * 255.0, 0, 255));
				wd8[(x + (y * width)) * 4 + 2] = uint8_t(CLAMP(color.b * 255.0, 0, 255));
				wd8[(x + (y * width)) * 4 + 3] = uint8_t(CLAMP(color.a * 255.0, 0, 255));
			}
		}
	}
	image->set_data(width, height, false, Image::FORMAT_RGBA8, data);
	if (as_normal_map) {
		image->bump_map_to_normal_map(bump_strength);
	}
	if (generate_mipmaps) {
		image->generate_mipmaps();
	}

	if (texture.is_valid()) {
		RID new_texture = RS::get_singleton()->texture_2d_create(image);
		RS::get_singleton()->texture_replace(texture, new_texture);
	} else {
		texture = RS::get_singleton()->texture_2d_create(image);
	}
}

void GridTexture::set_width(int p_width) {
	ERR_FAIL_COND_MSG(p_width <= 0 || p_width > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	width = p_width;
	_queue_update();
	emit_changed();
}

int GridTexture::get_width() const {
	return width;
}

void GridTexture::set_height(int p_height) {
	ERR_FAIL_COND_MSG(p_height <= 0 || p_height > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	height = p_height;
	_queue_update();
	emit_changed();
}

int GridTexture::get_height() const {
	return height;
}

RID GridTexture::get_rid() const {
	if (!texture.is_valid()) {
		texture = RS::get_singleton()->texture_2d_placeholder_create();
	}
	return texture;
}

Ref<Image> GridTexture::get_image() const {
	const_cast<GridTexture *>(this)->update_now();
	if (!texture.is_valid()) {
		return Ref<Image>();
	}
	return RenderingServer::get_singleton()->texture_2d_get(texture);
}

void GridTexture::update_now() {
	if (update_pending) {
		_update();
	}
}

void GridTexture::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == "bump_strength") {
		if (!as_normal_map) {
			p_property.usage = PROPERTY_USAGE_NO_EDITOR;
		}
	}
}

void GridTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_checker_color_1", "color"), &GridTexture::set_checker_color_1);
	ClassDB::bind_method(D_METHOD("get_checker_color_1"), &GridTexture::get_checker_color_1);
	ClassDB::bind_method(D_METHOD("set_checker_color_2", "color"), &GridTexture::set_checker_color_2);
	ClassDB::bind_method(D_METHOD("get_checker_color_2"), &GridTexture::get_checker_color_2);
	ClassDB::bind_method(D_METHOD("set_grid_color", "color"), &GridTexture::set_grid_color);
	ClassDB::bind_method(D_METHOD("get_grid_color"), &GridTexture::get_grid_color);
	ClassDB::bind_method(D_METHOD("set_grid_thickness", "thickness"), &GridTexture::set_grid_thickness);
	ClassDB::bind_method(D_METHOD("get_grid_thickness"), &GridTexture::get_grid_thickness);
	ClassDB::bind_method(D_METHOD("set_grid_primary_thickness", "thickness"), &GridTexture::set_grid_primary_thickness);
	ClassDB::bind_method(D_METHOD("get_grid_primary_thickness"), &GridTexture::get_grid_primary_thickness);
	ClassDB::bind_method(D_METHOD("set_grid_steps", "steps"), &GridTexture::set_grid_steps);
	ClassDB::bind_method(D_METHOD("get_grid_steps"), &GridTexture::get_grid_steps);
	ClassDB::bind_method(D_METHOD("set_grid_primary_steps", "steps"), &GridTexture::set_grid_primary_steps);
	ClassDB::bind_method(D_METHOD("get_grid_primary_steps"), &GridTexture::get_grid_primary_steps);

	ClassDB::bind_method(D_METHOD("set_generate_mipmaps", "generate_mipmaps"), &GridTexture::set_generate_mipmaps);
	ClassDB::bind_method(D_METHOD("is_generating_mipmaps"), &GridTexture::is_generating_mipmaps);
	ClassDB::bind_method(D_METHOD("set_as_normal_map", "as_normal_map"), &GridTexture::set_as_normal_map);
	ClassDB::bind_method(D_METHOD("is_normal_map"), &GridTexture::is_normal_map);
	ClassDB::bind_method(D_METHOD("set_bump_strength", "bump_strength"), &GridTexture::set_bump_strength);
	ClassDB::bind_method(D_METHOD("get_bump_strength"), &GridTexture::get_bump_strength);

	ClassDB::bind_method(D_METHOD("set_width", "width"), &GridTexture::set_width);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &GridTexture::set_height);

	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "checker_color_1"), "set_checker_color_1", "get_checker_color_1");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "checker_color_2"), "set_checker_color_2", "get_checker_color_2");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "grid_color"), "set_grid_color", "get_grid_color");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_thickness", PROPERTY_HINT_RANGE, "0,512"), "set_grid_thickness", "get_grid_thickness");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "grid_steps", PROPERTY_HINT_RANGE, "0,1024"), "set_grid_steps", "get_grid_steps");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_primary_steps", PROPERTY_HINT_RANGE, "0,512"), "set_grid_primary_steps", "get_grid_primary_steps");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_primary_thickness", PROPERTY_HINT_RANGE, "0,512"), "set_grid_primary_thickness", "get_grid_primary_thickness");

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "generate_mipmaps"), "set_generate_mipmaps", "is_generating_mipmaps");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_normal_map"), "set_as_normal_map", "is_normal_map");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bump_strength", PROPERTY_HINT_RANGE, "-32,32,0.1,or_less,or_greater"), "set_bump_strength", "get_bump_strength");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,2048,or_greater,suffix:px"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,2048,or_greater,suffix:px"), "set_height", "get_height");
}
