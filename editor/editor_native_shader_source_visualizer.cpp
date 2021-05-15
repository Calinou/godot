/*************************************************************************/
/*  editor_native_shader_source_visualizer.cpp                           */
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

#include "editor_native_shader_source_visualizer.h"

#include "editor/code_editor.h"
#include "editor/editor_scale.h"
#include "editor/editor_settings.h"
#include "scene/gui/text_edit.h"

void EditorNativeShaderSourceVisualizer::_load_theme_settings() {
	syntax_highlighter->set_number_color(EDITOR_GET("text_editor/highlighting/number_color"));
	syntax_highlighter->set_symbol_color(EDITOR_GET("text_editor/highlighting/symbol_color"));
	syntax_highlighter->set_function_color(EDITOR_GET("text_editor/highlighting/function_color"));
	syntax_highlighter->set_member_variable_color(EDITOR_GET("text_editor/highlighting/member_variable_color"));

	syntax_highlighter->clear_keyword_colors();

	List<String> keywords;
	ShaderLanguage::get_keyword_list(&keywords);
	const Color keyword_color = EDITOR_GET("text_editor/highlighting/keyword_color");
	const Color control_flow_keyword_color = EDITOR_GET("text_editor/highlighting/control_flow_keyword_color");

	for (List<String>::Element *E = keywords.front(); E; E = E->next()) {
		if (ShaderLanguage::is_control_flow_keyword(E->get())) {
			syntax_highlighter->add_keyword_color(E->get(), control_flow_keyword_color);
		} else {
			syntax_highlighter->add_keyword_color(E->get(), keyword_color);
		}
	}

	// Colorize comments.
	const Color comment_color = EDITOR_GET("text_editor/highlighting/comment_color");
	syntax_highlighter->clear_color_regions();
	syntax_highlighter->add_color_region("/*", "*/", comment_color, false);
	syntax_highlighter->add_color_region("//", "", comment_color, true);

	// Colorize preprocessor statements.
	const Color string_color = EDITOR_GET("text_editor/highlighting/string_color");
	syntax_highlighter->add_color_region("#", "", string_color, true);
}

void EditorNativeShaderSourceVisualizer::_inspect_shader(RID p_shader) {
	if (versions) {
		memdelete(versions);
		versions = nullptr;
	}

	RS::ShaderNativeSourceCode nsc = RS::get_singleton()->shader_get_native_source_code(p_shader);

	_load_theme_settings();

	versions = memnew(TabContainer);
	versions->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	versions->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	for (int i = 0; i < nsc.versions.size(); i++) {
		TabContainer *vtab = memnew(TabContainer);
		vtab->set_name("Version " + itos(i));
		vtab->set_v_size_flags(Control::SIZE_EXPAND_FILL);
		vtab->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		versions->add_child(vtab);
		for (int j = 0; j < nsc.versions[i].stages.size(); j++) {
			CodeEdit *code_edit = memnew(CodeEdit);
			code_edit->set_readonly(true);
			code_edit->set_syntax_highlighter(syntax_highlighter);
			code_edit->add_theme_font_override("font", get_theme_font("source", "EditorFonts"));
			code_edit->add_theme_font_size_override("font_size", get_theme_font_size("source_size", "EditorFonts"));
			code_edit->add_theme_constant_override("line_spacing", EDITOR_DEF("text_editor/theme/line_spacing", 6));

			// Set settings in a way similar to `editor/code_editor.cpp`.
			// Some settings don't make sense to be changed in a read-only context,
			// so these aren't set here.
			code_edit->set_highlight_all_occurrences(EditorSettings::get_singleton()->get("text_editor/highlighting/highlight_all_occurrences"));
			code_edit->set_indent_using_spaces(EditorSettings::get_singleton()->get("text_editor/indent/type"));
			code_edit->set_indent_size(EditorSettings::get_singleton()->get("text_editor/indent/size"));
			code_edit->set_auto_indent(EditorSettings::get_singleton()->get("text_editor/indent/auto_indent"));
			code_edit->set_draw_tabs(EditorSettings::get_singleton()->get("text_editor/indent/draw_tabs"));
			code_edit->set_draw_spaces(EditorSettings::get_singleton()->get("text_editor/indent/draw_spaces"));
			code_edit->set_smooth_scroll_enabled(EditorSettings::get_singleton()->get("text_editor/navigation/smooth_scrolling"));
			code_edit->set_v_scroll_speed(EditorSettings::get_singleton()->get("text_editor/navigation/v_scroll_speed"));
			code_edit->set_draw_minimap(EditorSettings::get_singleton()->get("text_editor/navigation/show_minimap"));
			code_edit->set_minimap_width((int)EditorSettings::get_singleton()->get("text_editor/navigation/minimap_width") * EDSCALE);
			code_edit->set_draw_line_numbers(EditorSettings::get_singleton()->get("text_editor/appearance/show_line_numbers"));
			code_edit->set_line_numbers_zero_padded(EditorSettings::get_singleton()->get("text_editor/appearance/line_numbers_zero_padded"));
			code_edit->set_hiding_enabled(EditorSettings::get_singleton()->get("text_editor/appearance/code_folding"));
			code_edit->set_draw_fold_gutter(EditorSettings::get_singleton()->get("text_editor/appearance/code_folding"));
			code_edit->set_wrap_enabled(EditorSettings::get_singleton()->get("text_editor/appearance/word_wrap"));
			code_edit->set_scroll_pass_end_of_file(EditorSettings::get_singleton()->get("text_editor/cursor/scroll_past_end_of_file"));
			code_edit->cursor_set_block_mode(EditorSettings::get_singleton()->get("text_editor/cursor/block_caret"));
			code_edit->cursor_set_blink_enabled(EditorSettings::get_singleton()->get("text_editor/cursor/caret_blink"));
			code_edit->cursor_set_blink_speed(EditorSettings::get_singleton()->get("text_editor/cursor/caret_blink_speed"));

			code_edit->set_name(nsc.versions[i].stages[j].name);
			code_edit->set_text(nsc.versions[i].stages[j].code);
			code_edit->set_v_size_flags(Control::SIZE_EXPAND_FILL);
			code_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
			vtab->add_child(code_edit);
		}
	}
	add_child(versions);
	popup_centered_ratio();
}

void EditorNativeShaderSourceVisualizer::_bind_methods() {
	ClassDB::bind_method("_inspect_shader", &EditorNativeShaderSourceVisualizer::_inspect_shader);
}

EditorNativeShaderSourceVisualizer::EditorNativeShaderSourceVisualizer() {
	syntax_highlighter.instance();

	add_to_group("_native_shader_source_visualizer");
	set_title(TTR("Native Shader Source Inspector"));
}
