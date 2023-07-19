/**************************************************************************/
/*  debug_console.cpp                                                     */
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

#include "debug_console.h"

#include "core/config/engine.h"
#include "label.h"

void DebugConsole::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PROCESS:
			const float fps = Engine::get_singleton()->get_frames_per_second();
			frames_per_second->set_text(vformat("%d FPS", fps));
			frames_per_second->set_modulate(frame_time_gradient->get_color_at_offset(Math::remap(fps, GRAPH_MIN_FPS, GRAPH_MAX_FPS, 0.0f, 1.0f)));
			break;
	}
}

void DebugConsole::set_display_mode(SceneTree::DebugConsoleDisplayMode p_mode) {
	if (p_mode == display_mode) {
		return;
	}

	display_mode = p_mode;

	switch (display_mode) {
		case SceneTree::DEBUG_CONSOLE_DISPLAY_MODE_COMPACT:
			print_line("Compact");
			break;
		case SceneTree::DEBUG_CONSOLE_DISPLAY_MODE_DETAILED:
			print_line("Detailed");
			break;
		default:
			// We don't handle `DEBUG_CONSOLE_DISPLAY_MODE_HIDDEN` as the node is freed immediately when it's set.
			break;
	}
}

SceneTree::DebugConsoleDisplayMode DebugConsole::get_display_mode() const {
	return display_mode;
}

DebugConsole::DebugConsole() {
	// Set on the highest layer, so that nothing else can draw on top.
	set_layer(128);

	// Make sure the compact setter is run, as this is the default mode when the debug menu is first created.
	set_display_mode(SceneTree::DebugConsoleDisplayMode::DEBUG_CONSOLE_DISPLAY_MODE_COMPACT);

	set_process(true);

	MarginContainer *margin_container = memnew(MarginContainer);
	margin_container->set_anchors_preset(Control::PRESET_FULL_RECT);
	const float default_base_scale = margin_container->get_theme_default_base_scale();
	const float default_font_size = margin_container->get_theme_default_font_size();
	margin_container->add_theme_constant_override("margin_top", Math::round(10 * default_base_scale));
	margin_container->add_theme_constant_override("margin_right", Math::round(10 * default_base_scale));
	margin_container->add_theme_constant_override("margin_bottom", Math::round(10 * default_base_scale));
	margin_container->add_theme_constant_override("margin_left", Math::round(10 * default_base_scale));
	add_child(margin_container);

	VBoxContainer *vbox_container = memnew(VBoxContainer);
	vbox_container->add_theme_constant_override("separation", Math::round(10 * default_base_scale));
	margin_container->add_child(vbox_container);

	frame_time_gradient.instantiate();
	frame_time_gradient->set_color(0, Color(1, 0, 0));
	frame_time_gradient->set_color(1, Color(0, 1, 1));
	frame_time_gradient->add_point(0.3333, Color(1, 1, 0));
	frame_time_gradient->add_point(0.6667, Color(0, 1, 0));

	frames_per_second = memnew(Label);
	frames_per_second->add_theme_font_size_override("font_size", Math::round(1.125 * default_font_size));
	frames_per_second->add_theme_color_override("font_outline_color", Color(0, 0, 0));
	frames_per_second->add_theme_constant_override("outline_size", Math::round(5 * default_base_scale));
	vbox_container->add_child(frames_per_second);
}
