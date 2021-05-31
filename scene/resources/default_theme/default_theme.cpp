/*************************************************************************/
/*  default_theme.cpp                                                    */
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

#include "default_theme.h"

#include "core/os/os.h"
#include "default_font.gen.h"
#include "default_theme_icons.gen.h"
#include "modules/svg/image_loader_svg.h"
#include "scene/resources/font.h"
#include "scene/resources/theme.h"
#include "servers/text_server.h"
#include "theme_data.h"

typedef Map<const void *, Ref<ImageTexture>> TexCacheMap;

static TexCacheMap *tex_cache;
static float scale = 1.0;

template <class T>
static Ref<StyleBoxTexture> make_stylebox(T p_src, float p_left, float p_top, float p_right, float p_bottom, float p_margin_left = -1, float p_margin_top = -1, float p_margin_right = -1, float p_margin_bottom = -1, bool p_draw_center = true) {
	Ref<ImageTexture> texture;

	if (tex_cache->has(p_src)) {
		texture = (*tex_cache)[p_src];
	} else {
		texture = Ref<ImageTexture>(memnew(ImageTexture));
		Ref<Image> img = memnew(Image(p_src));
		const Size2 orig_size = Size2(img->get_width(), img->get_height());
		img->convert(Image::FORMAT_RGBA8);
		img->resize(orig_size.x * scale, orig_size.y * scale);

		texture->create_from_image(img);
		(*tex_cache)[p_src] = texture;
	}

	Ref<StyleBoxTexture> style(memnew(StyleBoxTexture));
	style->set_texture(texture);
	style->set_margin_size(SIDE_LEFT, p_left * scale);
	style->set_margin_size(SIDE_RIGHT, p_right * scale);
	style->set_margin_size(SIDE_BOTTOM, p_bottom * scale);
	style->set_margin_size(SIDE_TOP, p_top * scale);
	style->set_default_margin(SIDE_LEFT, p_margin_left * scale);
	style->set_default_margin(SIDE_RIGHT, p_margin_right * scale);
	style->set_default_margin(SIDE_BOTTOM, p_margin_bottom * scale);
	style->set_default_margin(SIDE_TOP, p_margin_top * scale);
	style->set_draw_center(p_draw_center);

	return style;
}

static const int default_margin = 4;
static const int default_corner_radius = 3;

static Ref<StyleBoxFlat> make_flat_stylebox(Color p_color, float p_margin_left = default_margin, float p_margin_top = default_margin, float p_margin_right = default_margin, float p_margin_bottom = default_margin, int p_corner_radius = default_corner_radius, bool p_draw_center = true, int p_border_width = 0) {
	Ref<StyleBoxFlat> style(memnew(StyleBoxFlat));
	style->set_bg_color(p_color);
	style->set_default_margin(SIDE_LEFT, p_margin_left * scale);
	style->set_default_margin(SIDE_RIGHT, p_margin_right * scale);
	style->set_default_margin(SIDE_BOTTOM, p_margin_bottom * scale);
	style->set_default_margin(SIDE_TOP, p_margin_top * scale);

	style->set_corner_radius_all(p_corner_radius);
	// Adjust level of detail based on the corners' effective sizes.
	style->set_corner_detail(Math::ceil(1.5 * p_corner_radius));
	// Work around issue about antialiased edges being blurrier with rounded corners (GH-35279).
	style->set_anti_aliased(false);

	style->set_draw_center(p_draw_center);
	style->set_border_width_all(p_border_width);

	return style;
}

static Ref<StyleBoxTexture> sb_expand(Ref<StyleBoxTexture> p_sbox, float p_left, float p_top, float p_right, float p_bottom) {
	p_sbox->set_expand_margin_size(SIDE_LEFT, p_left * scale);
	p_sbox->set_expand_margin_size(SIDE_TOP, p_top * scale);
	p_sbox->set_expand_margin_size(SIDE_RIGHT, p_right * scale);
	p_sbox->set_expand_margin_size(SIDE_BOTTOM, p_bottom * scale);
	return p_sbox;
}

// See also `editor_generate_icon()` in `editor/editor_themes.cpp`.
static Ref<ImageTexture> generate_icon(int p_index) {
	Ref<ImageTexture> icon = memnew(ImageTexture);
	Ref<Image> img = memnew(Image);

	// Upsample icon generation only if the scale isn't an integer multiplier.
	// Generating upsampled icons is slower, and the benefit is hardly visible
	// with integer scales.
	const bool upsample = !Math::is_equal_approx(Math::round(scale), scale);
	ImageLoaderSVG::create_image_from_string(img, default_theme_icons_sources[p_index], scale, upsample, false);
	icon->create_from_image(img);

	return icon;
}

template <class T>
static Ref<Texture2D> make_icon(T p_src) {
	Ref<ImageTexture> texture(memnew(ImageTexture));
	Ref<Image> img = memnew(Image(p_src));
	const Size2 orig_size = Size2(img->get_width(), img->get_height());
	img->convert(Image::FORMAT_RGBA8);
	img->resize(orig_size.x * scale, orig_size.y * scale);
	texture->create_from_image(img);

	return texture;
}

static Ref<Texture2D> flip_icon(Ref<Texture2D> p_texture, bool p_flip_y = false, bool p_flip_x = false) {
	if (!p_flip_y && !p_flip_x) {
		return p_texture;
	}

	Ref<ImageTexture> texture(memnew(ImageTexture));
	Ref<Image> img = p_texture->get_image();
	img = img->duplicate();

	if (p_flip_y) {
		img->flip_y();
	}
	if (p_flip_x) {
		img->flip_x();
	}

	texture->create_from_image(img);
	return texture;
}

static Ref<StyleBox> make_empty_stylebox(float p_margin_left = -1, float p_margin_top = -1, float p_margin_right = -1, float p_margin_bottom = -1) {
	Ref<StyleBox> style(memnew(StyleBoxEmpty));

	style->set_default_margin(SIDE_LEFT, p_margin_left * scale);
	style->set_default_margin(SIDE_RIGHT, p_margin_right * scale);
	style->set_default_margin(SIDE_BOTTOM, p_margin_bottom * scale);
	style->set_default_margin(SIDE_TOP, p_margin_top * scale);

	return style;
}

void fill_default_theme(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &large_font, Ref<Texture2D> &default_icon, Ref<StyleBox> &default_style, float p_scale) {
	scale = p_scale;

	tex_cache = memnew(TexCacheMap);

	// Font colors
	const Color control_font_color = Color(0.875, 0.875, 0.875);
	const Color control_font_low_color = Color(0.7, 0.7, 0.7);
	const Color control_font_lower_color = Color(0.65, 0.65, 0.65);
	const Color control_font_hover_color = Color(0.95, 0.95, 0.95);
	const Color control_font_disabled_color = control_font_color * Color(1, 1, 1, 0.5);
	const Color control_font_pressed_color = Color(1, 1, 1);
	const Color control_selection_color = Color(0.5, 0.5, 0.5);

	// StyleBox colors
	const Color style_normal_color = Color(0.1, 0.1, 0.1, 0.5);
	const Color style_hover_color = Color(0.225, 0.225, 0.225, 0.5);
	const Color style_pressed_color = Color(0, 0, 0, 0.5);
	const Color style_disabled_color = Color(0.1, 0.1, 0.1, 0.25);
	const Color style_focus_color = Color(1, 1, 1, 0.75);
	const Color style_popup_color = Color(0.25, 0.25, 0.25, 1);
	const Color style_popup_border_color = Color(0.175, 0.175, 0.175, 1);
	const Color style_popup_hover_color = Color(0.4, 0.4, 0.4, 1);
	const Color style_selected_color = Color(1, 1, 1, 0.25);
	// Don't use a color too bright to keep the percentage readable.
	const Color style_progress_color = Color(1, 1, 1, 0.4);
	const Color style_separator_color = Color(0.5, 0.5, 0.5);

	// Convert the generated icon sources to a dictionary for easier access.
	// Unlike the editor icons, there is no central repository of icons in the Theme resource itself to keep it tidy.
	Dictionary icons;
	for (int i = 0; i < default_theme_icons_count; i++) {
		icons[default_theme_icons_names[i]] = generate_icon(i);
	}

	// Panel
	theme->set_stylebox("panel", "Panel", make_flat_stylebox(style_normal_color, 0, 0, 0, 0));
	theme->set_stylebox("panel_fg", "Panel", make_flat_stylebox(style_normal_color, 0, 0, 0, 0));

	// Button

	Ref<StyleBox> button_normal = make_flat_stylebox(style_normal_color);
	Ref<StyleBox> button_hover = make_flat_stylebox(style_hover_color);
	Ref<StyleBox> button_pressed = make_flat_stylebox(style_pressed_color);
	Ref<StyleBox> button_disabled = make_flat_stylebox(style_disabled_color);
	Ref<StyleBox> focus = make_flat_stylebox(style_focus_color, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);

	theme->set_stylebox("normal", "Button", button_normal);
	theme->set_stylebox("hover", "Button", button_hover);
	theme->set_stylebox("pressed", "Button", button_pressed);
	theme->set_stylebox("disabled", "Button", button_disabled);
	theme->set_stylebox("focus", "Button", focus);

	theme->set_font("font", "Button", Ref<Font>());
	theme->set_font_size("font_size", "Button", -1);
	theme->set_constant("outline_size", "Button", 0 * scale);

	theme->set_color("font_color", "Button", control_font_color);
	theme->set_color("font_pressed_color", "Button", control_font_pressed_color);
	theme->set_color("font_hover_color", "Button", control_font_hover_color);
	theme->set_color("font_hover_pressed_color", "Button", control_font_pressed_color);
	theme->set_color("font_disabled_color", "Button", control_font_disabled_color);
	theme->set_color("font_outline_color", "Button", Color(1, 1, 1));

	theme->set_color("icon_normal_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("icon_pressed_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("icon_hover_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("icon_hover_pressed_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("icon_disabled_color", "Button", Color(1, 1, 1, 1));

	theme->set_constant("hseparation", "Button", 2 * scale);

	// LinkButton

	theme->set_stylebox("focus", "LinkButton", focus);

	theme->set_font("font", "LinkButton", Ref<Font>());
	theme->set_font_size("font_size", "LinkButton", -1);

	theme->set_color("font_color", "LinkButton", control_font_color);
	theme->set_color("font_pressed_color", "LinkButton", control_font_pressed_color);
	theme->set_color("font_hover_color", "LinkButton", control_font_hover_color);
	theme->set_color("font_outline_color", "LinkButton", Color(1, 1, 1));

	theme->set_constant("outline_size", "LinkButton", 0);
	theme->set_constant("underline_spacing", "LinkButton", 2 * scale);

	// ColorPickerButton

	theme->set_stylebox("normal", "ColorPickerButton", button_normal);
	theme->set_stylebox("pressed", "ColorPickerButton", button_pressed);
	theme->set_stylebox("hover", "ColorPickerButton", button_hover);
	theme->set_stylebox("disabled", "ColorPickerButton", button_disabled);
	theme->set_stylebox("focus", "ColorPickerButton", focus);

	theme->set_font("font", "ColorPickerButton", Ref<Font>());
	theme->set_font_size("font_size", "ColorPickerButton", -1);

	theme->set_color("font_color", "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color("font_pressed_color", "ColorPickerButton", Color(0.8, 0.8, 0.8, 1));
	theme->set_color("font_hover_color", "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color("font_disabled_color", "ColorPickerButton", Color(0.9, 0.9, 0.9, 0.3));
	theme->set_color("font_outline_color", "ColorPickerButton", Color(1, 1, 1));

	theme->set_constant("hseparation", "ColorPickerButton", 2 * scale);
	theme->set_constant("outline_size", "ColorPickerButton", 0);

	// OptionButton
	theme->set_stylebox("focus", "OptionButton", focus);

	Ref<StyleBox> sb_optbutton_normal = make_flat_stylebox(style_normal_color, 2 * default_margin, default_margin, 21, default_margin);
	Ref<StyleBox> sb_optbutton_hover = make_flat_stylebox(style_hover_color, 2 * default_margin, default_margin, 21, default_margin);
	Ref<StyleBox> sb_optbutton_pressed = make_flat_stylebox(style_pressed_color, 2 * default_margin, default_margin, 21, default_margin);
	Ref<StyleBox> sb_optbutton_disabled = make_flat_stylebox(style_disabled_color, 2 * default_margin, default_margin, 21, default_margin);

	theme->set_stylebox("normal", "OptionButton", sb_optbutton_normal);
	theme->set_stylebox("hover", "OptionButton", sb_optbutton_hover);
	theme->set_stylebox("pressed", "OptionButton", sb_optbutton_pressed);
	theme->set_stylebox("disabled", "OptionButton", sb_optbutton_disabled);

	Ref<StyleBox> sb_optbutton_normal_mirrored = make_flat_stylebox(style_normal_color, 21, default_margin, 2 * default_margin, default_margin);
	Ref<StyleBox> sb_optbutton_hover_mirrored = make_flat_stylebox(style_hover_color, 21, default_margin, 2 * default_margin, default_margin);
	Ref<StyleBox> sb_optbutton_pressed_mirrored = make_flat_stylebox(style_pressed_color, 21, default_margin, 2 * default_margin, default_margin);
	Ref<StyleBox> sb_optbutton_disabled_mirrored = make_flat_stylebox(style_disabled_color, 21, default_margin, 2 * default_margin, default_margin);

	theme->set_stylebox("normal_mirrored", "OptionButton", sb_optbutton_normal_mirrored);
	theme->set_stylebox("hover_mirrored", "OptionButton", sb_optbutton_hover_mirrored);
	theme->set_stylebox("pressed_mirrored", "OptionButton", sb_optbutton_pressed_mirrored);
	theme->set_stylebox("disabled_mirrored", "OptionButton", sb_optbutton_disabled_mirrored);

	theme->set_icon("arrow", "OptionButton", make_icon(option_arrow_png));

	theme->set_font("font", "OptionButton", Ref<Font>());
	theme->set_font_size("font_size", "OptionButton", -1);

	theme->set_color("font_color", "OptionButton", control_font_color);
	theme->set_color("font_pressed_color", "OptionButton", control_font_pressed_color);
	theme->set_color("font_hover_color", "OptionButton", control_font_hover_color);
	theme->set_color("font_disabled_color", "OptionButton", control_font_disabled_color);
	theme->set_color("font_outline_color", "OptionButton", Color(1, 1, 1));

	theme->set_constant("hseparation", "OptionButton", 2 * scale);
	theme->set_constant("arrow_margin", "OptionButton", 2 * scale);
	theme->set_constant("outline_size", "OptionButton", 0);

	// MenuButton

	theme->set_stylebox("normal", "MenuButton", button_normal);
	theme->set_stylebox("pressed", "MenuButton", button_pressed);
	theme->set_stylebox("hover", "MenuButton", button_hover);
	theme->set_stylebox("disabled", "MenuButton", button_disabled);
	theme->set_stylebox("focus", "MenuButton", focus);

	theme->set_font("font", "MenuButton", Ref<Font>());
	theme->set_font_size("font_size", "MenuButton", -1);

	theme->set_color("font_color", "MenuButton", control_font_color);
	theme->set_color("font_pressed_color", "MenuButton", control_font_pressed_color);
	theme->set_color("font_hover_color", "MenuButton", control_font_hover_color);
	theme->set_color("font_disabled_color", "MenuButton", Color(1, 1, 1, 0.3));
	theme->set_color("font_outline_color", "MenuButton", Color(1, 1, 1));

	theme->set_constant("hseparation", "MenuButton", 3 * scale);
	theme->set_constant("outline_size", "MenuButton", 0);

	// CheckBox

	Ref<StyleBox> cbx_empty = memnew(StyleBoxEmpty);
	cbx_empty->set_default_margin(SIDE_LEFT, 4 * scale);
	cbx_empty->set_default_margin(SIDE_RIGHT, 4 * scale);
	cbx_empty->set_default_margin(SIDE_TOP, 4 * scale);
	cbx_empty->set_default_margin(SIDE_BOTTOM, 4 * scale);
	Ref<StyleBox> cbx_focus = focus;
	cbx_focus->set_default_margin(SIDE_LEFT, 4 * scale);
	cbx_focus->set_default_margin(SIDE_RIGHT, 4 * scale);
	cbx_focus->set_default_margin(SIDE_TOP, 4 * scale);
	cbx_focus->set_default_margin(SIDE_BOTTOM, 4 * scale);

	theme->set_stylebox("normal", "CheckBox", cbx_empty);
	theme->set_stylebox("pressed", "CheckBox", cbx_empty);
	theme->set_stylebox("disabled", "CheckBox", cbx_empty);
	theme->set_stylebox("hover", "CheckBox", cbx_empty);
	theme->set_stylebox("hover_pressed", "CheckBox", cbx_empty);
	theme->set_stylebox("focus", "CheckBox", cbx_focus);

	theme->set_icon("checked", "CheckBox", icons["checked"]);
	theme->set_icon("checked_disabled", "CheckBox", icons["checked"]);
	theme->set_icon("unchecked", "CheckBox", icons["unchecked"]);
	theme->set_icon("unchecked_disabled", "CheckBox", icons["unchecked"]);
	theme->set_icon("radio_checked", "CheckBox", icons["radio_checked"]);
	theme->set_icon("radio_checked_disabled", "CheckBox", icons["radio_checked"]);
	theme->set_icon("radio_unchecked", "CheckBox", icons["radio_unchecked"]);
	theme->set_icon("radio_unchecked_disabled", "CheckBox", icons["radio_unchecked"]);

	theme->set_font("font", "CheckBox", Ref<Font>());
	theme->set_font_size("font_size", "CheckBox", -1);

	theme->set_color("font_color", "CheckBox", control_font_color);
	theme->set_color("font_pressed_color", "CheckBox", control_font_pressed_color);
	theme->set_color("font_hover_color", "CheckBox", control_font_hover_color);
	theme->set_color("font_hover_pressed_color", "CheckBox", control_font_pressed_color);
	theme->set_color("font_disabled_color", "CheckBox", control_font_disabled_color);
	theme->set_color("font_outline_color", "CheckBox", Color(1, 1, 1));

	theme->set_constant("hseparation", "CheckBox", 4 * scale);
	theme->set_constant("check_vadjust", "CheckBox", 0 * scale);
	theme->set_constant("outline_size", "CheckBox", 0);

	// CheckButton

	Ref<StyleBox> cb_empty = memnew(StyleBoxEmpty);
	cb_empty->set_default_margin(SIDE_LEFT, 6 * scale);
	cb_empty->set_default_margin(SIDE_RIGHT, 6 * scale);
	cb_empty->set_default_margin(SIDE_TOP, 4 * scale);
	cb_empty->set_default_margin(SIDE_BOTTOM, 4 * scale);

	theme->set_stylebox("normal", "CheckButton", cb_empty);
	theme->set_stylebox("pressed", "CheckButton", cb_empty);
	theme->set_stylebox("disabled", "CheckButton", cb_empty);
	theme->set_stylebox("hover", "CheckButton", cb_empty);
	theme->set_stylebox("hover_pressed", "CheckButton", cb_empty);
	theme->set_stylebox("focus", "CheckButton", focus);

	theme->set_icon("on", "CheckButton", icons["toggle_on"]);
	theme->set_icon("on_disabled", "CheckButton", icons["toggle_on_disabled"]);
	theme->set_icon("off", "CheckButton", icons["toggle_off"]);
	theme->set_icon("off_disabled", "CheckButton", icons["toggle_off_disabled"]);

	theme->set_icon("on_mirrored", "CheckButton", icons["toggle_on_mirrored"]);
	theme->set_icon("on_disabled_mirrored", "CheckButton", icons["toggle_on_disabled_mirrored"]);
	theme->set_icon("off_mirrored", "CheckButton", icons["toggle_off_mirrored"]);
	theme->set_icon("off_disabled_mirrored", "CheckButton", icons["toggle_off_disabled_mirrored"]);

	theme->set_font("font", "CheckButton", Ref<Font>());
	theme->set_font_size("font_size", "CheckButton", -1);

	theme->set_color("font_color", "CheckButton", control_font_color);
	theme->set_color("font_pressed_color", "CheckButton", control_font_pressed_color);
	theme->set_color("font_hover_color", "CheckButton", control_font_hover_color);
	theme->set_color("font_hover_pressed_color", "CheckButton", control_font_pressed_color);
	theme->set_color("font_disabled_color", "CheckButton", control_font_disabled_color);
	theme->set_color("font_outline_color", "CheckButton", Color(1, 1, 1));

	theme->set_constant("hseparation", "CheckButton", 4 * scale);
	theme->set_constant("check_vadjust", "CheckButton", 0 * scale);
	theme->set_constant("outline_size", "CheckButton", 0);

	// Label

	theme->set_stylebox("normal", "Label", memnew(StyleBoxEmpty));
	theme->set_font("font", "Label", Ref<Font>());
	theme->set_font_size("font_size", "Label", -1);

	theme->set_color("font_color", "Label", Color(1, 1, 1));
	theme->set_color("font_shadow_color", "Label", Color(0, 0, 0, 0));
	theme->set_color("font_outline_color", "Label", Color(1, 1, 1));

	theme->set_constant("shadow_offset_x", "Label", 1 * scale);
	theme->set_constant("shadow_offset_y", "Label", 1 * scale);
	theme->set_constant("outline_size", "Label", 0);
	theme->set_constant("shadow_outline_size", "Label", 1 * scale);
	theme->set_constant("line_spacing", "Label", 3 * scale);

	// LineEdit

	Ref<StyleBoxFlat> style_line_edit = make_flat_stylebox(style_normal_color);
	// Add a line at the bottom to make LineEdits distinguishable from Buttons.
	style_line_edit->set_border_width(SIDE_BOTTOM, 2);
	style_line_edit->set_border_color(style_pressed_color);
	theme->set_stylebox("normal", "LineEdit", style_line_edit);

	theme->set_stylebox("focus", "LineEdit", focus);

	Ref<StyleBoxFlat> style_line_edit_read_only = make_flat_stylebox(style_disabled_color);
	// Add a line at the bottom to make LineEdits distinguishable from Buttons.
	style_line_edit_read_only->set_border_width(SIDE_BOTTOM, 2);
	style_line_edit_read_only->set_border_color(style_pressed_color * Color(1, 1, 1, 0.5));
	theme->set_stylebox("read_only", "LineEdit", style_line_edit_read_only);

	theme->set_font("font", "LineEdit", Ref<Font>());
	theme->set_font_size("font_size", "LineEdit", -1);

	theme->set_color("font_color", "LineEdit", control_font_color);
	theme->set_color("font_selected_color", "LineEdit", control_font_pressed_color);
	theme->set_color("font_uneditable_color", "LineEdit", control_font_disabled_color);
	theme->set_color("font_outline_color", "LineEdit", Color(1, 1, 1));
	theme->set_color("caret_color", "LineEdit", control_font_hover_color);
	theme->set_color("selection_color", "LineEdit", control_selection_color);
	theme->set_color("clear_button_color", "LineEdit", control_font_color);
	theme->set_color("clear_button_color_pressed", "LineEdit", control_font_pressed_color);

	theme->set_constant("minimum_character_width", "LineEdit", 4);
	theme->set_constant("outline_size", "LineEdit", 0);

	theme->set_icon("clear", "LineEdit", make_icon(line_edit_clear_png));

	// ProgressBar

	theme->set_stylebox("bg", "ProgressBar", make_flat_stylebox(style_disabled_color, 2, 2, 2, 2));
	theme->set_stylebox("fg", "ProgressBar", make_flat_stylebox(style_progress_color, 2, 2, 2, 2));

	theme->set_font("font", "ProgressBar", Ref<Font>());
	theme->set_font_size("font_size", "ProgressBar", -1);

	theme->set_color("font_color", "ProgressBar", control_font_hover_color);
	theme->set_color("font_shadow_color", "ProgressBar", Color(0, 0, 0));
	theme->set_color("font_outline_color", "ProgressBar", Color(1, 1, 1));

	theme->set_constant("outline_size", "ProgressBar", 0);

	// TextEdit

	theme->set_stylebox("normal", "TextEdit", style_line_edit);
	theme->set_stylebox("focus", "TextEdit", focus);
	theme->set_stylebox("read_only", "TextEdit", style_line_edit_read_only);

	theme->set_icon("tab", "TextEdit", make_icon(tab_png));
	theme->set_icon("space", "TextEdit", make_icon(space_png));

	theme->set_font("font", "TextEdit", Ref<Font>());
	theme->set_font_size("font_size", "TextEdit", -1);

	theme->set_color("background_color", "TextEdit", Color(0, 0, 0, 0));
	theme->set_color("font_color", "TextEdit", control_font_color);
	theme->set_color("font_selected_color", "TextEdit", control_font_pressed_color);
	theme->set_color("font_readonly_color", "TextEdit", control_font_disabled_color);
	theme->set_color("font_outline_color", "TextEdit", Color(1, 1, 1));
	theme->set_color("selection_color", "TextEdit", control_selection_color);
	theme->set_color("current_line_color", "TextEdit", Color(0.25, 0.25, 0.26, 0.8));
	theme->set_color("caret_color", "TextEdit", control_font_color);
	theme->set_color("caret_background_color", "TextEdit", Color(0, 0, 0));
	theme->set_color("brace_mismatch_color", "TextEdit", Color(1, 0.363, 0.363)); // Matches the error icon color.
	theme->set_color("word_highlighted_color", "TextEdit", Color(0.5, 0.5, 0.5, 0.25));

	theme->set_constant("line_spacing", "TextEdit", 4 * scale);
	theme->set_constant("outline_size", "TextEdit", 0);

	// CodeEdit

	theme->set_stylebox("normal", "CodeEdit", make_stylebox(tree_bg_png, 3, 3, 3, 3, 0, 0, 0, 0));
	theme->set_stylebox("focus", "CodeEdit", focus);
	theme->set_stylebox("read_only", "CodeEdit", make_stylebox(tree_bg_disabled_png, 4, 4, 4, 4, 0, 0, 0, 0));
	theme->set_stylebox("completion", "CodeEdit", make_stylebox(tree_bg_png, 3, 3, 3, 3, 0, 0, 0, 0));

	theme->set_icon("tab", "CodeEdit", make_icon(tab_png));
	theme->set_icon("space", "CodeEdit", make_icon(space_png));
	theme->set_icon("breakpoint", "CodeEdit", make_icon(graph_port_png));
	theme->set_icon("bookmark", "CodeEdit", make_icon(bookmark_png));
	theme->set_icon("executing_line", "CodeEdit", make_icon(arrow_right_png));
	theme->set_icon("can_fold", "CodeEdit", make_icon(arrow_down_png));
	theme->set_icon("folded", "CodeEdit", make_icon(arrow_right_png));
	theme->set_icon("folded_eol_icon", "CodeEdit", make_icon(ellipsis_png));

	theme->set_font("font", "CodeEdit", Ref<Font>());
	theme->set_font_size("font_size", "CodeEdit", -1);

	theme->set_color("background_color", "CodeEdit", Color(0, 0, 0, 0));
	theme->set_color("completion_background_color", "CodeEdit", Color(0.17, 0.16, 0.2));
	theme->set_color("completion_selected_color", "CodeEdit", Color(0.26, 0.26, 0.27));
	theme->set_color("completion_existing_color", "CodeEdit", Color(0.87, 0.87, 0.87, 0.13));
	theme->set_color("completion_scroll_color", "CodeEdit", control_font_pressed_color);
	theme->set_color("completion_font_color", "CodeEdit", Color(0.67, 0.67, 0.67));
	theme->set_color("font_color", "CodeEdit", control_font_color);
	theme->set_color("font_selected_color", "CodeEdit", Color(0, 0, 0));
	theme->set_color("font_readonly_color", "CodeEdit", Color(control_font_color.r, control_font_color.g, control_font_color.b, 0.5f));
	theme->set_color("font_outline_color", "CodeEdit", Color(1, 1, 1));
	theme->set_color("selection_color", "CodeEdit", control_selection_color);
	theme->set_color("bookmark_color", "CodeEdit", Color(0.5, 0.64, 1, 0.8));
	theme->set_color("breakpoint_color", "CodeEdit", Color(0.9, 0.29, 0.3));
	theme->set_color("executing_line_color", "CodeEdit", Color(0.98, 0.89, 0.27));
	theme->set_color("current_line_color", "CodeEdit", Color(0.25, 0.25, 0.26, 0.8));
	theme->set_color("code_folding_color", "CodeEdit", Color(0.8, 0.8, 0.8, 0.8));
	theme->set_color("caret_color", "CodeEdit", control_font_color);
	theme->set_color("caret_background_color", "CodeEdit", Color(0, 0, 0));
	theme->set_color("brace_mismatch_color", "CodeEdit", Color(1, 0.2, 0.2));
	theme->set_color("line_number_color", "CodeEdit", Color(0.67, 0.67, 0.67, 0.4));
	theme->set_color("safe_line_number_color", "CodeEdit", Color(0.67, 0.78, 0.67, 0.6));
	theme->set_color("word_highlighted_color", "CodeEdit", Color(0.8, 0.9, 0.9, 0.15));

	theme->set_constant("completion_lines", "CodeEdit", 7);
	theme->set_constant("completion_max_width", "CodeEdit", 50);
	theme->set_constant("completion_scroll_width", "CodeEdit", 3);
	theme->set_constant("line_spacing", "CodeEdit", 4 * scale);
	theme->set_constant("outline_size", "CodeEdit", 0);

	Ref<Texture2D> empty_icon = memnew(ImageTexture);

	// HScrollBar

	theme->set_stylebox("scroll", "HScrollBar", make_stylebox(scroll_bg_png, 5, 5, 5, 5, 0, 0, 0, 0));
	theme->set_stylebox("scroll_focus", "HScrollBar", make_stylebox(scroll_bg_png, 5, 5, 5, 5, 0, 0, 0, 0));
	theme->set_stylebox("grabber", "HScrollBar", make_stylebox(scroll_grabber_png, 5, 5, 5, 5, 2, 2, 2, 2));
	theme->set_stylebox("grabber_highlight", "HScrollBar", make_stylebox(scroll_grabber_hl_png, 5, 5, 5, 5, 2, 2, 2, 2));
	theme->set_stylebox("grabber_pressed", "HScrollBar", make_stylebox(scroll_grabber_pressed_png, 5, 5, 5, 5, 2, 2, 2, 2));

	theme->set_icon("increment", "HScrollBar", empty_icon);
	theme->set_icon("increment_highlight", "HScrollBar", empty_icon);
	theme->set_icon("decrement", "HScrollBar", empty_icon);
	theme->set_icon("decrement_highlight", "HScrollBar", empty_icon);

	// VScrollBar

	theme->set_stylebox("scroll", "VScrollBar", make_stylebox(scroll_bg_png, 5, 5, 5, 5, 0, 0, 0, 0));
	theme->set_stylebox("scroll_focus", "VScrollBar", make_stylebox(scroll_bg_png, 5, 5, 5, 5, 0, 0, 0, 0));
	theme->set_stylebox("grabber", "VScrollBar", make_stylebox(scroll_grabber_png, 5, 5, 5, 5, 2, 2, 2, 2));
	theme->set_stylebox("grabber_highlight", "VScrollBar", make_stylebox(scroll_grabber_hl_png, 5, 5, 5, 5, 2, 2, 2, 2));
	theme->set_stylebox("grabber_pressed", "VScrollBar", make_stylebox(scroll_grabber_pressed_png, 5, 5, 5, 5, 2, 2, 2, 2));

	theme->set_icon("increment", "VScrollBar", empty_icon);
	theme->set_icon("increment_highlight", "VScrollBar", empty_icon);
	theme->set_icon("decrement", "VScrollBar", empty_icon);
	theme->set_icon("decrement_highlight", "VScrollBar", empty_icon);

	// HSlider

	theme->set_stylebox("slider", "HSlider", make_stylebox(hslider_bg_png, 4, 4, 4, 4));
	theme->set_stylebox("grabber_area", "HSlider", make_stylebox(hslider_bg_png, 4, 4, 4, 4));
	theme->set_stylebox("grabber_area_highlight", "HSlider", make_stylebox(hslider_bg_png, 4, 4, 4, 4));

	theme->set_icon("grabber", "HSlider", make_icon(hslider_grabber_png));
	theme->set_icon("grabber_highlight", "HSlider", make_icon(hslider_grabber_hl_png));
	theme->set_icon("grabber_disabled", "HSlider", make_icon(hslider_grabber_disabled_png));
	theme->set_icon("tick", "HSlider", make_icon(hslider_tick_png));

	// VSlider

	theme->set_stylebox("slider", "VSlider", make_stylebox(vslider_bg_png, 4, 4, 4, 4));
	theme->set_stylebox("grabber_area", "VSlider", make_stylebox(vslider_bg_png, 4, 4, 4, 4));
	theme->set_stylebox("grabber_area_highlight", "VSlider", make_stylebox(vslider_bg_png, 4, 4, 4, 4));

	theme->set_icon("grabber", "VSlider", make_icon(vslider_grabber_png));
	theme->set_icon("grabber_highlight", "VSlider", make_icon(vslider_grabber_hl_png));
	theme->set_icon("grabber_disabled", "VSlider", make_icon(vslider_grabber_disabled_png));
	theme->set_icon("tick", "VSlider", make_icon(vslider_tick_png));

	// SpinBox

	theme->set_icon("updown", "SpinBox", make_icon(spinbox_updown_png));

	// ScrollContainer

	Ref<StyleBoxEmpty> empty;
	empty.instantiate();
	theme->set_stylebox("bg", "ScrollContainer", empty);

	// WindowDialog

	theme->set_stylebox("panel", "Window", default_style);
	theme->set_stylebox("window_panel", "Window", sb_expand(make_stylebox(popup_window_png, 10, 26, 10, 8), 8, 24, 8, 6));
	theme->set_constant("scaleborder_size", "Window", 4 * scale);

	theme->set_font("title_font", "Window", large_font);
	theme->set_font_size("title_font_size", "Window", -1);

	theme->set_color("title_color", "Window", Color(0, 0, 0));
	theme->set_color("title_outline_modulate", "Window", Color(1, 1, 1));

	theme->set_constant("title_outline_size", "Window", 0);
	theme->set_constant("title_height", "Window", 20 * scale);
	theme->set_constant("resize_margin", "Window", 4 * scale);

	theme->set_icon("close", "Window", make_icon(close_png));
	theme->set_icon("close_highlight", "Window", make_icon(close_hl_png));
	theme->set_constant("close_h_ofs", "Window", 18 * scale);
	theme->set_constant("close_v_ofs", "Window", 18 * scale);

	// FileDialog

	theme->set_icon("parent_folder", "FileDialog", make_icon(icon_parent_folder_png));
	theme->set_icon("back_folder", "FileDialog", make_icon(arrow_left_png));
	theme->set_icon("forward_folder", "FileDialog", make_icon(arrow_right_png));
	theme->set_icon("reload", "FileDialog", make_icon(icon_reload_png));
	theme->set_icon("toggle_hidden", "FileDialog", make_icon(icon_visibility_png));

	// Popup

	theme->set_stylebox("panel", "PopupPanel", make_flat_stylebox(style_normal_color));

	// PopupDialog

	theme->set_stylebox("panel", "PopupDialog", make_flat_stylebox(style_normal_color));

	// PopupMenu

	Ref<StyleBoxLine> separator_horizontal = memnew(StyleBoxLine);
	separator_horizontal->set_thickness(Math::round(scale));
	separator_horizontal->set_color(style_separator_color);
	separator_horizontal->set_default_margin(SIDE_LEFT, default_margin);
	separator_horizontal->set_default_margin(SIDE_TOP, 0);
	separator_horizontal->set_default_margin(SIDE_RIGHT, default_margin);
	separator_horizontal->set_default_margin(SIDE_BOTTOM, 0);
	Ref<StyleBoxLine> separator_vertical = separator_horizontal->duplicate();
	separator_vertical->set_vertical(true);
	separator_vertical->set_default_margin(SIDE_LEFT, 0);
	separator_vertical->set_default_margin(SIDE_TOP, default_margin);
	separator_vertical->set_default_margin(SIDE_RIGHT, 0);
	separator_vertical->set_default_margin(SIDE_BOTTOM, default_margin);

	// Always display a border for PopupMenus so they can be distinguished from their background.
	Ref<StyleBoxFlat> style_popup_panel = make_flat_stylebox(style_popup_color);
	style_popup_panel->set_border_width_all(2);
	style_popup_panel->set_border_color(style_popup_border_color);
	Ref<StyleBoxFlat> style_popup_panel_disabled = style_popup_panel->duplicate();
	style_popup_panel_disabled->set_bg_color(style_disabled_color);

	theme->set_stylebox("panel", "PopupMenu", style_popup_panel);
	theme->set_stylebox("panel_disabled", "PopupMenu", style_popup_panel_disabled);
	theme->set_stylebox("hover", "PopupMenu", make_flat_stylebox(style_popup_hover_color));
	theme->set_stylebox("separator", "PopupMenu", separator_horizontal);
	theme->set_stylebox("labeled_separator_left", "PopupMenu", separator_horizontal);
	theme->set_stylebox("labeled_separator_right", "PopupMenu", separator_horizontal);

	theme->set_icon("checked", "PopupMenu", icons["checked"]);
	theme->set_icon("unchecked", "PopupMenu", icons["unchecked"]);
	theme->set_icon("radio_checked", "PopupMenu", icons["radio_checked"]);
	theme->set_icon("radio_unchecked", "PopupMenu", icons["radio_unchecked"]);
	theme->set_icon("submenu", "PopupMenu", make_icon(submenu_png));
	theme->set_icon("submenu_mirrored", "PopupMenu", make_icon(submenu_mirrored_png));

	theme->set_font("font", "PopupMenu", Ref<Font>());
	theme->set_font_size("font_size", "PopupMenu", -1);

	theme->set_color("font_color", "PopupMenu", control_font_color);
	theme->set_color("font_accelerator_color", "PopupMenu", Color(0.7, 0.7, 0.7, 0.8));
	theme->set_color("font_disabled_color", "PopupMenu", Color(0.4, 0.4, 0.4, 0.8));
	theme->set_color("font_hover_color", "PopupMenu", control_font_color);
	theme->set_color("font_separator_color", "PopupMenu", control_font_color);
	theme->set_color("font_outline_color", "PopupMenu", Color(1, 1, 1));

	theme->set_constant("hseparation", "PopupMenu", 4 * scale);
	theme->set_constant("vseparation", "PopupMenu", 4 * scale);
	theme->set_constant("outline_size", "PopupMenu", 0);
	theme->set_constant("item_start_padding", "PopupMenu", 2 * scale);
	theme->set_constant("item_end_padding", "PopupMenu", 2 * scale);

	// GraphNode

	Ref<StyleBoxTexture> graphsb = make_stylebox(graph_node_png, 6, 24, 6, 5, 16, 24, 16, 6);
	Ref<StyleBoxTexture> graphsbcomment = make_stylebox(graph_node_comment_png, 6, 24, 6, 5, 16, 24, 16, 6);
	Ref<StyleBoxTexture> graphsbcommentselected = make_stylebox(graph_node_comment_focus_png, 6, 24, 6, 5, 16, 24, 16, 6);
	Ref<StyleBoxTexture> graphsbselected = make_stylebox(graph_node_selected_png, 6, 24, 6, 5, 16, 24, 16, 6);
	Ref<StyleBoxTexture> graphsbdefault = make_stylebox(graph_node_default_png, 4, 4, 4, 4, 6, 4, 4, 4);
	Ref<StyleBoxTexture> graphsbdeffocus = make_stylebox(graph_node_default_focus_png, 4, 4, 4, 4, 6, 4, 4, 4);
	Ref<StyleBoxTexture> graph_bpoint = make_stylebox(graph_node_breakpoint_png, 6, 24, 6, 5, 16, 24, 16, 6);
	Ref<StyleBoxTexture> graph_position = make_stylebox(graph_node_position_png, 6, 24, 6, 5, 16, 24, 16, 6);

	theme->set_stylebox("frame", "GraphNode", graphsb);
	theme->set_stylebox("selectedframe", "GraphNode", graphsbselected);
	theme->set_stylebox("defaultframe", "GraphNode", graphsbdefault);
	theme->set_stylebox("defaultfocus", "GraphNode", graphsbdeffocus);
	theme->set_stylebox("comment", "GraphNode", graphsbcomment);
	theme->set_stylebox("commentfocus", "GraphNode", graphsbcommentselected);
	theme->set_stylebox("breakpoint", "GraphNode", graph_bpoint);
	theme->set_stylebox("position", "GraphNode", graph_position);
	theme->set_constant("separation", "GraphNode", 1 * scale);
	theme->set_icon("port", "GraphNode", make_icon(graph_port_png));
	theme->set_icon("close", "GraphNode", make_icon(graph_node_close_png));
	theme->set_icon("resizer", "GraphNode", make_icon(window_resizer_png));
	theme->set_font("title_font", "GraphNode", Ref<Font>());
	theme->set_color("title_color", "GraphNode", Color(0, 0, 0, 1));
	theme->set_color("close_color", "GraphNode", Color(0, 0, 0, 1));
	theme->set_color("resizer_color", "GraphNode", Color(0, 0, 0, 1));
	theme->set_constant("title_offset", "GraphNode", 20 * scale);
	theme->set_constant("close_offset", "GraphNode", 18 * scale);
	theme->set_constant("port_offset", "GraphNode", 3 * scale);

	// Tree

	theme->set_stylebox("bg", "Tree", make_stylebox(tree_bg_png, 4, 4, 4, 5));
	theme->set_stylebox("bg_focus", "Tree", focus);
	theme->set_stylebox("selected", "Tree", make_flat_stylebox(style_selected_color));
	theme->set_stylebox("selected_focus", "Tree", make_flat_stylebox(style_selected_color));
	theme->set_stylebox("cursor", "Tree", focus);
	theme->set_stylebox("cursor_unfocused", "Tree", focus);
	theme->set_stylebox("button_pressed", "Tree", button_pressed);
	theme->set_stylebox("title_button_normal", "Tree", make_stylebox(tree_title_png, 4, 4, 4, 4));
	theme->set_stylebox("title_button_pressed", "Tree", make_stylebox(tree_title_pressed_png, 4, 4, 4, 4));
	theme->set_stylebox("title_button_hover", "Tree", make_stylebox(tree_title_png, 4, 4, 4, 4));
	theme->set_stylebox("custom_button", "Tree", button_normal);
	theme->set_stylebox("custom_button_pressed", "Tree", button_pressed);
	theme->set_stylebox("custom_button_hover", "Tree", button_hover);

	theme->set_icon("checked", "Tree", icons["checked"]);
	theme->set_icon("unchecked", "Tree", icons["unchecked"]);
	theme->set_icon("updown", "Tree", make_icon(updown_png));
	theme->set_icon("select_arrow", "Tree", make_icon(dropdown_png));
	theme->set_icon("arrow", "Tree", make_icon(arrow_down_png));
	theme->set_icon("arrow_collapsed", "Tree", make_icon(arrow_right_png));
	theme->set_icon("arrow_collapsed_mirrored", "Tree", make_icon(arrow_left_png));

	theme->set_font("title_button_font", "Tree", Ref<Font>());
	theme->set_font("font", "Tree", Ref<Font>());
	theme->set_font_size("font_size", "Tree", -1);

	theme->set_color("title_button_color", "Tree", control_font_color);
	theme->set_color("font_color", "Tree", control_font_low_color);
	theme->set_color("font_selected_color", "Tree", control_font_pressed_color);
	theme->set_color("font_outline_color", "Tree", Color(1, 1, 1));
	theme->set_color("guide_color", "Tree", Color(0, 0, 0, 0.1));
	theme->set_color("drop_position_color", "Tree", Color(1, 0.3, 0.2));
	theme->set_color("relationship_line_color", "Tree", Color(0.27, 0.27, 0.27));
	theme->set_color("parent_hl_line_color", "Tree", Color(0.27, 0.27, 0.27));
	theme->set_color("children_hl_line_color", "Tree", Color(0.27, 0.27, 0.27));
	theme->set_color("custom_button_font_highlight", "Tree", control_font_hover_color);

	theme->set_constant("hseparation", "Tree", 4 * scale);
	theme->set_constant("vseparation", "Tree", 4 * scale);
	theme->set_constant("item_margin", "Tree", 12 * scale);
	theme->set_constant("button_margin", "Tree", 4 * scale);
	theme->set_constant("draw_relationship_lines", "Tree", 0);
	theme->set_constant("relationship_line_width", "Tree", 1);
	theme->set_constant("parent_hl_line_width", "Tree", 1);
	theme->set_constant("children_hl_line_width", "Tree", 1);
	theme->set_constant("parent_hl_line_margin", "Tree", 0);
	theme->set_constant("draw_guides", "Tree", 1);
	theme->set_constant("scroll_border", "Tree", 4);
	theme->set_constant("scroll_speed", "Tree", 12);
	theme->set_constant("outline_size", "Tree", 0);

	// ItemList

	theme->set_stylebox("bg", "ItemList", make_flat_stylebox(style_normal_color));
	theme->set_stylebox("bg_focus", "ItemList", focus);
	theme->set_constant("hseparation", "ItemList", 4);
	theme->set_constant("vseparation", "ItemList", 2);
	theme->set_constant("icon_margin", "ItemList", 4);
	theme->set_constant("line_separation", "ItemList", 2 * scale);

	theme->set_font("font", "ItemList", Ref<Font>());
	theme->set_font_size("font_size", "ItemList", -1);

	theme->set_color("font_color", "ItemList", control_font_lower_color);
	theme->set_color("font_selected_color", "ItemList", control_font_pressed_color);
	theme->set_color("font_outline_color", "ItemList", Color(1, 1, 1));
	theme->set_color("guide_color", "ItemList", Color(0, 0, 0, 0.1));
	theme->set_stylebox("selected", "ItemList", make_flat_stylebox(style_selected_color));
	theme->set_stylebox("selected_focus", "ItemList", make_flat_stylebox(style_selected_color));
	theme->set_stylebox("cursor", "ItemList", focus);
	theme->set_stylebox("cursor_unfocused", "ItemList", focus);

	theme->set_constant("outline_size", "ItemList", 0);

	// TabContainer

	Ref<StyleBoxTexture> tc_sb = sb_expand(make_stylebox(tab_container_bg_png, 4, 4, 4, 4, 4, 4, 4, 4), 3, 3, 3, 3);

	tc_sb->set_expand_margin_size(SIDE_TOP, 2 * scale);
	tc_sb->set_default_margin(SIDE_TOP, 8 * scale);

	Ref<StyleBoxFlat> style_tab_selected = make_flat_stylebox(style_normal_color, 10, 4, 10, 4, 0);
	style_tab_selected->set_border_width(SIDE_TOP, Math::round(2 * scale));
	style_tab_selected->set_border_color(style_focus_color);
	Ref<StyleBoxFlat> style_tab_unselected = make_flat_stylebox(style_pressed_color, 10, 4, 10, 4, 0);
	// Add some spacing between unselected tabs to make them easier to distinguish from each other.
	style_tab_unselected->set_border_width(SIDE_LEFT, Math::round(scale));
	style_tab_unselected->set_border_width(SIDE_RIGHT, Math::round(scale));
	style_tab_unselected->set_border_color(style_popup_border_color);
	Ref<StyleBoxFlat> style_tab_disabled = style_tab_unselected->duplicate();
	style_tab_disabled->set_bg_color(style_disabled_color);

	theme->set_stylebox("tab_selected", "TabContainer", style_tab_selected);
	theme->set_stylebox("tab_unselected", "TabContainer", style_tab_unselected);
	theme->set_stylebox("tab_disabled", "TabContainer", style_tab_disabled);
	theme->set_stylebox("panel", "TabContainer", make_flat_stylebox(style_normal_color, 0, 0, 0, 0));

	theme->set_icon("increment", "TabContainer", make_icon(scroll_button_right_png));
	theme->set_icon("increment_highlight", "TabContainer", make_icon(scroll_button_right_hl_png));
	theme->set_icon("decrement", "TabContainer", make_icon(scroll_button_left_png));
	theme->set_icon("decrement_highlight", "TabContainer", make_icon(scroll_button_left_hl_png));
	theme->set_icon("menu", "TabContainer", make_icon(tab_menu_png));
	theme->set_icon("menu_highlight", "TabContainer", make_icon(tab_menu_hl_png));

	theme->set_font("font", "TabContainer", Ref<Font>());
	theme->set_font_size("font_size", "TabContainer", -1);

	theme->set_color("font_selected_color", "TabContainer", control_font_hover_color);
	theme->set_color("font_unselected_color", "TabContainer", control_font_low_color);
	theme->set_color("font_disabled_color", "TabContainer", control_font_disabled_color);
	theme->set_color("font_outline_color", "TabContainer", Color(1, 1, 1));

	theme->set_constant("side_margin", "TabContainer", 8 * scale);
	theme->set_constant("icon_separation", "TabContainer", 4 * scale);
	theme->set_constant("outline_size", "TabContainer", 0);

	// Tabs

	theme->set_stylebox("tab_selected", "Tabs", sb_expand(make_stylebox(tab_current_png, 4, 3, 4, 1, 16, 3, 16, 2), 2, 2, 2, 2));
	theme->set_stylebox("tab_unselected", "Tabs", sb_expand(make_stylebox(tab_behind_png, 5, 4, 5, 1, 16, 5, 16, 2), 3, 3, 3, 3));
	theme->set_stylebox("tab_disabled", "Tabs", sb_expand(make_stylebox(tab_disabled_png, 5, 5, 5, 1, 16, 6, 16, 4), 3, 0, 3, 3));
	theme->set_stylebox("button_pressed", "Tabs", button_pressed);
	theme->set_stylebox("button", "Tabs", button_normal);

	theme->set_icon("increment", "Tabs", make_icon(scroll_button_right_png));
	theme->set_icon("increment_highlight", "Tabs", make_icon(scroll_button_right_hl_png));
	theme->set_icon("decrement", "Tabs", make_icon(scroll_button_left_png));
	theme->set_icon("decrement_highlight", "Tabs", make_icon(scroll_button_left_hl_png));
	theme->set_icon("close", "Tabs", make_icon(tab_close_png));

	theme->set_font("font", "Tabs", Ref<Font>());
	theme->set_font_size("font_size", "Tabs", -1);

	theme->set_color("font_selected_color", "Tabs", control_font_hover_color);
	theme->set_color("font_unselected_color", "Tabs", control_font_low_color);
	theme->set_color("font_disabled_color", "Tabs", control_font_disabled_color);
	theme->set_color("font_outline_color", "Tabs", Color(1, 1, 1));

	theme->set_constant("hseparation", "Tabs", 4 * scale);
	theme->set_constant("outline_size", "Tabs", 0);

	// Separators

	theme->set_stylebox("separator", "HSeparator", separator_horizontal);
	theme->set_stylebox("separator", "VSeparator", separator_vertical);

	theme->set_icon("close", "Icons", make_icon(icon_close_png));
	theme->set_font("normal", "Fonts", Ref<Font>());
	theme->set_font("large", "Fonts", large_font);

	theme->set_constant("separation", "HSeparator", 4 * scale);
	theme->set_constant("separation", "VSeparator", 4 * scale);

	// Dialogs

	theme->set_constant("margin", "Dialogs", 8 * scale);
	theme->set_constant("button_margin", "Dialogs", 32 * scale);

	// FileDialog

	theme->set_icon("folder", "FileDialog", make_icon(icon_folder_png));
	theme->set_icon("file", "FileDialog", make_icon(icon_file_png));
	theme->set_color("folder_icon_modulate", "FileDialog", Color(1, 1, 1));
	theme->set_color("file_icon_modulate", "FileDialog", Color(1, 1, 1));
	theme->set_color("files_disabled", "FileDialog", Color(0, 0, 0, 0.7));

	// ColorPicker

	theme->set_constant("margin", "ColorPicker", 4 * scale);
	theme->set_constant("sv_width", "ColorPicker", 256 * scale);
	theme->set_constant("sv_height", "ColorPicker", 256 * scale);
	theme->set_constant("h_width", "ColorPicker", 30 * scale);
	theme->set_constant("label_width", "ColorPicker", 10 * scale);

	theme->set_icon("screen_picker", "ColorPicker", make_icon(icon_color_pick_png));
	theme->set_icon("add_preset", "ColorPicker", make_icon(icon_add_png));
	theme->set_icon("color_hue", "ColorPicker", make_icon(color_picker_hue_png));
	theme->set_icon("color_sample", "ColorPicker", make_icon(color_picker_sample_png));
	theme->set_icon("preset_bg", "ColorPicker", make_icon(mini_checkerboard_png));
	theme->set_icon("overbright_indicator", "ColorPicker", make_icon(overbright_indicator_png));
	theme->set_icon("bar_arrow", "ColorPicker", make_icon(bar_arrow_png));
	theme->set_icon("picker_cursor", "ColorPicker", make_icon(picker_cursor_png));

	theme->set_icon("bg", "ColorPickerButton", make_icon(mini_checkerboard_png));

	// TooltipPanel + TooltipLabel

	theme->set_stylebox("panel", "TooltipPanel",
			make_flat_stylebox(Color(0, 0, 0, 0.5), 2 * default_margin, 0.5 * default_margin, 2 * default_margin, 0.5 * default_margin));

	theme->set_font("font", "TooltipLabel", Ref<Font>());
	theme->set_font_size("font_size", "TooltipLabel", -1);

	theme->set_color("font_color", "TooltipLabel", control_font_color);
	theme->set_color("font_shadow_color", "TooltipLabel", Color(0, 0, 0, 0));
	theme->set_color("font_outline_color", "TooltipLabel", Color(0, 0, 0, 0));

	theme->set_constant("shadow_offset_x", "TooltipLabel", 1);
	theme->set_constant("shadow_offset_y", "TooltipLabel", 1);
	theme->set_constant("outline_size", "TooltipLabel", 0);

	// RichTextLabel

	theme->set_stylebox("focus", "RichTextLabel", focus);
	theme->set_stylebox("normal", "RichTextLabel", make_empty_stylebox(0, 0, 0, 0));

	theme->set_font("normal_font", "RichTextLabel", Ref<Font>());
	theme->set_font("bold_font", "RichTextLabel", Ref<Font>());
	theme->set_font("italics_font", "RichTextLabel", Ref<Font>());
	theme->set_font("bold_italics_font", "RichTextLabel", Ref<Font>());
	theme->set_font("mono_font", "RichTextLabel", Ref<Font>());

	theme->set_font_size("normal_font_size", "RichTextLabel", -1);
	theme->set_font_size("bold_font_size", "RichTextLabel", -1);
	theme->set_font_size("italics_font_size", "RichTextLabel", -1);
	theme->set_font_size("bold_italics_font_size", "RichTextLabel", -1);
	theme->set_font_size("mono_font_size", "RichTextLabel", -1);

	theme->set_color("default_color", "RichTextLabel", Color(1, 1, 1));
	theme->set_color("font_selected_color", "RichTextLabel", Color(0, 0, 0));
	theme->set_color("selection_color", "RichTextLabel", Color(0.1, 0.1, 1, 0.8));

	theme->set_color("font_shadow_color", "RichTextLabel", Color(0, 0, 0, 0));

	theme->set_color("font_outline_color", "RichTextLabel", Color(1, 1, 1));

	theme->set_constant("shadow_offset_x", "RichTextLabel", 1 * scale);
	theme->set_constant("shadow_offset_y", "RichTextLabel", 1 * scale);
	theme->set_constant("shadow_as_outline", "RichTextLabel", 0 * scale);

	theme->set_constant("line_separation", "RichTextLabel", 1 * scale);
	theme->set_constant("table_hseparation", "RichTextLabel", 3 * scale);
	theme->set_constant("table_vseparation", "RichTextLabel", 3 * scale);

	theme->set_constant("outline_size", "RichTextLabel", 0);

	theme->set_color("table_odd_row_bg", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("table_even_row_bg", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("table_border", "RichTextLabel", Color(0, 0, 0, 0));

	// Containers

	theme->set_stylebox("bg", "VSplitContainer", make_stylebox(vsplit_bg_png, 1, 1, 1, 1));
	theme->set_stylebox("bg", "HSplitContainer", make_stylebox(hsplit_bg_png, 1, 1, 1, 1));

	theme->set_icon("grabber", "VSplitContainer", make_icon(vsplitter_png));
	theme->set_icon("grabber", "HSplitContainer", make_icon(hsplitter_png));

	theme->set_constant("separation", "HBoxContainer", 4 * scale);
	theme->set_constant("separation", "VBoxContainer", 4 * scale);
	theme->set_constant("margin_left", "MarginContainer", 0 * scale);
	theme->set_constant("margin_top", "MarginContainer", 0 * scale);
	theme->set_constant("margin_right", "MarginContainer", 0 * scale);
	theme->set_constant("margin_bottom", "MarginContainer", 0 * scale);
	theme->set_constant("hseparation", "GridContainer", 4 * scale);
	theme->set_constant("vseparation", "GridContainer", 4 * scale);
	theme->set_constant("separation", "HSplitContainer", 12 * scale);
	theme->set_constant("separation", "VSplitContainer", 12 * scale);
	theme->set_constant("autohide", "HSplitContainer", 1 * scale);
	theme->set_constant("autohide", "VSplitContainer", 1 * scale);

	theme->set_stylebox("panel", "PanelContainer", make_flat_stylebox(style_normal_color, 0, 0, 0, 0));

	theme->set_icon("minus", "GraphEdit", make_icon(icon_zoom_less_png));
	theme->set_icon("reset", "GraphEdit", make_icon(icon_zoom_reset_png));
	theme->set_icon("more", "GraphEdit", make_icon(icon_zoom_more_png));
	theme->set_icon("snap", "GraphEdit", make_icon(icon_snap_grid_png));
	theme->set_icon("minimap", "GraphEdit", make_icon(icon_grid_minimap_png));
	theme->set_stylebox("bg", "GraphEdit", make_stylebox(tree_bg_png, 4, 4, 4, 5));
	theme->set_color("grid_minor", "GraphEdit", Color(1, 1, 1, 0.05));
	theme->set_color("grid_major", "GraphEdit", Color(1, 1, 1, 0.2));
	theme->set_color("selection_fill", "GraphEdit", Color(1, 1, 1, 0.3));
	theme->set_color("selection_stroke", "GraphEdit", Color(1, 1, 1, 0.8));
	theme->set_color("activity", "GraphEdit", Color(1, 1, 1));
	theme->set_constant("bezier_len_pos", "GraphEdit", 80 * scale);
	theme->set_constant("bezier_len_neg", "GraphEdit", 160 * scale);

	// Visual Node Ports

	theme->set_constant("port_grab_distance_horizontal", "GraphEdit", 48 * scale);
	theme->set_constant("port_grab_distance_vertical", "GraphEdit", 6 * scale);

	theme->set_stylebox("bg", "GraphEditMinimap", make_flat_stylebox(Color(0.24, 0.24, 0.24), true, 0, 0, 0, 0, 0));
	Ref<StyleBoxFlat> style_minimap_camera = make_flat_stylebox(Color(0.65, 0.65, 0.65, 0.2), true, 0, 0, 0, 0, 0);
	style_minimap_camera->set_border_color(Color(0.65, 0.65, 0.65, 0.45));
	style_minimap_camera->set_border_width_all(1);
	theme->set_stylebox("camera", "GraphEditMinimap", style_minimap_camera);
	Ref<StyleBoxFlat> style_minimap_node = make_flat_stylebox(Color(1, 1, 1), true, 0, 0, 0, 0, 0);
	style_minimap_node->set_corner_radius_all(2);
	theme->set_stylebox("node", "GraphEditMinimap", style_minimap_node);

	Ref<Texture2D> resizer_icon = make_icon(window_resizer_png);
	theme->set_icon("resizer", "GraphEditMinimap", flip_icon(resizer_icon, true, true));
	theme->set_color("resizer_color", "GraphEditMinimap", Color(1, 1, 1, 0.85));

	// Theme

	default_icon = icons["error_icon"];
	// Same color as the error icon.
	default_style = make_flat_stylebox(Color(1, 0.365, 0.365), 4, 4, 4, 4, 0, false, 2);

	memdelete(tex_cache);
}

void make_default_theme(float p_scale, Ref<Font> p_font) {
	Ref<Theme> t;
	t.instantiate();

	Ref<StyleBox> default_style;
	Ref<Texture2D> default_icon;
	Ref<Font> default_font;
	int default_font_size = 16;
	if (p_font.is_valid()) {
		// Use the custom font defined in the Project Settings.
		default_font = p_font;
	} else {
		// Use the default DynamicFont (separate from the editor font).
		// The default DynamicFont is chosen to have a small file size since it's
		// embedded in both editor and export template binaries.
		Ref<Font> dynamic_font;
		dynamic_font.instantiate();

		Ref<FontData> dynamic_font_data;
		dynamic_font_data.instantiate();
		dynamic_font_data->load_memory(_font_OpenSans_SemiBold, _font_OpenSans_SemiBold_size, "ttf", default_font_size);
		dynamic_font->add_data(dynamic_font_data);

		default_font = dynamic_font;
	}

	Ref<Font> large_font = default_font;
	fill_default_theme(t, default_font, large_font, default_icon, default_style, p_scale);

	Theme::set_default(t);
	Theme::set_default_icon(default_icon);
	Theme::set_default_style(default_style);
	Theme::set_default_font(default_font);
	Theme::set_default_font_size(default_font_size);
}

void clear_default_theme() {
	Theme::set_project_default(nullptr);
	Theme::set_default(nullptr);
	Theme::set_default_icon(nullptr);
	Theme::set_default_style(nullptr);
	Theme::set_default_font(nullptr);
}
