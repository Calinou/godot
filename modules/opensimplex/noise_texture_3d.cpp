/*************************************************************************/
/*  noise_texture_3d.cpp                                                 */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "noise_texture_3d.h"

#include "core/core_string_names.h"

NoiseTexture3D::NoiseTexture3D() {
	noise = Ref<OpenSimplexNoise>();

	_queue_update();
}

NoiseTexture3D::~NoiseTexture3D() {
	if (texture.is_valid()) {
		RS::get_singleton()->free(texture);
	}
	noise_thread.wait_to_finish();
}

void NoiseTexture3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_width", "width"), &NoiseTexture3D::set_width);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &NoiseTexture3D::set_height);

	ClassDB::bind_method(D_METHOD("set_noise", "noise"), &NoiseTexture3D::set_noise);
	ClassDB::bind_method(D_METHOD("get_noise"), &NoiseTexture3D::get_noise);

	ClassDB::bind_method(D_METHOD("set_noise_offset", "noise_offset"), &NoiseTexture3D::set_noise_offset);
	ClassDB::bind_method(D_METHOD("get_noise_offset"), &NoiseTexture3D::get_noise_offset);

	ClassDB::bind_method(D_METHOD("set_seamless", "seamless"), &NoiseTexture3D::set_seamless);
	ClassDB::bind_method(D_METHOD("get_seamless"), &NoiseTexture3D::get_seamless);

	ClassDB::bind_method(D_METHOD("_update_texture"), &NoiseTexture3D::_update_texture);
	ClassDB::bind_method(D_METHOD("_generate_texture"), &NoiseTexture3D::_generate_texture);
	ClassDB::bind_method(D_METHOD("_thread_done", "image"), &NoiseTexture3D::_thread_done);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,512,1,or_greater"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,512,1,or_greater"), "set_height", "get_height");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "depth", PROPERTY_HINT_RANGE, "1,512,1,or_greater"), "set_depth", "get_depth");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "seamless"), "set_seamless", "get_seamless");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise", PROPERTY_HINT_RESOURCE_TYPE, "OpenSimplexNoise"), "set_noise", "get_noise");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "noise_offset"), "set_noise_offset", "get_noise_offset");
}

void NoiseTexture3D::_set_texture_image(const Vector<Ref<Image>> &p_data) {
	data = p_data;
	if (data.size() >= 1) {
		if (texture.is_valid()) {
			RID new_texture = RS::get_singleton()->texture_3d_create(Image::FORMAT_L8, size.x, size.y, size.z, false, p_data);
			RS::get_singleton()->texture_replace(texture, new_texture);
		} else {
			texture = RS::get_singleton()->texture_3d_create(Image::FORMAT_L8, size.x, size.y, size.z, false, p_data);
		}
	}
	emit_changed();
}

void NoiseTexture3D::_thread_done(const Vector<Ref<Image>> &p_data) {
	_set_texture_image(p_data);
	noise_thread.wait_to_finish();
	if (regen_queued) {
		noise_thread.start(_thread_function, this);
		regen_queued = false;
	}
}

void NoiseTexture3D::_thread_function(void *p_ud) {
	NoiseTexture3D *tex = (NoiseTexture3D *)p_ud;
	// FIXME: This doesn't compile because we can't use `Vector<Ref<Image>>` as a parameter type in `call_deferred()`.
	tex->call_deferred(SNAME("_thread_done"), tex->_generate_texture());
}

void NoiseTexture3D::_queue_update() {
	if (update_queued) {
		return;
	}

	update_queued = true;
	call_deferred(SNAME("_update_texture"));
}

Vector<Ref<Image>> NoiseTexture3D::_generate_texture() {
	// Prevent memdelete due to unref() on other thread.
	Ref<OpenSimplexNoise> ref_noise = noise;

	if (ref_noise.is_null()) {
		return Vector<Ref<Image>>();
	}

	Vector<Ref<Image>> images;

	if (seamless) {
		images = ref_noise->get_seamless_image_3d(size.x);
	} else {
		images = ref_noise->get_image_3d(size.x, size.y, size.z, noise_offset);
	}

	return images;
}

void NoiseTexture3D::_update_texture() {
	bool use_thread = true;
	if (first_time) {
		use_thread = false;
		first_time = false;
	}
#ifdef NO_THREADS
	use_thread = false;
#endif
	if (use_thread) {
		if (!noise_thread.is_started()) {
			noise_thread.start(_thread_function, this);
			regen_queued = false;
		} else {
			regen_queued = true;
		}

	} else {
		Ref<Image> image = _generate_texture();
		_set_texture_image(image);
	}
	update_queued = false;
}

void NoiseTexture3D::set_noise(Ref<OpenSimplexNoise> p_noise) {
	if (p_noise == noise) {
		return;
	}
	if (noise.is_valid()) {
		noise->disconnect(CoreStringNames::get_singleton()->changed, callable_mp(this, &NoiseTexture3D::_queue_update));
	}
	noise = p_noise;
	if (noise.is_valid()) {
		noise->connect(CoreStringNames::get_singleton()->changed, callable_mp(this, &NoiseTexture3D::_queue_update));
	}
	_queue_update();
}

Ref<OpenSimplexNoise> NoiseTexture3D::get_noise() {
	return noise;
}

void NoiseTexture3D::set_width(int p_width) {
	ERR_FAIL_COND(p_width <= 0);
	if (p_width == size.x) {
		return;
	}
	size.x = p_width;
	_queue_update();
}

void NoiseTexture3D::set_height(int p_height) {
	ERR_FAIL_COND(p_height <= 0);
	if (p_height == size.y) {
		return;
	}
	size.y = p_height;
	_queue_update();
}

void NoiseTexture3D::set_depth(int p_depth) {
	ERR_FAIL_COND(p_depth <= 0);
	if (p_depth == size.z) {
		return;
	}
	size.z = p_depth;
	_queue_update();
}

void NoiseTexture3D::set_noise_offset(Vector3 p_noise_offset) {
	if (noise_offset == p_noise_offset) {
		return;
	}
	noise_offset = p_noise_offset;
	_queue_update();
}

void NoiseTexture3D::set_seamless(bool p_seamless) {
	if (p_seamless == seamless) {
		return;
	}
	seamless = p_seamless;
	_queue_update();
}

bool NoiseTexture3D::get_seamless() {
	return seamless;
}

int NoiseTexture3D::get_width() const {
	return size.x;
}

int NoiseTexture3D::get_height() const {
	return size.y;
}

int NoiseTexture3D::get_depth() const {
	return size.z;
}

Vector3 NoiseTexture3D::get_noise_offset() const {
	return noise_offset;
}

RID NoiseTexture3D::get_rid() const {
	if (!texture.is_valid()) {
		texture = RS::get_singleton()->texture_3d_placeholder_create();
	}

	return texture;
}

Vector<Ref<Image>> NoiseTexture3D::get_data() const {
	return data;
}
