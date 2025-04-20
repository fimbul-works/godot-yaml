@tool
class_name YAMLEditorStatusPanel extends HBoxContainer

@export var status_label: Label
@export var line_column_label: Label

var ERROR_COLOR := Color.html("#ff6f6f")

func set_status(text: String, color := Color.WHITE) -> void:
	status_label.text = text
	status_label.modulate = color

func set_line_column(line_column: Array[int]) -> void:
	var line := line_column[0]
	var col := line_column[1]
	line_column_label.text = "%d : %d" % [line, col]

func set_validation_result(result: YAMLResult) -> void:
	if !result.has_error():
		return set_status("")

	var error := result.get_error_message()
	var line := result.get_error_line()
	var col := result.get_error_column()
	var error_status := "Error at (%d, %d): %s" % [line, col, error] if line >= 0 else "Error: %s" % error
	set_status(error_status, ERROR_COLOR)
