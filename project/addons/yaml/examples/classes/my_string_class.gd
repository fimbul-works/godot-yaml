class_name MyStringClass extends Object

@export var value: String

func _init(p_value := "") -> void:
	value = p_value

static func deserialize(data: Variant):
	if typeof(data) != TYPE_STRING:
		return YAMLResult.error("Deserializing MyStringClass expects String, received %s" % [type_string(typeof(data))])

	return MyStringClass.new(data)

func serialize() -> String:
	return value
