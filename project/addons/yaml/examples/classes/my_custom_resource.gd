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

static func deserialize(data: Variant):
	if typeof(data) != TYPE_DICTIONARY:
		return YAMLResult.error("Deserializing MyCustomResource expects Dictionary, received %s" % [type_string(typeof(data))])

	var dict: Dictionary = data

	if !dict.has("string_val"):
		return YAMLResult.error("Missing string_val field")
	if !dict.has("int_val"):
		return YAMLResult.error("Missing int_val field")
	if !dict.has("float_val"):
		return YAMLResult.error("Missing float_val field")
	if !dict.has("color_val"):
		return YAMLResult.error("Missing color_val field")

	var string_val: String = dict.get("string_val")
	var int_val: int = dict.get("int_val")
	var float_val: float = dict.get("float_val")
	var color_val: Color = dict.get("color_val")

	return MyCustomResource.new(
		string_val,
		int_val,
		float_val,
		color_val
	)

func serialize() -> Dictionary:
	return {
		"string_val": string_val,
		"int_val": int_val,
		"float_val": float_val,
		"color_val": color_val,
	}

func _to_string() -> String:
	return "MyCustomResource(%s)" % string_val
