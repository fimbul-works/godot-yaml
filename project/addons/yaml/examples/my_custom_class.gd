class_name MyCustomClass extends Node

@export var string_val: String
@export var int_val: int
@export var float_val: float
@export var color_val: Color

func _init(p_string := "", p_int := 0, p_float := 0.0, p_color = Color.WHITE) -> void:
	string_val = p_string
	int_val = p_int
	float_val = p_float
	color_val = p_color

func hello():
	print(string_val)

static func from_dict(dict: Dictionary):
	var string_val: String = dict.get("string_val", "")
	var int_val: int = dict.get("int_val", 0)
	var float_val: float = dict.get("float_val", 0.0)
	var color_val: Color = dict.get("color_val", Color.RED)
	if !string_val:
		return YAMLResult.error("Missing String value")
	if !int_val:
		return YAMLResult.error("Missing integer value")
	if !float_val:
		return YAMLResult.error("Missing float value")
	if !color_val:
		return YAMLResult.error("Missing Color value")

	return MyCustomClass.new(
		string_val,
		int_val,
		float_val,
		color_val
	)

func to_dict() -> Dictionary:
	return {
		"string_val": string_val,
		"int_val": int_val,
		"float_val": float_val,
		"color_val": color_val,
	}
