extends Node2D

func _ready():
	var file = FileAccess.open("res://test.yaml", FileAccess.READ)
	var yaml_input = file.get_as_text().replace("\t", "    ") # TODO

	var yaml = YAML.new()
	# Getting version information
	print(yaml.version())

	## Parsing YAML
	#var start = Time.get_ticks_usec()
	#var dict = yaml.parse(yaml_input)
	#var elapsed = Time.get_ticks_usec() - start
	#print_rich("[b]Parsed data:[/b]\n", JSON.stringify(dict, "  ", false))
	#print_rich("[i]Parsing took %d microseconds[/i]\n" % elapsed)
#
	## Stringifying to YAML
	#start = Time.get_ticks_usec()
	#var yaml_output = yaml.stringify(dict)
	#elapsed = Time.get_ticks_usec() - start
	#print_rich("[b]Stringified data:[/b]\n", yaml_output)
	#print_rich("[i]Stringifying took %d microseconds[/i]\n" % elapsed)

	# Test variant
	var dict = {
		"aabb": AABB(Vector3(0, 1, 2), Vector3(3, 4, 5)),
		"basis": Basis(Vector3(6, 7, 8), Vector3(9, 10, 11), Vector3(12, 13, 14)),
		"color": Color(1, 0.5, 0.25),
		"rect2": Rect2(1, 2, 3, 4),
		"rect2i": Rect2i(5, 6, 7, 8),
		"vector2": Vector2(9, 10),
		"vector2i": Vector2i(11, 12),
		"vector3": Vector3(13, 14, 15),
		"vector3i": Vector3i(16, 17, 18),
		"vector4": Vector4(19, 20, 21, 22),
		"vector4i": Vector4i(23, 24, 25, 26),
	}
	for key in dict.keys():
		var value = dict[key]
		var yaml_str = yaml.stringify(value)
		if yaml.get_error():
			printerr(key, " stringify error: ", yaml.get_error())
			return
		var val = yaml.parse(yaml_str)
		if yaml.get_error():
			printerr(key, " parse error: ", yaml.get_error())
			return
		print_rich("[b]%s YAML:[/b]\n%s" % [key, yaml_str])
		print_rich("[b]%s:[/b]\n%s\n" % [key, val])

	var dict_str = yaml.stringify(dict)
	print_rich("[b]Variants as YAML:[/b]\n", dict_str)
	dict = yaml.parse(dict_str)
	print_rich("[b]Variants from YAML:[/b]\n", dict)
	print(type_string(typeof(dict["rect2"])))
