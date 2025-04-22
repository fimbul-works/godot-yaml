@tool
class_name YAMLEditorStatusBar extends HBoxContainer

@export var editor: YAMLCodeEditor
@export var status_label: Label
@export var zoom_button: Button
@export var line_column_label: Label

var zoom_popup_menu: PopupMenu

func _ready() -> void:
	# Zoom popup menu
	zoom_popup_menu = PopupMenu.new()
	add_child(zoom_popup_menu)
	zoom_popup_menu.add_item("25 %", 0)
	zoom_popup_menu.add_item("50 %", 1)
	zoom_popup_menu.add_item("75 %", 2)
	zoom_popup_menu.add_item("100 %", 3)
	zoom_popup_menu.add_item("150 %", 4)
	zoom_popup_menu.add_item("200 %", 5)
	zoom_popup_menu.add_item("300 %", 6)
	zoom_popup_menu.id_pressed.connect(_on_zoom_popup_menu_id_pressed)
	zoom_button.pressed.connect(
		func():
			var global_rect := Rect2(get_global_mouse_position(), Vector2.ZERO)
			zoom_popup_menu.popup_on_parent(global_rect)
	)

	status_label.set("theme_override_constants/use_pixel_snap", true)

func _on_zoom_popup_menu_id_pressed(idx: int) -> void:
	match idx:
		0: editor.set_zoom(0.25)
		1: editor.set_zoom(0.5)
		2: editor.set_zoom(0.75)
		3: editor.set_zoom(1.0)
		4: editor.set_zoom(1.5)
		5: editor.set_zoom(2.0)
		6: editor.set_zoom(3.0)
	zoom_popup_menu.hide()

func set_status(text: String, color := Color.WHITE) -> void:
	status_label.text = text
	status_label.modulate = color

func set_line_column(line_column: Array[int]) -> void:
	var line := line_column[0]
	var col := line_column[1]
	line_column_label.text = "%d : %d" % [line, col]

func set_zoom_level(level: float) -> void:
	zoom_button.text = str(int(level * 100)) + " %"

func set_validation_result(result: YAMLResult) -> void:
	if !result.has_error():
		return set_status("")

	var error := result.get_error_message()
	var line := result.get_error_line()
	var col := result.get_error_column()
	var error_text := "Error at (%d, %d): %s" % [line, col, error] if line >= 0 else "Error: %s" % error

	var error_color: Color = EditorInterface.get_editor_settings().get_setting("text_editor/theme/highlighting/brace_mismatch_color")
	set_status(error_text, error_color)
