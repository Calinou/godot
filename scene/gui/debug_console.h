/**************************************************************************/
/*  debug_console.h                                                       */
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

#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H

#include "box_container.h"
#include "button.h"
#include "label.h"
#include "line_edit.h"
#include "margin_container.h"
#include "panel_container.h"
#include "rich_text_label.h"
#include "scene/main/canvas_layer.h"

class DebugConsole : public CanvasLayer {
	GDCLASS(DebugConsole, CanvasLayer);

	RichTextLabel *log;
	LineEdit *input;

	// The display mode is always overridden to `DEBUG_CONSOLE_DISPLAY_MODE_COMPACT` on initialization.
	SceneTree::DebugConsoleDisplayMode display_mode = SceneTree::DEBUG_CONSOLE_DISPLAY_MODE_HIDDEN;

	Ref<Gradient> frame_time_gradient;

protected:
	void _notification(int p_what);

public:
	void set_display_mode(SceneTree::DebugConsoleDisplayMode p_mode);
	SceneTree::DebugConsoleDisplayMode get_display_mode() const;

	DebugConsole();
};

#endif // DEBUG_CONSOLE_H
