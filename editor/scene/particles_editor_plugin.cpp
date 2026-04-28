/**************************************************************************/
/*  particles_editor_plugin.cpp                                           */
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

#include "particles_editor_plugin.h"

#include "core/object/callable_mp.h"
#include "editor/doc/editor_help.h"
#include "editor/docks/scene_tree_dock.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/settings/editor_settings.h"
#include "editor/themes/editor_scale.h"
#include "scene/3d/cpu_particles_3d.h"
#include "scene/3d/gpu_particles_3d.h"
#include "scene/gui/box_container.h"
#include "scene/gui/item_list.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/spin_box.h"
#include "scene/resources/3d/primitive_meshes.h"
#include "scene/resources/gradient_texture.h"
#include "scene/resources/particle_process_material.h"

void ParticlesEditorPlugin::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (handled_type.ends_with("2D")) {
				add_control_to_container(CONTAINER_CANVAS_EDITOR_MENU, toolbar);
			} else if (handled_type.ends_with("3D")) {
				add_control_to_container(CONTAINER_SPATIAL_EDITOR_MENU, toolbar);
			} else {
				DEV_ASSERT(false);
			}

			menu->set_button_icon(menu->get_editor_theme_icon(handled_type));
			menu->set_text(handled_type);

			PopupMenu *popup = menu->get_popup();
			popup->add_shortcut(ED_SHORTCUT("particles/setup_particles", TTRC("Setup Particles"), KeyModifierMask::CTRL | KeyModifierMask::ALT | Key::R), MENU_SETUP_PARTICLES);
			popup->add_separator();
			popup->add_shortcut(ED_SHORTCUT("particles/restart_emission", TTRC("Restart Emission"), KeyModifierMask::CTRL | Key::R), MENU_RESTART);
			_add_menu_options(popup);
			popup->add_item(conversion_option_name, MENU_OPTION_CONVERT);
		} break;
	}
}

void ParticlesEditorPlugin::_setup_particles_item_selected(int p_idx) {
	setup_dialog_choice = SetupPreset(p_idx);
	setup_dialog_help_bit->set_custom_text(setup_preset_names[p_idx], "", setup_preset_descriptions[p_idx]);
}

void ParticlesEditorPlugin::_setup_particles_item_activated(int p_idx) {
	setup_dialog_choice = SetupPreset(p_idx);
	setup_dialog->hide();
	_setup_particles_confirmed();
}

void ParticlesEditorPlugin::_setup_particles_hide_requested() {
	setup_dialog->hide();
}

void ParticlesEditorPlugin::_setup_particles_confirmed() {
	GPUParticles3D *gpu_particles_3d = memnew(GPUParticles3D);

	Ref<ParticleProcessMaterial> process_material;
	process_material.instantiate();

	process_material->set_direction(Vector3(0, 1, 0));
	process_material->set_collision_mode(ParticleProcessMaterial::COLLISION_RIGID);
	process_material->set_collision_friction(0.1);
	process_material->set_collision_bounce(0.5);
	process_material->set_collision_use_scale(true);

	Ref<GradientTexture2D> color_ramp_texture;
	color_ramp_texture.instantiate();
	Ref<Gradient> color_ramp;
	color_ramp.instantiate();
	// Make particles fade in quickly, then fade out slowly.
	color_ramp->set_color(0, Color(0, 0, 0, 0));
	color_ramp->set_color(0.1, Color(1, 1, 1));
	color_ramp->set_color(1, Color(0, 0, 0, 0));
	color_ramp_texture->set_gradient(color_ramp);
	process_material->set_color_ramp(color_ramp_texture);

	Ref<CurveTexture> scale_curve_texture;
	scale_curve_texture.instantiate();
	Ref<Curve> scale_curve;
	scale_curve.instantiate();
	// Make particles grow quickly in size, then shrink slowly.
	scale_curve->add_point(Vector2(0.0, 0.0));
	scale_curve->add_point(Vector2(0.1, 1.0));
	scale_curve->add_point(Vector2(1.0, 0.0));
	scale_curve_texture->set_curve(scale_curve);
	process_material->set_param_texture(ParticleProcessMaterial::PARAM_SCALE, scale_curve_texture);

	switch (setup_dialog_choice) {
		case SETUP_PRESET_POINT:
			gpu_particles_3d->set_amount(1);
			process_material->set_gravity(Vector3(0, 0, 0));
			break;
		case SETUP_PRESET_SMOKE:
			process_material->set_gravity(Vector3(0, 0, 0));
			process_material->set_spread(180);
			process_material->set_param(ParticleProcessMaterial::PARAM_INITIAL_LINEAR_VELOCITY, Vector2(1.0, 1.0));
			process_material->set_param(ParticleProcessMaterial::PARAM_DAMPING, Vector2(0.5, 0.5));
			break;
		case SETUP_PRESET_EXPLOSION:
			gpu_particles_3d->set_explosiveness_ratio(1.0);
			process_material->set_spread(90);
			process_material->set_param(ParticleProcessMaterial::PARAM_INITIAL_LINEAR_VELOCITY, Vector2(2.5, 2.5));
			break;
		case SETUP_PRESET_SPRINKLE:
			process_material->set_spread(15);
			process_material->set_direction(Vector3(0, 1, 0));
			process_material->set_gravity(Vector3(0, 5, 0));
			process_material->set_param(ParticleProcessMaterial::PARAM_INITIAL_LINEAR_VELOCITY, Vector2(1.0, 1.0));
			break;
		case SETUP_PRESET_GLITTER:
			gpu_particles_3d->set_lifetime(3.0);
			gpu_particles_3d->set_amount(180);
			process_material->set_emission_shape(ParticleProcessMaterial::EMISSION_SHAPE_SPHERE);
			process_material->set_emission_sphere_radius(2.0);
			process_material->set_gravity(Vector3(0, 0, 0));
			process_material->set_spread(180);
			process_material->set_param(ParticleProcessMaterial::PARAM_INITIAL_LINEAR_VELOCITY, Vector2(0.1, 0.1));
			break;
		case SETUP_PRESET_MAX:
			// Internal value, skip.
			break;
	}

	gpu_particles_3d->set_amount(60);
	gpu_particles_3d->set_process_material(process_material);

	// Create material to display billboarded texture that follows the color and scale ramps
	// defined in ParticleProcessMaterial.
	Ref<StandardMaterial3D> material;
	material.instantiate();
	material->set_shading_mode(BaseMaterial3D::SHADING_MODE_UNSHADED);
	material->set_transparency(BaseMaterial3D::TRANSPARENCY_ALPHA);
	material->set_blend_mode(BaseMaterial3D::BLEND_MODE_PREMULT_ALPHA);
	material->set_flag(BaseMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
	material->set_flag(BaseMaterial3D::FLAG_BILLBOARD_KEEP_SCALE, true);
	material->set_billboard_mode(BaseMaterial3D::BILLBOARD_PARTICLES);

	// Assign a smooth circle texture.
	Ref<GradientTexture2D> texture;
	texture.instantiate();
	// Reduce grainy appearance at a distance by using a low resolution, as GradientTexture can't generate mipmaps.
	texture->set_width(32);
	texture->set_height(32);
	texture->set_fill(GradientTexture2D::FILL_RADIAL);
	texture->set_fill_from(Vector2(0.5, 0.5));
	texture->set_fill_to(Vector2(0.5, 0.01));
	Ref<Gradient> gradient;
	gradient.instantiate();
	gradient->set_color(0, Color(1, 1, 1));
	gradient->set_offset(0, 0.9);
	gradient->set_color(1, Color(0, 0, 0, 0));
	texture->set_gradient(gradient);
	material->set_texture(BaseMaterial3D::TEXTURE_ALBEDO, texture);

	if (setup_dialog_add_trails->is_pressed()) {
		gpu_particles_3d->set_trail_enabled(true);
		gpu_particles_3d->set_trail_lifetime(0.1);
		process_material->set_particle_flag(ParticleProcessMaterial::PARTICLE_FLAG_ALIGN_Y_TO_VELOCITY, true);
		material->set_flag(BaseMaterial3D::FLAG_PARTICLE_TRAILS_MODE, true);
		material->set_billboard_mode(BaseMaterial3D::BILLBOARD_DISABLED);
		// Improve visibility for trails by making the backface visible.
		material->set_cull_mode(BaseMaterial3D::CULL_DISABLED);

		Ref<RibbonTrailMesh> ribbon_trail_mesh = memnew(RibbonTrailMesh);
		//ribbon_trail_mesh->set_shape(RibbonTrailMesh::SHAPE_FLAT);
		ribbon_trail_mesh->set_size(0.025);
		ribbon_trail_mesh->set_material(material);
		gpu_particles_3d->set_draw_pass_mesh(0, ribbon_trail_mesh);
	} else {
		Ref<QuadMesh> quad_mesh = memnew(QuadMesh);
		quad_mesh->set_size(Vector2(0.025, 0.025));
		quad_mesh->set_material(material);
		gpu_particles_3d->set_draw_pass_mesh(0, quad_mesh);
	}

	Node *new_particles;
	if (handled_type.begins_with("CPU")) {
		CPUParticles3D *cpu_particles_3d = memnew(CPUParticles3D);
		cpu_particles_3d->convert_from_particles(gpu_particles_3d);
		// CPUParticles doesn't support interpolation; simulate particles every frame
		// to avoid jittery visuals and match default CPUParticles behavior.
		cpu_particles_3d->set_fixed_fps(0);
		new_particles = cpu_particles_3d;
	} else {
		new_particles = gpu_particles_3d;
	}

	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	ur->create_action(vformat(TTR("Setup Particles for \"%s\" with preset: %s"), edited_node->get_name(), setup_dialog_choices->get_item_text(setup_dialog_choice)), UndoRedo::MERGE_DISABLE, edited_node);
	SceneTreeDock::get_singleton()->replace_node(edited_node, new_particles);
	ur->commit_action(false);
}

bool ParticlesEditorPlugin::need_show_lifetime_dialog(SpinBox *p_seconds) {
	// Add one second to the default generation lifetime, since the progress is updated every second.
	p_seconds->set_value(MAX(1.0, std::trunc(edited_node->get("lifetime").operator double()) + 1.0));

	if (p_seconds->get_value() >= 11.0 + CMP_EPSILON) {
		// Only pop up the time dialog if the particle's lifetime is long enough to warrant shortening it.
		return true;
	} else {
		// Generate the visibility rect/AABB immediately.
		return false;
	}
}

void ParticlesEditorPlugin::_menu_callback(int p_idx) {
	switch (p_idx) {
		case MENU_OPTION_CONVERT: {
			Node *converted_node = _convert_particles();

			EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
			ur->create_action(conversion_option_name, UndoRedo::MERGE_DISABLE, edited_node);
			SceneTreeDock::get_singleton()->replace_node(edited_node, converted_node);
			ur->commit_action(false);
		} break;

		case MENU_SETUP_PARTICLES: {
			setup_dialog->popup_centered_clamped(Size2i(500, 0) * EDSCALE);
			// Select first item in the list automatically and grab focus,
			// so you can choose with the keyboard and press Enter.
			setup_dialog_choices->select(0);
			// Required to update the description shown at the bottom of the dialog.
			_setup_particles_item_selected(0);
			setup_dialog_choices->grab_focus();
		} break;

		case MENU_RESTART: {
			edited_node->call("restart");
		} break;
	}
}

void ParticlesEditorPlugin::edit(Object *p_object) {
	edited_node = Object::cast_to<Node>(p_object);
}

bool ParticlesEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class(handled_type);
}

void ParticlesEditorPlugin::make_visible(bool p_visible) {
	toolbar->set_visible(p_visible);
}

ParticlesEditorPlugin::ParticlesEditorPlugin() {
	toolbar = memnew(HBoxContainer);
	toolbar->hide();

	menu = memnew(MenuButton);
	menu->set_switch_on_hover(true);
	menu->set_flat(false);
	menu->set_theme_type_variation("FlatMenuButtonNoIconTint");
	toolbar->add_child(menu);
	menu->get_popup()->connect(SceneStringName(id_pressed), callable_mp(this, &ParticlesEditorPlugin::_menu_callback));

	setup_dialog = memnew(ConfirmationDialog);
	toolbar->add_child(setup_dialog);
	setup_dialog->set_title(TTR("Choose a Particles Type"));
	setup_dialog->connect(SceneStringName(confirmed), callable_mp(this, &ParticlesEditorPlugin::_setup_particles_confirmed));

	VBoxContainer *setup_dialog_vbc = memnew(VBoxContainer);
	setup_dialog->add_child(setup_dialog_vbc);

	setup_dialog_choices = memnew(ItemList);
	setup_dialog_choices->set_custom_minimum_size(Size2(0, 250) * EDSCALE);
	// Items here must match the order of the SetupPreset enum.
	for (int i = 0; i < SETUP_PRESET_MAX; i++) {
		setup_dialog_choices->add_item(setup_preset_names[i]);
	}
	setup_dialog_choices->connect(SceneStringName(item_selected), callable_mp(this, &ParticlesEditorPlugin::_setup_particles_item_selected));
	setup_dialog_choices->connect(SceneStringName(item_activated), callable_mp(this, &ParticlesEditorPlugin::_setup_particles_item_activated));
	setup_dialog_vbc->add_child(setup_dialog_choices);

	setup_dialog_help_bit = memnew(EditorHelpBit);
	setup_dialog_help_bit->set_content_height_limits(64 * EDSCALE, 64 * EDSCALE);
	setup_dialog_help_bit->connect("request_hide", callable_mp(this, &ParticlesEditorPlugin::_setup_particles_hide_requested));
	setup_dialog_vbc->add_margin_child(TTR("Description:"), setup_dialog_help_bit);

	setup_dialog_add_trails = memnew(CheckBox);
	if (handled_type.begins_with("CPU")) {
		setup_dialog_add_trails->set_text(TTR("Enable Trails") + " " + TTR("(not supported with CPUParticles)"));
		setup_dialog_add_trails->set_disabled(true);
	} else {
		setup_dialog_add_trails->set_text(TTR("Enable Trails"));
	}
	setup_dialog_vbc->add_child(setup_dialog_add_trails);
}
