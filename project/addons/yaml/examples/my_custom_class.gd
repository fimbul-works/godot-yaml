class_name MyCustomClass extends Node

@export var string_val: String
@export var int_val: int
@export var float_val: float

func _init(p_string: String = "", p_int: int = 0, p_float: float = 0) -> void:
	string_val = p_string
	int_val = p_int
	float_val = p_float

func hello():
	print(string_val)

static func from_dict(dict: Dictionary) -> MyCustomClass:
	var obj := MyCustomClass.new(
		dict.get("string_val"),
		dict.get("int_val"),
		dict.get("float_val")
	)
	return obj

func to_dict() -> Dictionary:
	return {
		"string_val": string_val,
		"int_val": int_val,
		"float_val": float_val,
	}
