/*************************************************************************/
/*  noise_texture_3d.h                                                   */
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

#ifndef NOISE_TEXTURE_3D_H
#define NOISE_TEXTURE_3D_H

#include "open_simplex_noise.h"

#include "core/io/image.h"
#include "core/object/ref_counted.h"
#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "editor/property_editor.h"

class NoiseTexture3D : public Texture3D {
	GDCLASS(NoiseTexture3D, Texture3D);

private:
	Vector<Ref<Image>> data;

	Thread noise_thread;

	bool first_time = true;
	bool update_queued = false;
	bool regen_queued = false;

	mutable RID texture;
	uint32_t flags = 0;

	Ref<OpenSimplexNoise> noise;
	Vector3i size = Vector3i(64, 64, 64);
	Vector3 noise_offset;
	bool seamless = false;

	void _thread_done(const Vector<Ref<Image>> &p_data);
	static void _thread_function(void *p_ud);

	void _queue_update();
	Vector<Ref<Image>> _generate_texture();
	void _update_texture();
	void _set_texture_image(const Vector<Ref<Image>> &p_data);

protected:
	static void _bind_methods();

public:
	void set_noise(Ref<OpenSimplexNoise> p_noise);
	Ref<OpenSimplexNoise> get_noise();

	void set_width(int p_width);
	void set_height(int p_height);
	void set_depth(int p_depth);

	void set_noise_offset(Vector3 p_noise_offset);
	Vector3 get_noise_offset() const;

	void set_seamless(bool p_seamless);
	bool get_seamless();

	int get_width() const override;
	int get_height() const override;
	int get_depth() const override;

	virtual RID get_rid() const override;
	virtual bool has_mipmaps() const override { return false; }
	virtual Image::Format get_format() const override { return Image::FORMAT_L8; }

	virtual Vector<Ref<Image>> get_data() const override;

	NoiseTexture3D();
	virtual ~NoiseTexture3D();
};

#endif // NOISE_TEXTURE_3D_H
