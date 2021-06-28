/*************************************************************************/
/*  voxel_gi_editor_plugin.cpp                                           */
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

#include "voxel_gi_editor_plugin.h"

void VoxelGIEditorPlugin::_bake() {
	if (voxel_gi) {
		if (voxel_gi->get_probe_data().is_null()) {
			String path = get_tree()->get_edited_scene_root()->get_filename();
			if (path == String()) {
				path = "res://" + voxel_gi->get_name() + "_data.res";
			} else {
				String ext = path.get_extension();
				path = path.get_basename() + "." + voxel_gi->get_name() + "_data.res";
			}
			probe_file->set_current_path(path);
			probe_file->popup_file_dialog();
			return;
		}
		voxel_gi->bake();
	}
}

void VoxelGIEditorPlugin::edit(Object *p_object) {
	VoxelGI *s = Object::cast_to<VoxelGI>(p_object);
	if (!s) {
		return;
	}

	voxel_gi = s;
}

bool VoxelGIEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class("VoxelGI");
}

void VoxelGIEditorPlugin::_notification(int p_what) {
	if (p_what == NOTIFICATION_PROCESS) {
		if (!voxel_gi) {
			return;
		}

		const Vector3i size = voxel_gi->get_estimated_cell_size();
		String text = vformat(String::utf8("%d × %d × %d"), size.x, size.y, size.z);
		int data_size = 4;
		// if (GLOBAL_GET("rendering/quality/voxel_gi/anisotropic")) {
		// 	data_size += 4;
		// }
		const double size_mb = size.x * size.y * size.z * data_size / (1024.0 * 1024.0);
		text += " - " + vformat(TTR("VRAM Size: %s MB"), String::num(size_mb, 2));

		if (bake_info->get_text() == text) {
			return;
		}

		// Color the label depending on the estimated performance level.
		Color color;
		if (size_mb <= 16.0 + CMP_EPSILON) {
			// Fast.
			color = bake_info->get_theme_color("success_color", "Editor");
		} else if (size_mb <= 64.0 + CMP_EPSILON) {
			// Medium.
			color = bake_info->get_theme_color("warning_color", "Editor");
		} else {
			// Slow.
			color = bake_info->get_theme_color("error_color", "Editor");
		}
		bake_info->add_theme_color_override("font_color", color);

		bake_info->set_text(text);
	}
}

void VoxelGIEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		bake_hb->show();
		set_process(true);
	} else {
		bake_hb->hide();
		set_process(false);
	}
}

EditorProgress *VoxelGIEditorPlugin::tmp_progress = nullptr;

void VoxelGIEditorPlugin::bake_func_begin(int p_steps) {
	ERR_FAIL_COND(tmp_progress != nullptr);

	tmp_progress = memnew(EditorProgress("bake_gi", TTR("Bake GI Probe"), p_steps));
}

void VoxelGIEditorPlugin::bake_func_step(int p_step, const String &p_description) {
	ERR_FAIL_COND(tmp_progress == nullptr);
	tmp_progress->step(p_description, p_step, false);
}

void VoxelGIEditorPlugin::bake_func_end() {
	ERR_FAIL_COND(tmp_progress == nullptr);
	memdelete(tmp_progress);
	tmp_progress = nullptr;
}

void VoxelGIEditorPlugin::_voxel_gi_save_path_and_bake(const String &p_path) {
	probe_file->hide();
	if (voxel_gi) {
		voxel_gi->bake();
		ERR_FAIL_COND(voxel_gi->get_probe_data().is_null());
		ResourceSaver::save(p_path, voxel_gi->get_probe_data(), ResourceSaver::FLAG_CHANGE_PATH);
	}
}

void VoxelGIEditorPlugin::_show_fit_extents_dialog() {
	fit_extents_dialog->popup_scenetree_dialog();
}

void VoxelGIEditorPlugin::_fit_extents_dialog_selected(const NodePath &p_path) {
	Node *sel = get_node(p_path);
	if (!sel) {
		return;
	}

	if (!sel->is_class("Node3D")) {
		// The SceneTreeDialog is configured not to allow nodes that don't inherit from Node3D,
		// but keep this as a defensive measure anyway.
		EditorNode::get_singleton()->show_warning(vformat(TTR("\"%s\" doesn't inherit from Node3D."), sel->get_name()), TTR("Error"));
		return;
	}

	Vector<AABB> transformed_aabbs;
	bool has_node_with_correct_gi_mode = false;

	// The selected node is a GeometryInstance3D.
	GeometryInstance3D *gi = Object::cast_to<GeometryInstance3D>(sel);
	if (gi) {
		if (gi->get_gi_mode() == GeometryInstance3D::GI_MODE_BAKED || gi->get_gi_mode() == GeometryInstance3D::GI_MODE_DYNAMIC) {
			transformed_aabbs.push_back(gi->get_transformed_aabb());
			print_line("Root AABB:" + String(gi->get_transformed_aabb()));
			has_node_with_correct_gi_mode = true;
		}
	}

	for (int i = 0; i < sel->get_child_count(); i++) {
		GeometryInstance3D *gi = Object::cast_to<GeometryInstance3D>(sel->get_child(i));
		if (gi) {
			if (gi->get_gi_mode() == GeometryInstance3D::GI_MODE_BAKED || gi->get_gi_mode() == GeometryInstance3D::GI_MODE_DYNAMIC) {
				transformed_aabbs.push_back(gi->get_transformed_aabb());
				print_line("Child AABB:" + String(gi->get_transformed_aabb()));
				has_node_with_correct_gi_mode = true;
			}
		}
	}

	if (!has_node_with_correct_gi_mode) {
		EditorNode::get_singleton()->show_warning(
				vformat(TTR("\"%s\" or its immediate children do not have their GI Mode property set to Baked or Dynamic.\nOnly nodes with their GI Mode set to Baked or Dynamic are taken into account when fitting VoxelGI extents.\nTo solve this, set \"%s\"'s or its immediate children's GI Mode to Baked or Dynamic."), sel->get_name(), sel->get_name()), TTR("Error"));
		return;
	}

	// Calculate the AABB that encloses all the child nodes' AABBs.
	AABB transformed_enclosing_aabb;
	for (int i = 0; i < transformed_aabbs.size(); i++) {
		transformed_enclosing_aabb.merge_with(transformed_aabbs[i]);
	}
	print_line("Enclosing AABB: " + String(transformed_enclosing_aabb));

	if (transformed_enclosing_aabb.size.is_equal_approx(Vector3())) {
		EditorNode::get_singleton()->show_warning(
				vformat(TTR("\"%s\" has an empty axis-aligned bounding box (its size is `Vector3(0, 0, 0)`).\nThe VoxelGI's extents were left unchanged."), sel->get_name()), TTR("Error"));
		return;
	}

	Transform3D transform;
	// VoxelGI extents extend from the node's origin.
	transform.origin = transformed_enclosing_aabb.position + transformed_enclosing_aabb.size * 0.5;

	UndoRedo *ur = EditorNode::get_singleton()->get_undo_redo();
	ur->create_action(TTR("Fit VoxelGI Extents"));
	ur->add_do_method(voxel_gi, "set_global_transform", transform);
	ur->add_do_method(voxel_gi, "set_extents", transformed_enclosing_aabb.size * 0.5);
	ur->add_undo_method(voxel_gi, "set_global_transform", voxel_gi->get_transform());
	ur->add_undo_method(voxel_gi, "set_extents", voxel_gi->get_extents());
	ur->commit_action();
}

void VoxelGIEditorPlugin::_bind_methods() {
}

VoxelGIEditorPlugin::VoxelGIEditorPlugin(EditorNode *p_node) {
	editor = p_node;
	bake_hb = memnew(HBoxContainer);
	bake_hb->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	bake_hb->hide();

	fit_extents = memnew(Button);
	fit_extents->set_flat(true);
	fit_extents->set_icon(editor->get_gui_base()->get_theme_icon("RoomBounds", "EditorIcons"));
	fit_extents->set_text(TTR("Fit"));
	fit_extents->set_tooltip(
			TTR("Fits the VoxelGI's extents to the node selected in the dialog (and its immediate children, if any).\nOnly nodes inherting from GeometryInstance3D with their GI Mode set to Baked or Dynamic are taken into account.\nYou can create a Node3D and place other nodes as children to group several nodes together."));
	fit_extents->connect("pressed", callable_mp(this, &VoxelGIEditorPlugin::_show_fit_extents_dialog));
	bake_hb->add_child(fit_extents);

	bake = memnew(Button);
	bake->set_flat(true);
	bake->set_icon(editor->get_gui_base()->get_theme_icon("Bake", "EditorIcons"));
	bake->set_text(TTR("Bake GI Probe"));
	bake->connect("pressed", callable_mp(this, &VoxelGIEditorPlugin::_bake));
	bake_hb->add_child(bake);

	bake_info = memnew(Label);
	bake_info->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	bake_info->set_clip_text(true);
	bake_hb->add_child(bake_info);

	add_control_to_container(CONTAINER_SPATIAL_EDITOR_MENU, bake_hb);
	voxel_gi = nullptr;
	probe_file = memnew(EditorFileDialog);
	probe_file->set_file_mode(EditorFileDialog::FILE_MODE_SAVE_FILE);
	probe_file->add_filter("*.res");
	probe_file->connect("file_selected", callable_mp(this, &VoxelGIEditorPlugin::_voxel_gi_save_path_and_bake));
	get_editor_interface()->get_base_control()->add_child(probe_file);
	probe_file->set_title(TTR("Select path for VoxelGI Data File"));

	fit_extents_dialog = memnew(SceneTreeDialog);
	fit_extents_dialog->set_title(TTR("Fit VoxelGI Extents to Selected Node(s)"));
	Vector<StringName> types;
	types.push_back("Node3D");
	fit_extents_dialog->get_scene_tree()->set_valid_types(types);
	add_child(fit_extents_dialog);
	fit_extents_dialog->connect("selected", callable_mp(this, &VoxelGIEditorPlugin::_fit_extents_dialog_selected));

	VoxelGI::bake_begin_function = bake_func_begin;
	VoxelGI::bake_step_function = bake_func_step;
	VoxelGI::bake_end_function = bake_func_end;
}

VoxelGIEditorPlugin::~VoxelGIEditorPlugin() {
}
