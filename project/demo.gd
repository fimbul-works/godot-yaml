extends Node2D

func _ready():
	var file = FileAccess.open("res://yaml_data/comprehensive.yaml", FileAccess.READ)
	var yaml_input = file.get_as_text().replace("\t", "    ") # TODO

	return
	# Show version information
	print(YAML.version())

	# Test parsing
	var dict = test_parsing(yaml_input)
	if !dict:
		printerr("Parsing failed. Bummer!")
		return

	# Test emitting
	var yaml = test_emitting(dict)
	if !yaml:
		printerr("Emitting failed. Bogus!")
		return

	# Test variant conversions
	test_variants()

func test_parsing(input: String) -> Variant:
	return time_call(func():
		return YAML.parse(input)
	, "Parsing")

func test_emitting(input: Variant) -> Variant:
	return time_call(func():
		return YAML.emit(input)
	, "Emitting")

func test_variants():
	var variant_dict = get_variant_dict()

	for key in variant_dict.keys():
		var original_value = variant_dict[key]
		var eres = YAML.emit(original_value)
		if eres.has_error():
			printerr(key, " emit error: ", eres.get_error())
			continue

		var value_as_yaml = eres.get_data()
		print_rich("[b]%s YAML:[/b]\n%s" % [key, value_as_yaml])

		var pres = YAML.parse(value_as_yaml)
		if pres.has_error():
			printerr(key, " parse error: ", pres.get_error())
			continue
		var parsed_value = pres.get_data()
		print_rich("[b]%s parsed:[/b]\n%s\n" % [key, parsed_value])

	print_rich("[b]Before encoding:[/b]\n" + JSON.stringify(variant_dict, "  ", false))
	var emit_result = YAML.emit(variant_dict)
	if emit_result.has_error():
		printerr(emit_result.get_error())
		return

	var emitted_yaml = emit_result.get_data()
	print_rich("[b]Variants as YAML:[/b]\n", emitted_yaml)
	var parse_result = YAML.parse(emitted_yaml)
	if parse_result.has_error():
		printerr(parse_result.get_error())
		return
	var parsed_dict = parse_result.get_error() if parse_result.has_error() else parse_result.get_data()
	print_rich("[b]Variants after decoding:[/b]\n" + JSON.stringify(parsed_dict, "  ", false))

## Time a function call for performance
func time_call(fn: Callable, label: String = "Operation") -> Variant:
	var start = Time.get_ticks_usec()
	var result = fn.call()
	var elapsed = Time.get_ticks_usec() - start
	if result.has_error():
		printerr("%s failed: " % label, result.get_error())
		return
	var data = result.get_data()
	if typeof(data) == TYPE_STRING:
		print_rich("[b]%s Output:[/b]\n" % label, data)
	else:
		print_rich("[b]%s Output:[/b]\n" % label, JSON.stringify(data, "  ", false))
	print_rich("[i]Operation completed in %d microseconds[/i]\n" % elapsed)
	return data

func get_variant_dict():
	return {
		"aabb": AABB(Vector3(0, 1, 2), Vector3(3, 4, 5)),
		"basis": Basis(Vector3(6, 7, 8), Vector3(9, 10, 11), Vector3(12, 13, 14)),
		"callable": Callable(_callable_method), # Callable will be ignored, but will raise a warning
		"color": Color(1, 0.5, 0.25),
		"node_path": NodePath("root/Demo"),
		"packed_byte_array": Marshalls.base64_to_raw("aGVsbG8gd29ybGQ="),
		"packed_color_array": PackedColorArray([Color(1, 0, 0), Color(0, 1, 0), Color(0, 0, 1)]),
		"packed_float32_array": PackedFloat32Array([42.0, 1337.1337, -666.0]),
		"packed_float64_array": PackedFloat64Array([42.0, 1337.1337, -666.0]),
		"packed_int32_array": PackedInt32Array([2147483647, -2147483647]),
		"packed_int64_array": PackedInt64Array([9223372036854775807, -9223372036854775807]),
		"packed_string_array": PackedStringArray(["hello", "world", "this has\na new line"]),
		"packed_vector2_array": PackedVector2Array([Vector2(6, 7), Vector2(9, 10), Vector2(12, 13)]),
		"packed_vector3_array": PackedVector3Array([Vector3(42.0, 1337.1337, -666.0), Vector3(1, 0.5, 0.25), Vector3(0, 1, 2)]),
		"plane": Plane(Vector3(0, 1, 2), 123),
		"projection": Projection(Vector4(0, 1, 2, 3), Vector4(4, 5, 6, 7), Vector4(8, 9, 10, 11), Vector4(12, 13, 14, 15)),
		"quaternion": Quaternion(0, 1, 2, 3),
		"rect2": Rect2(1, 2, 3, 4),
		"rect2i": Rect2i(5, 6, 7, 8),
		"resource": ResourceLoader.load("res://test_resource.tres"),
		"string_name": &"string name",
		"transform2d": Transform2D(Vector2(0, 1), Vector2(2, 3), Vector2(4, 5)),
		"transform3d": Transform3D(Vector3(0, 1, 2), Vector3(3, 4, 5), Vector3(6, 7, 8), Vector3(9, 10, 11)),
		"vector2": Vector2(9, 10),
		"vector2i": Vector2i(11, 12),
		"vector3": Vector3(13, 14, 15),
		"vector3i": Vector3i(16, 17, 18),
		"vector4": Vector4(19, 20, 21, 22),
		"vector4i": Vector4i(23, 24, 25, 26),
	}

func _callable_method():
	pass
