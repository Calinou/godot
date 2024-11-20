/**************************************************************************/
/*  composite_texture.h                                                   */
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

#ifndef COMPOSITE_TEXTURE_H
#define COMPOSITE_TEXTURE_H

#include "scene/resources/texture.h"

class CompositeTexture2D : public Texture2D {
	GDCLASS(CompositeTexture2D, Texture2D);

public:
	enum BlendMode {
		BLEND_MODE_MIX,
		BLEND_MODE_ADD,
		BLEND_MODE_SUB,
		BLEND_MODE_MUL,
		BLEND_MODE_PREMUL_ALPHA,
	};

	enum GenerateMipmapsMode {
		GENERATE_MIPMAPS_MODE_AUTOMATIC,
		GENERATE_MIPMAPS_MODE_NEVER,
		GENERATE_MIPMAPS_MODE_ALWAYS,
	};

private:
	Ref<Texture2D> texture_overlay;
	Ref<Texture2D> texture_base;

	mutable RID texture;

	int width = 1;
	int height = 1;

	BlendMode blend_mode = BLEND_MODE_MIX;
	float blend_factor = 0.5f;

	GenerateMipmapsMode generate_mipmaps = GENERATE_MIPMAPS_MODE_AUTOMATIC;

	Color _blend_pixel(const Color &p_base, const Color &p_overlay) const;

	mutable bool update_pending = false;
	void _update_size();
	void _queue_update();
	void _update() const;

protected:
	static void _bind_methods();

public:
	void set_texture_overlay(const Ref<Texture2D> &p_texture);
	Ref<Texture2D> get_texture_overlay() const;

	void set_texture_base(const Ref<Texture2D> &p_texture);
	Ref<Texture2D> get_texture_base() const;

	void set_blend_mode(BlendMode p_mode);
	BlendMode get_blend_mode() const;

	void set_blend_factor(float p_factor);
	float get_blend_factor() const;

	void set_generate_mipmaps(GenerateMipmapsMode p_mode);
	GenerateMipmapsMode get_generate_mipmaps() const;

	virtual int get_width() const override;
	virtual int get_height() const override;

	virtual RID get_rid() const override;
	virtual bool has_alpha() const override { return true; }
	virtual Ref<Image> get_image() const override;
	void update_now() const;

	CompositeTexture2D();
	virtual ~CompositeTexture2D();
};

VARIANT_ENUM_CAST(CompositeTexture2D::BlendMode);
VARIANT_ENUM_CAST(CompositeTexture2D::GenerateMipmapsMode);

#endif // COMPOSITE_TEXTURE_H
