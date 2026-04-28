/**************************************************************************/
/*  particles_editor_plugin.h                                             */
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

#pragma once

#include "editor/plugins/editor_plugin.h"

class CheckBox;
class ConfirmationDialog;
class EditorHelpBit;
class HBoxContainer;
class ItemList;
class MenuButton;
class OptionButton;
class SceneTreeDialog;
class SpinBox;

class ParticlesEditorPlugin : public EditorPlugin {
	GDCLASS(ParticlesEditorPlugin, EditorPlugin);

private:
	enum {
		MENU_OPTION_CONVERT,
		MENU_SETUP_PARTICLES,
		MENU_RESTART,
	};

	enum SetupPreset {
		SETUP_PRESET_POINT,
		SETUP_PRESET_SMOKE,
		SETUP_PRESET_EXPLOSION,
		SETUP_PRESET_SPRINKLE,
		SETUP_PRESET_GLITTER,
		SETUP_PRESET_MAX,
	};

	PackedStringArray setup_preset_names = {
		TTR("Point"),
		TTR("Smoke"),
		TTR("Explosion"),
		TTR("Sprinkle"),
		TTR("Glitter"),
	};

	PackedStringArray setup_preset_descriptions = {
		TTR("A single point that stays in place."),
		TTR("Smoke slowly emitting in all directions."),
		TTR("An explosion that emits all particles at once, propelling particles upwards with gravity. Enable the Oneshot property for the explosion to occur only once."),
		TTR("Particles quickly sprinkling upwards."),
		TTR("Particles suspended in the air."),
	};

	SetupPreset setup_dialog_choice = SETUP_PRESET_POINT;

	HBoxContainer *toolbar = nullptr;
	MenuButton *menu = nullptr;

	ConfirmationDialog *setup_dialog = nullptr;
	ItemList *setup_dialog_choices = nullptr;
	EditorHelpBit *setup_dialog_help_bit = nullptr;
	CheckBox *setup_dialog_add_trails = nullptr;

protected:
	String handled_type;
	String conversion_option_name;

	Node *edited_node = nullptr;

	void _notification(int p_what);

	bool need_show_lifetime_dialog(SpinBox *p_seconds);
	virtual void _menu_callback(int p_idx);

	void _setup_particles_item_selected(int p_idx);
	void _setup_particles_item_activated(int p_idx);
	void _setup_particles_hide_requested();
	void _setup_particles_confirmed();

	virtual void _add_menu_options(PopupMenu *p_menu) {}
	virtual Node *_convert_particles() = 0;

public:
	virtual void edit(Object *p_object) override;
	virtual bool handles(Object *p_object) const override;
	virtual void make_visible(bool p_visible) override;

	ParticlesEditorPlugin();
};
