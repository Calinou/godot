/**************************************************************************/
/*  composite_texture.cpp                                                 */
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

#include "composite_texture.h"

CompositeTexture2D::CompositeTexture2D() {
	_queue_update();
}

CompositeTexture2D::~CompositeTexture2D() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
	}
}

void CompositeTexture2D::set_texture_overlay(const Ref<Texture2D> &p_texture_overlay) {
	if (texture_overlay == p_texture_overlay) {
		return;
	}

	if (texture_overlay.is_valid()) {
		texture_overlay->disconnect_changed(callable_mp(this, &CompositeTexture2D::_queue_update));
	}

	texture_overlay = p_texture_overlay;

	if (texture_overlay.is_valid()) {
		texture_overlay->connect_changed(callable_mp(this, &CompositeTexture2D::_queue_update));
	}

	_update_size();

	_queue_update();
	emit_changed();
}

Ref<Texture2D> CompositeTexture2D::get_texture_overlay() const {
	return texture_overlay;
}

void CompositeTexture2D::set_texture_base(const Ref<Texture2D> &p_texture_base) {
	if (texture_base == p_texture_base) {
		return;
	}

	if (texture_base.is_valid()) {
		texture_base->disconnect_changed(callable_mp(this, &CompositeTexture2D::_queue_update));
	}

	texture_base = p_texture_base;

	if (texture_base.is_valid()) {
		texture_base->connect_changed(callable_mp(this, &CompositeTexture2D::_queue_update));
	}

	_update_size();

	_queue_update();
	emit_changed();
}

void CompositeTexture2D::_update_size() {
	if (texture_overlay.is_valid() && texture_base.is_valid()) {
		width = MAX(texture_overlay->get_size().width, texture_base->get_size().width);
		height = MAX(texture_overlay->get_size().height, texture_base->get_size().height);
	} else if (texture_overlay.is_valid()) {
		width = texture_overlay->get_size().width;
		height = texture_overlay->get_size().height;
	} else if (texture_base.is_valid()) {
		width = texture_base->get_size().width;
		height = texture_base->get_size().height;
	} else {
		width = 1;
		height = 1;
	}
}

Ref<Texture2D> CompositeTexture2D::get_texture_base() const {
	return texture_base;
}

void CompositeTexture2D::set_blend_mode(BlendMode p_blend_mode) {
	if (blend_mode == p_blend_mode) {
		return;
	}

	blend_mode = p_blend_mode;

	_queue_update();
	emit_changed();
}

CompositeTexture2D::BlendMode CompositeTexture2D::get_blend_mode() const {
	return blend_mode;
}

void CompositeTexture2D::set_blend_factor(float p_factor) {
	if (blend_factor == p_factor) {
		return;
	}

	blend_factor = CLAMP(p_factor, 0.0, 1.0);

	_queue_update();
	emit_changed();
}

float CompositeTexture2D::get_blend_factor() const {
	return blend_factor;
}

void CompositeTexture2D::set_generate_mipmaps(GenerateMipmapsMode p_mode) {
	if (generate_mipmaps == p_mode) {
		return;
	}

	generate_mipmaps = p_mode;

	_queue_update();
	emit_changed();
}

CompositeTexture2D::GenerateMipmapsMode CompositeTexture2D::get_generate_mipmaps() const {
	return generate_mipmaps;
}

void CompositeTexture2D::_queue_update() {
	if (update_pending) {
		return;
	}
	update_pending = true;
	callable_mp(this, &CompositeTexture2D::update_now).call_deferred();
}

void CompositeTexture2D::_update() const {
	update_pending = false;

	if (texture_base.is_null() && texture_overlay.is_null()) {
		return;
	}

	// Guard against situations where a texture's is updated late, such as with GradientTexture.
	if ((texture_base.is_valid() && texture_base->get_image().is_null()) || (texture_overlay.is_valid() && texture_overlay->get_image().is_null())) {
		return;
	}

	Ref<Image> image;
	image.instantiate();

	if (texture_base.is_valid() && texture_overlay.is_null()) {
		// No need to blend two images together.
		image = texture_base->get_image();
	} else if (texture_overlay.is_valid() && texture_base.is_null()) {
		// No need to blend two images together.
		image = texture_overlay->get_image();
	} else {
		const Ref<Image> image_overlay = texture_overlay->get_image();
		const Ref<Image> image_base = texture_base->get_image();
		if (image_overlay->is_compressed()) {
			image_overlay->decompress();
		}
		if (image_base->is_compressed()) {
			image_base->decompress();
		}

		image->copy_from(texture_base);

		if (true) {
			image->copy_from(image_base);
			if (generate_mipmaps == GENERATE_MIPMAPS_MODE_NEVER) {
				image->clear_mipmaps();
			}
			// Resize images to match the largest image of the two.
			// FIXME: This doesn't work in nested CompositeTexture scenarios if there are textures of different sizes.
			if (image_overlay->get_size().width < width || image_overlay->get_size().height < height) {
				image_overlay->resize(width, height);
			} else if (image_base->get_size().width < width || image_base->get_size().height < height) {
				image_base->resize(width, height);
			}
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					image->set_pixel(x, y, _blend_pixel(image->get_pixel(x, y), image_overlay->get_pixel(x, y)));
				}
			}

			if (generate_mipmaps == GENERATE_MIPMAPS_MODE_ALWAYS || (generate_mipmaps == GENERATE_MIPMAPS_MODE_AUTOMATIC && (image_base->has_mipmaps() || image_overlay->has_mipmaps()))) {
				image->generate_mipmaps();
			}
		} else {
			// 	// Use an optimized codepath.
			// 	Vector<uint8_t> data;
			// 	data.resize(width * height * 4);
			// 	{
			// 		uint8_t *wd8 = data.ptrw();
			// 		Gradient &g = **gradient;
			// 		for (int y = 0; y < height; y++) {
			// 			for (int x = 0; x < width; x++) {
			// 				float ofs = _get_blend_mode_offset_at(x, y);
			// 				const Color &c = g.get_color_at_offset(ofs);

			// 				wd8[(x + (y * width)) * 4 + 0] = uint8_t(CLAMP(c.r * 255.0, 0, 255));
			// 				wd8[(x + (y * width)) * 4 + 1] = uint8_t(CLAMP(c.g * 255.0, 0, 255));
			// 				wd8[(x + (y * width)) * 4 + 2] = uint8_t(CLAMP(c.b * 255.0, 0, 255));
			// 				wd8[(x + (y * width)) * 4 + 3] = uint8_t(CLAMP(c.a * 255.0, 0, 255));
			// 			}
			// 		}
			// 	}
			// 	image->set_data(width, height, false, Image::FORMAT_RGBA8, data);
			// }
		}
	}

	if (texture.is_valid()) {
		RID new_texture = RS::get_singleton()->texture_2d_create(image);
		RS::get_singleton()->texture_replace(texture, new_texture);
	} else {
		texture = RS::get_singleton()->texture_2d_create(image);
	}
	RS::get_singleton()->texture_set_path(texture, get_path());
}

Color CompositeTexture2D::_blend_pixel(const Color &p_base, const Color &p_overlay) const {
	switch (blend_mode) {
		case BLEND_MODE_MIX:
			return p_base.lerp(p_overlay, blend_factor);
		case BLEND_MODE_ADD:
			return p_base + p_overlay * blend_factor;
		case BLEND_MODE_SUB:
			return p_base - p_overlay * blend_factor;
		case BLEND_MODE_MUL:
			return p_base.lerp(p_base * p_overlay, blend_factor);
		case BLEND_MODE_PREMUL_ALPHA:
			return Color(p_base + p_overlay * blend_factor).lerp(p_base.lerp(p_overlay, blend_factor), p_overlay.a);
	}
}

int CompositeTexture2D::get_width() const {
	return width;
}

int CompositeTexture2D::get_height() const {
	return height;
}

RID CompositeTexture2D::get_rid() const {
	if (!texture.is_valid()) {
		texture = RS::get_singleton()->texture_2d_placeholder_create();
	}
	return texture;
}

Ref<Image> CompositeTexture2D::get_image() const {
	update_now();
	if (!texture.is_valid()) {
		return Ref<Image>();
	}
	return RenderingServer::get_singleton()->texture_2d_get(texture);
}

void CompositeTexture2D::update_now() const {
	if (update_pending) {
		_update();
	}
}

void CompositeTexture2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_texture_overlay", "texture"), &CompositeTexture2D::set_texture_overlay);
	ClassDB::bind_method(D_METHOD("get_texture_overlay"), &CompositeTexture2D::get_texture_overlay);

	ClassDB::bind_method(D_METHOD("set_texture_base", "texture"), &CompositeTexture2D::set_texture_base);
	ClassDB::bind_method(D_METHOD("get_texture_base"), &CompositeTexture2D::get_texture_base);

	ClassDB::bind_method(D_METHOD("set_blend_mode", "blend_mode"), &CompositeTexture2D::set_blend_mode);
	ClassDB::bind_method(D_METHOD("get_blend_mode"), &CompositeTexture2D::get_blend_mode);

	ClassDB::bind_method(D_METHOD("set_blend_factor", "blend_factor"), &CompositeTexture2D::set_blend_factor);
	ClassDB::bind_method(D_METHOD("get_blend_factor"), &CompositeTexture2D::get_blend_factor);

	ClassDB::bind_method(D_METHOD("set_generate_mipmaps", "mode"), &CompositeTexture2D::set_generate_mipmaps);
	ClassDB::bind_method(D_METHOD("get_generate_mipmaps"), &CompositeTexture2D::get_generate_mipmaps);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_overlay", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture_overlay", "get_texture_overlay");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_base", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture_base", "get_texture_base");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "blend_mode", PROPERTY_HINT_ENUM, "Mix,Add,Subtract,Multiply,Premultiplied Alpha"), "set_blend_mode", "get_blend_mode");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "blend_factor", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_blend_factor", "get_blend_factor");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "generate_mipmaps", PROPERTY_HINT_ENUM, "Automatic,Never,Always"), "set_generate_mipmaps", "get_generate_mipmaps");

	BIND_ENUM_CONSTANT(BLEND_MODE_MIX);
	BIND_ENUM_CONSTANT(BLEND_MODE_ADD);
	BIND_ENUM_CONSTANT(BLEND_MODE_SUB);
	BIND_ENUM_CONSTANT(BLEND_MODE_MUL);
	BIND_ENUM_CONSTANT(BLEND_MODE_PREMUL_ALPHA);

	BIND_ENUM_CONSTANT(GENERATE_MIPMAPS_MODE_AUTOMATIC);
	BIND_ENUM_CONSTANT(GENERATE_MIPMAPS_MODE_NEVER);
	BIND_ENUM_CONSTANT(GENERATE_MIPMAPS_MODE_ALWAYS);
}
