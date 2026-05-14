class_name ValidationTestParent extends Object

var title: String
var child: ValidationTestChild

func _init(p_title := "", p_child = null):
	title = p_title
	child = p_child

static func deserialize(data: Variant):
	if typeof(data) != TYPE_DICTIONARY:
		return YAMLResult.error("ValidationParent expects Dictionary")

	var dict: Dictionary = data

	# Child is already deserialized by the YAML parser
	var child_obj = dict.get("child")
	if not child_obj is ValidationTestChild:
		return YAMLResult.error("Child must be ValidationTestChild instance, got %s" % type_string(typeof(child_obj)))

	return ValidationTestParent.new(dict.get("title"), child_obj)

func serialize() -> Dictionary:
	return {
		"title": title,
		"child": child.serialize() if child else null
	}

func _to_string() -> String:
	return "ValidationTestParent(%s, %s)" % [title, child]
