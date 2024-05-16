/**************************************************************************/
/*  grid_texture.h                                                        */
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

#ifndef GRID_TEXTURE_H
#define GRID_TEXTURE_H

#include "scene/resources/texture.h"

class GridTexture : public Texture2D {
	GDCLASS(GridTexture, Texture2D);

private:
	Color checker_color_1 = Color(0.4, 0.4, 0.4);
	Color checker_color_2 = Color(0.6, 0.6, 0.6);
	Color grid_color = Color(0.8, 0.8, 0.8);
	int grid_thickness = 1;
	int grid_primary_thickness = 4;
	Vector2i grid_steps = Vector2i(16, 16);
	int grid_primary_steps = 8;

	bool generate_mipmaps = true;
	bool as_normal_map = false;
	float bump_strength = 8.0f;

	mutable RID texture;

	int width = 1024;
	int height = 1024;

	bool update_pending = false;
	void _queue_update();
	void _update();

protected:
	static void _bind_methods();
	void _validate_property(PropertyInfo &p_property) const;

public:
	void set_checker_color_1(Color p_color);
	Color get_checker_color_1() const;

	void set_checker_color_2(Color p_color);
	Color get_checker_color_2() const;

	void set_grid_color(Color p_color);
	Color get_grid_color() const;

	void set_grid_thickness(int p_thickness);
	int get_grid_thickness() const;

	void set_grid_primary_thickness(int p_thickness);
	int get_grid_primary_thickness() const;

	void set_grid_steps(Vector2i p_steps);
	Vector2i get_grid_steps() const;

	void set_grid_primary_steps(int p_steps);
	int get_grid_primary_steps() const;

	void set_generate_mipmaps(bool p_enable);
	bool is_generating_mipmaps() const;

	void set_as_normal_map(bool p_as_normal_map);
	bool is_normal_map() const;

	void set_bump_strength(float p_bump_strength);
	float get_bump_strength() const;

	void set_width(int p_width);
	virtual int get_width() const override;
	void set_height(int p_height);
	virtual int get_height() const override;

	virtual RID get_rid() const override;
	virtual bool has_alpha() const override { return true; }
	virtual Ref<Image> get_image() const override;
	void update_now();

	GridTexture();
	virtual ~GridTexture();
};

#endif // GRID_TEXTURE_H
