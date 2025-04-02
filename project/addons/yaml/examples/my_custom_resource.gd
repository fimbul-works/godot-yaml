class_name MyCustomResource extends Resource

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

static func from_dict(dict: Dictionary) -> MyCustomResource:
	var res := MyCustomResource.new(
		dict.get("string_val", ""),
		dict.get("int_val", 0),
		dict.get("float_val", 0.0),
		dict.get("color_val", Color.WHITE)
	)
	return res

func to_dict() -> Dictionary:
	return {
		"string_val": string_val,
		"int_val": int_val,
		"float_val": float_val,
		"color_val": color_val,
	}
