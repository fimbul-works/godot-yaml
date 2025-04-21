@tool
class_name YAMLEditorShortcuts

# This helper class registers editor shortcuts for the YAML editor

const SHORTCUTS = [
	{
		"name": "Save",
		"shortcut": KEY_MASK_CTRL | KEY_S,
		"callback": "_on_save_shortcut"
	},
	{
		"name": "Save As",
		"shortcut": KEY_MASK_CTRL | KEY_MASK_SHIFT | KEY_S,
		"callback": "_on_save_as_shortcut"
	},
	{
		"name": "Close File",
		"shortcut": KEY_MASK_CTRL | KEY_W,
		"callback": "_on_close_shortcut"
	},
	{
		"name": "New File",
		"shortcut": KEY_MASK_CTRL | KEY_N,
		"callback": "_on_new_shortcut"
	},
	{
		"name": "Open File",
		"shortcut": KEY_MASK_CTRL | KEY_O,
		"callback": "_on_open_shortcut"
	},
	{
		"name": "Validate YAML",
		"shortcut": KEY_F4,
		"callback": "_on_validate_shortcut"
	},
	{
		"name": "Undo",
		"shortcut": KEY_MASK_CTRL | KEY_Z,
		"callback": "_on_undo_shortcut"
	},
	{
		"name": "Redo",
		"shortcut": KEY_MASK_CTRL | KEY_MASK_SHIFT | KEY_Z,
		"callback": "_on_redo_shortcut"
	},
	{
		"name": "Cut",
		"shortcut": KEY_MASK_CTRL | KEY_X,
		"callback": "_on_cut_shortcut"
	},
	{
		"name": "Copy",
		"shortcut": KEY_MASK_CTRL | KEY_C,
		"callback": "_on_copy_shortcut"
	},
	{
		"name": "Paste",
		"shortcut": KEY_MASK_CTRL | KEY_V,
		"callback": "_on_paste_shortcut"
	},
	{
		"name": "Select All",
		"shortcut": KEY_MASK_CTRL | KEY_A,
		"callback": "_on_select_all_shortcut"
	},
	{
		"name": "Find",
		"shortcut": KEY_MASK_CTRL | KEY_F,
		"callback": "_on_find_shortcut"
	},
	{
		"name": "Find Next",
		"shortcut": KEY_F3,
		"callback": "_on_find_next_shortcut"
	},
	{
		"name": "Find Previous",
		"shortcut": KEY_MASK_SHIFT | KEY_F3,
		"callback": "_on_find_previous_shortcut"
	},
	{
		"name": "Replace",
		"shortcut": KEY_MASK_CTRL | KEY_R,
		"callback": "_on_replace_shortcut"
	},
	# Zoom shortcuts
	{
		"name": "Zoom In",
		"shortcut": KEY_MASK_CTRL | KEY_EQUAL,
		"callback": "_on_zoom_in_shortcut"
	},
	{
		"name": "Zoom In (Numpad)",
		"shortcut": KEY_MASK_CTRL | KEY_KP_ADD,
		"callback": "_on_zoom_in_shortcut"
	},
	{
		"name": "Zoom Out",
		"shortcut": KEY_MASK_CTRL | KEY_MINUS,
		"callback": "_on_zoom_out_shortcut"
	},
	{
		"name": "Zoom Out (Numpad)",
		"shortcut": KEY_MASK_CTRL | KEY_KP_SUBTRACT,
		"callback": "_on_zoom_out_shortcut"
	},
	{
		"name": "Reset Zoom",
		"shortcut": KEY_MASK_CTRL | KEY_0,
		"callback": "_on_zoom_reset_shortcut"
	},
	{
		"name": "Reset Zoom (Numpad)",
		"shortcut": KEY_MASK_CTRL | KEY_KP_0,
		"callback": "_on_zoom_reset_shortcut"
	}
]

static func register_shortcuts(editor_plugin: EditorPlugin, target_object: Object) -> void:
	# Create shortcut inputs for the YAML editor
	var editor_settings := editor_plugin.get_editor_interface().get_editor_settings()
	var shortcuts_settings := editor_settings.get_setting("shortcuts") if editor_settings.has_setting("shortcuts") else {}

	# Create a unique editor name for our shortcuts
	var editor_name := "YAML Editor"

	# Register each shortcut
	for shortcut_data in SHORTCUTS:
		var shortcut_name: String = "yaml_editor/" + shortcut_data.name.to_lower().replace(" ", "_")
		var input_event := InputEventKey.new()
		input_event.keycode = shortcut_data.shortcut

		# Create a shortcut
		var shortcut := Shortcut.new()
		shortcut.events = [input_event]

		# Register the shortcut with Godot's input map
		if not InputMap.has_action(shortcut_name):
			InputMap.add_action(shortcut_name)
			InputMap.action_add_event(shortcut_name, input_event)

	# Connect to the target object's _unhandled_key_input method if it exists
	if !target_object.has_method("_unhandled_key_input"):
		# Create connections for shortcuts if the target doesn't handle key input directly
		for shortcut_data in SHORTCUTS:
			var shortcut_name: String = "yaml_editor/" + shortcut_data.name.to_lower().replace(" ", "_")
			if target_object.has_method(shortcut_data.callback):
				InputMap.action_add_event(shortcut_name, InputEventAction.new())
				# Connect the shortcut action to the target's callback
				var root := editor_plugin.get_tree().root
				root.connect("input_event",
					func(event):
						if event is InputEventKey and event.pressed:
							if event.get_keycode_with_modifiers() == shortcut_data.shortcut:
								target_object.call(shortcut_data.callback)
								print("called a thing")
								root.get_viewport().set_input_as_handled()
				)

static func unregister_shortcuts() -> void:
	# Remove all registered shortcuts
	for shortcut_data in SHORTCUTS:
		var shortcut_name: String = "yaml_editor/" + shortcut_data.name.to_lower().replace(" ", "_")
		if InputMap.has_action(shortcut_name):
			InputMap.erase_action(shortcut_name)
