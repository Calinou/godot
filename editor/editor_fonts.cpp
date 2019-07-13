/*************************************************************************/
/*  editor_fonts.cpp                                                     */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
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

#include "editor_fonts.h"

#include "builtin_fonts.gen.h"
#include "core/os/dir_access.h"
#include "editor_scale.h"
#include "editor_settings.h"
#include "scene/resources/default_theme/default_theme.h"
#include "scene/resources/dynamic_font.h"

#define MAKE_FALLBACKS(m_name)          \
	m_name->add_fallback(FontArabic);   \
	m_name->add_fallback(FontHebrew);   \
	m_name->add_fallback(FontThai);     \
	m_name->add_fallback(FontHindi);    \
	m_name->add_fallback(FontJapanese); \
	m_name->add_fallback(FontFallback);

// the custom spacings might only work with Noto Sans
#define MAKE_DEFAULT_FONT(m_name, m_size)                       \
	Ref<DynamicFont> m_name;                                    \
	m_name.instance();                                          \
	m_name->set_size(m_size);                                   \
	if (CustomFont.is_valid()) {                                \
		m_name->set_font_data(CustomFont);                      \
		m_name->add_fallback(DefaultFont);                      \
	} else {                                                    \
		m_name->set_font_data(DefaultFont);                     \
	}                                                           \
	m_name->set_spacing(DynamicFont::SPACING_TOP, -EDSCALE);    \
	m_name->set_spacing(DynamicFont::SPACING_BOTTOM, -EDSCALE); \
	MAKE_FALLBACKS(m_name);

#define MAKE_BOLD_FONT(m_name, m_size)                          \
	Ref<DynamicFont> m_name;                                    \
	m_name.instance();                                          \
	m_name->set_size(m_size);                                   \
	if (CustomFontBold.is_valid()) {                            \
		m_name->set_font_data(CustomFontBold);                  \
		m_name->add_fallback(DefaultFontBold);                  \
	} else {                                                    \
		m_name->set_font_data(DefaultFontBold);                 \
	}                                                           \
	m_name->set_spacing(DynamicFont::SPACING_TOP, -EDSCALE);    \
	m_name->set_spacing(DynamicFont::SPACING_BOTTOM, -EDSCALE); \
	MAKE_FALLBACKS(m_name);

#define MAKE_ITALIC_FONT(m_name, m_size)                        \
	Ref<DynamicFont> m_name;                                    \
	m_name.instance();                                          \
	m_name->set_size(m_size);                                   \
	if (CustomFontItalic.is_valid()) {                          \
		m_name->set_font_data(CustomFontItalic);                \
		m_name->add_fallback(DefaultFontItalic);                \
	} else {                                                    \
		m_name->set_font_data(DefaultFontItalic);               \
	}                                                           \
	m_name->set_spacing(DynamicFont::SPACING_TOP, -EDSCALE);    \
	m_name->set_spacing(DynamicFont::SPACING_BOTTOM, -EDSCALE); \
	MAKE_FALLBACKS(m_name);

#define MAKE_BOLD_ITALIC_FONT(m_name, m_size)                   \
	Ref<DynamicFont> m_name;                                    \
	m_name.instance();                                          \
	m_name->set_size(m_size);                                   \
	if (CustomFontBoldItalic.is_valid()) {                      \
		m_name->set_font_data(CustomFontBoldItalic);            \
		m_name->add_fallback(DefaultFontBoldItalic);            \
	} else {                                                    \
		m_name->set_font_data(DefaultFontBoldItalic);           \
	}                                                           \
	m_name->set_spacing(DynamicFont::SPACING_TOP, -EDSCALE);    \
	m_name->set_spacing(DynamicFont::SPACING_BOTTOM, -EDSCALE); \
	MAKE_FALLBACKS(m_name);

#define MAKE_SOURCE_FONT(m_name, m_size)                        \
	Ref<DynamicFont> m_name;                                    \
	m_name.instance();                                          \
	m_name->set_size(m_size);                                   \
	if (CustomFontSource.is_valid()) {                          \
		m_name->set_font_data(CustomFontSource);                \
		m_name->add_fallback(dfmono);                           \
	} else {                                                    \
		m_name->set_font_data(dfmono);                          \
	}                                                           \
	m_name->set_spacing(DynamicFont::SPACING_TOP, -EDSCALE);    \
	m_name->set_spacing(DynamicFont::SPACING_BOTTOM, -EDSCALE); \
	MAKE_FALLBACKS(m_name);

void editor_register_fonts(Ref<Theme> p_theme) {
	DirAccess *dir = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);

	/* Custom font */

	bool font_antialiased = (bool)EditorSettings::get_singleton()->get("interface/editor/main_font_antialiased");
	DynamicFontData::Hinting font_hinting = (DynamicFontData::Hinting)(int)EditorSettings::get_singleton()->get("interface/editor/main_font_hinting");

	String custom_font_path = EditorSettings::get_singleton()->get("interface/editor/main_font");
	Ref<DynamicFontData> CustomFont;
	if (custom_font_path.length() > 0 && dir->file_exists(custom_font_path)) {
		CustomFont.instance();
		CustomFont->set_antialiased(font_antialiased);
		CustomFont->set_hinting(font_hinting);
		CustomFont->set_font_path(custom_font_path);
		CustomFont->set_force_autohinter(true);
	} else {
		EditorSettings::get_singleton()->set_manually("interface/editor/main_font", "");
	}

	/* Custom Bold font */

	String custom_font_path_bold = EditorSettings::get_singleton()->get("interface/editor/main_font_bold");
	Ref<DynamicFontData> CustomFontBold;
	if (custom_font_path_bold.length() > 0 && dir->file_exists(custom_font_path_bold)) {
		CustomFontBold.instance();
		CustomFontBold->set_antialiased(font_antialiased);
		CustomFontBold->set_hinting(font_hinting);
		CustomFontBold->set_font_path(custom_font_path_bold);
		CustomFontBold->set_force_autohinter(true);
	} else {
		EditorSettings::get_singleton()->set_manually("interface/editor/main_font_bold", "");
	}

	/* Custom Italic font */

	String custom_font_path_italic = EditorSettings::get_singleton()->get("interface/editor/main_font_italic");
	Ref<DynamicFontData> CustomFontItalic;
	if (custom_font_path_italic.length() > 0 && dir->file_exists(custom_font_path_italic)) {
		CustomFontItalic.instance();
		CustomFontItalic->set_antialiased(font_antialiased);
		CustomFontItalic->set_hinting(font_hinting);
		CustomFontItalic->set_font_path(custom_font_path_italic);
		CustomFontItalic->set_force_autohinter(true);
	} else {
		EditorSettings::get_singleton()->set_manually("interface/editor/main_font_italic", "");
	}

	/* Custom Bold Italic font */

	String custom_font_path_bold_italic = EditorSettings::get_singleton()->get("interface/editor/main_font_bold_italic");
	Ref<DynamicFontData> CustomFontBoldItalic;
	if (custom_font_path_bold_italic.length() > 0 && dir->file_exists(custom_font_path_bold_italic)) {
		CustomFontBoldItalic.instance();
		CustomFontBoldItalic->set_antialiased(font_antialiased);
		CustomFontBoldItalic->set_hinting(font_hinting);
		CustomFontBoldItalic->set_font_path(custom_font_path_bold_italic);
		CustomFontBoldItalic->set_force_autohinter(true);
	} else {
		EditorSettings::get_singleton()->set_manually("interface/editor/main_font_bold_italic", "");
	}

	/* Custom source code font */

	String custom_font_path_source = EditorSettings::get_singleton()->get("interface/editor/code_font");
	bool font_source_antialiased = (bool)EditorSettings::get_singleton()->get("interface/editor/code_font_antialiased");
	DynamicFontData::Hinting font_source_hinting = (DynamicFontData::Hinting)(int)EditorSettings::get_singleton()->get("interface/editor/code_font_hinting");
	Ref<DynamicFontData> CustomFontSource;
	if (custom_font_path_source.length() > 0 && dir->file_exists(custom_font_path_source)) {
		CustomFontSource.instance();
		CustomFontSource->set_antialiased(font_source_antialiased);
		CustomFontSource->set_hinting(font_source_hinting);
		CustomFontSource->set_font_path(custom_font_path_source);
	} else {
		EditorSettings::get_singleton()->set_manually("interface/editor/code_font", "");
	}

	memdelete(dir);

	// Unhinted fonts are bundled with the editor, as they are smaller compared to hinted fonts.
	// To provide hinting, all fonts (except Hack) use the FreeType autohinter to
	// generate hinting instructions on-the-fly.

	Ref<DynamicFontData> DefaultFont;
	DefaultFont.instance();
	DefaultFont->set_antialiased(font_antialiased);
	DefaultFont->set_hinting(font_hinting);
	DefaultFont->set_font_ptr(_font_NotoSans_Regular, _font_NotoSans_Regular_size);
	DefaultFont->set_force_autohinter(true);

	Ref<DynamicFontData> DefaultFontBold;
	DefaultFontBold.instance();
	DefaultFontBold->set_antialiased(font_antialiased);
	DefaultFontBold->set_hinting(font_hinting);
	DefaultFontBold->set_font_ptr(_font_NotoSans_Bold, _font_NotoSans_Bold_size);
	DefaultFontBold->set_force_autohinter(true);

	Ref<DynamicFontData> DefaultFontItalic;
	DefaultFontItalic.instance();
	DefaultFontItalic->set_antialiased(font_antialiased);
	DefaultFontItalic->set_hinting(font_hinting);
	DefaultFontItalic->set_font_ptr(_font_NotoSans_Italic, _font_NotoSans_Italic_size);
	DefaultFontItalic->set_force_autohinter(true);

	Ref<DynamicFontData> DefaultFontBoldItalic;
	DefaultFontBoldItalic.instance();
	DefaultFontBoldItalic->set_antialiased(font_antialiased);
	DefaultFontBoldItalic->set_hinting(font_hinting);
	DefaultFontBoldItalic->set_font_ptr(_font_NotoSans_BoldItalic, _font_NotoSans_BoldItalic_size);
	DefaultFontBoldItalic->set_force_autohinter(true);

	Ref<DynamicFontData> FontFallback;
	FontFallback.instance();
	FontFallback->set_antialiased(font_antialiased);
	FontFallback->set_hinting(font_hinting);
	FontFallback->set_font_ptr(_font_DroidSansFallback, _font_DroidSansFallback_size);
	FontFallback->set_force_autohinter(true);

	Ref<DynamicFontData> FontJapanese;
	FontJapanese.instance();
	FontJapanese->set_antialiased(font_antialiased);
	FontJapanese->set_hinting(font_hinting);
	FontJapanese->set_font_ptr(_font_DroidSansJapanese, _font_DroidSansJapanese_size);
	FontJapanese->set_force_autohinter(true);

	Ref<DynamicFontData> FontArabic;
	FontArabic.instance();
	FontArabic->set_antialiased(font_antialiased);
	FontArabic->set_hinting(font_hinting);
	FontArabic->set_font_ptr(_font_NotoNaskhArabicUI_Regular, _font_NotoNaskhArabicUI_Regular_size);
	FontArabic->set_force_autohinter(true);

	Ref<DynamicFontData> FontHebrew;
	FontHebrew.instance();
	FontHebrew->set_antialiased(font_antialiased);
	FontHebrew->set_hinting(font_hinting);
	FontHebrew->set_font_ptr(_font_NotoSansHebrew_Regular, _font_NotoSansHebrew_Regular_size);
	FontHebrew->set_force_autohinter(true);

	Ref<DynamicFontData> FontThai;
	FontThai.instance();
	FontThai->set_antialiased(font_antialiased);
	FontThai->set_hinting(font_hinting);
	FontThai->set_font_ptr(_font_NotoSansThaiUI_Regular, _font_NotoSansThaiUI_Regular_size);
	FontThai->set_force_autohinter(true);

	Ref<DynamicFontData> FontHindi;
	FontHindi.instance();
	FontHindi->set_antialiased(font_antialiased);
	FontHindi->set_hinting(font_hinting);
	FontHindi->set_font_ptr(_font_NotoSansDevanagariUI_Regular, _font_NotoSansDevanagariUI_Regular_size);
	FontHindi->set_force_autohinter(true);

	Ref<DynamicFontData> dfmono;
	dfmono.instance();
	dfmono->set_antialiased(font_source_antialiased);
	dfmono->set_hinting(font_source_hinting);
	dfmono->set_font_ptr(_font_Hack_Regular, _font_Hack_Regular_size);

	int default_font_size = int(EDITOR_GET("interface/editor/main_font_size")) * EDSCALE;

	// Default font
	MAKE_DEFAULT_FONT(df, default_font_size);
	p_theme->set_default_theme_font(df);

	// Bold font
	MAKE_BOLD_FONT(df_bold, default_font_size);
	p_theme->set_font("bold", "EditorFonts", df_bold);

	// Italic font
	MAKE_ITALIC_FONT(df_italic, default_font_size);
	p_theme->set_font("italic", "EditorFonts", df_italic);

	// Bold italic font
	MAKE_BOLD_ITALIC_FONT(df_bold_italic, default_font_size);
	p_theme->set_font("bold_italic", "EditorFonts", df_bold_italic);

	// Title font
	MAKE_BOLD_FONT(df_title, default_font_size + 2 * EDSCALE);
	p_theme->set_font("title", "EditorFonts", df_title);

	// Documentation fonts
	MAKE_DEFAULT_FONT(df_doc, int(EDITOR_GET("text_editor/help/help_font_size")) * EDSCALE);
	MAKE_BOLD_FONT(df_doc_bold, int(EDITOR_GET("text_editor/help/help_font_size")) * EDSCALE);
	MAKE_BOLD_FONT(df_doc_title, int(EDITOR_GET("text_editor/help/help_title_font_size")) * EDSCALE);
	MAKE_SOURCE_FONT(df_doc_code, int(EDITOR_GET("text_editor/help/help_source_font_size")) * EDSCALE);
	p_theme->set_font("doc", "EditorFonts", df_doc);
	p_theme->set_font("doc_bold", "EditorFonts", df_doc_bold);
	p_theme->set_font("doc_title", "EditorFonts", df_doc_title);
	p_theme->set_font("doc_source", "EditorFonts", df_doc_code);

	// Ruler font
	MAKE_DEFAULT_FONT(df_rulers, 8 * EDSCALE);
	p_theme->set_font("rulers", "EditorFonts", df_rulers);

	// Code font
	MAKE_SOURCE_FONT(df_code, int(EDITOR_GET("interface/editor/code_font_size")) * EDSCALE);
	p_theme->set_font("source", "EditorFonts", df_code);

	MAKE_SOURCE_FONT(df_expression, (int(EDITOR_GET("interface/editor/code_font_size")) - 1) * EDSCALE);
	p_theme->set_font("expression", "EditorFonts", df_expression);

	MAKE_SOURCE_FONT(df_output_code, int(EDITOR_GET("run/output/font_size")) * EDSCALE);
	p_theme->set_font("output_source", "EditorFonts", df_output_code);

	MAKE_SOURCE_FONT(df_text_editor_status_code, default_font_size);
	p_theme->set_font("status_source", "EditorFonts", df_text_editor_status_code);
}
