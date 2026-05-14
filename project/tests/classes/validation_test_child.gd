class_name ValidationTestChild extends Object

var name: String
var age: int

func _init(p_name := "", p_age := 0):
	name = p_name
	age = p_age

static func deserialize(data: Variant):
	if typeof(data) != TYPE_DICTIONARY:
		return YAMLResult.error("ValidationTestChild expects Dictionary")

	var dict: Dictionary = data
	return ValidationTestChild.new(dict.get("name", ""), dict.get("age", 0))

func serialize() -> Dictionary:
	return {"name": name, "age": age}

func _to_string() -> String:
	return "ValidationTestChild(%s, %d)" % [name, age]
