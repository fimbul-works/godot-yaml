class_name YAMLExampleStringClass extends Object

@export var value: String

func _init(p_value := "") -> void:
	value = p_value

static func deserialize(data: Variant):
	if typeof(data) != TYPE_STRING:
		return YAMLResult.error("Deserializing YAMLExampleStringClass expects String, received %s" % [type_string(typeof(data))])

	return YAMLExampleStringClass.new(data)

func serialize() -> String:
	return value

func _to_string() -> String:
	return "YAMLExampleStringClass(%s)" % value
