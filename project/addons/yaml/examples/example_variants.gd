extends ExampleBase

const EPSILON := 0.000001  # Tolerance for floating point comparisons

func _init() -> void:
	icon = "🧩"

func run_examples():
	var variants := get_variant_dict()

	# Test each type individually
	for key in variants:
		run_example(key, func(): run_variant_conversion(key, variants[key]))

	# Test full dictionary conversion
	print_rich("\n[b]Testing Full Dictionary Conversion:[/b]")
	var yaml_result := YAML.stringify(variants)
	if yaml_result.has_error():
		print_rich("[color=red]Dictionary stringify failed: %s[/color]" % yaml_result.get_error())
		return

	var yaml_text = yaml_result.get_data()
	var parse_result := YAML.parse(yaml_text)
	if parse_result.has_error():
		print_rich("[color=red]Dictionary parse failed: %s[/color]" % parse_result.get_error())
		return

	var decoded = parse_result.get_data()
	var all_passed := true
	for key in variants:
		if !is_approximately_equal(variants[key], decoded[key]):
			print_rich("[color=red]Dictionary value mismatch for %s:[/color]" % key)
			print_rich("  Expected: %s" % variants[key])
			print_rich("  Got: %s" % decoded[key])
			all_passed = false

	if all_passed:
		print_rich("[color=green]✓ All variant type conversions passed![/color]")

func run_variant_conversion(type_name: String, value: Variant) -> void:
	print_rich("\n[b]Testing %s:[/b]" % type_name)
	print_rich("[i]Original:[/i] %s" % str(value))

	# Test stringification
	var yaml_result := YAML.stringify(value)
	if yaml_result.has_error():
		print_rich("[color=red]Stringify failed: %s[/color]" % yaml_result.get_error())
		return

	var yaml = yaml_result.get_data()
	print_rich("[i]As YAML:[/i]\n%s" % yaml)

	# Test parsing
	var parse_result := YAML.parse(yaml)
	if parse_result.has_error():
		print_rich("[color=red]Parse failed: %s[/color]" % parse_result.get_error())
		return

	var decoded = parse_result.get_data()
	print_rich("[i]Decoded:[/i] %s" % str(decoded))

	# Verify value equality
	if !is_approximately_equal(value, decoded):
		print_rich("[color=red]Value mismatch:[/color]")
		print_rich("  Expected: %s" % str(value))
		print_rich("  Got: %s" % decoded)
	else:
		print_rich("[color=green]✓ Values match[/color]")

func is_approximately_equal(a: Variant, b: Variant) -> bool:
	if typeof(b) == TYPE_STRING:
		return str(a) == b
	match typeof(a):
		TYPE_STRING, TYPE_STRING_NAME:
			return a == b
		TYPE_FLOAT:
			return abs(a - b) < EPSILON
		TYPE_VECTOR2, TYPE_VECTOR3, TYPE_VECTOR4:
			return a.is_equal_approx(b)
		TYPE_ARRAY, TYPE_PACKED_FLOAT32_ARRAY, TYPE_PACKED_FLOAT64_ARRAY:
			if a.size() != b.size():
				return false
			for i in a.size():
				if not is_approximately_equal(a[i], b[i]):
					return false
			return true
		TYPE_QUATERNION, TYPE_BASIS, TYPE_TRANSFORM2D, TYPE_TRANSFORM3D:
			return a.is_equal_approx(b)
		TYPE_COLOR:
			var va = Vector4(a.r, a.g, a.b, a.a)
			var vb = Vector4(b.r, b.g, b.b, b.a)
			var dist = (va - vb).length()
			return dist < 0.01
		TYPE_DICTIONARY:
			if a.size() != b.size():
				return false
			for key in a:
				if not b.has(key) or not is_approximately_equal(a[key], b[key]):
					return false
			return true
		_:
			return a == b

func get_variant_dict() -> Dictionary:
	return {
		# Vector types
		"Vector2": Vector2(1, 2),
		"Vector2i": Vector2i(2, 4),
		"Vector3": Vector3(1, 2, 4),
		"Vector3i": Vector3i(2, 4, 8),
		"Vector4": Vector4(1, 2, 4, 8),
		"Vector4i": Vector4i(2, 4, 8, 16),

		# Geometric types
		"AABB": AABB(Vector3(1, 2, 4), Vector3(8, 16, 32)),
		"Basis": Basis(Vector3(1, 2, 4), Vector3(8, 16, 32), Vector3(64, 128, 256)),
		"Plane": Plane(Vector3(1, 2, 4), PI),
		"Quaternion": Quaternion(PI, 2*PI, 4*PI, 8*PI),
		"Rect2": Rect2(1, 2, 4, 8),
		"Rect2i": Rect2i(2, 4, 8, 16),
		"Transform2D": Transform2D(PI, Vector2(2*PI, 4*PI)),
		"Transform3D": Transform3D(Basis(), Vector3(1, 2, 4)),

		# Color types
		"Color": Color(1.0, 0.5, 0.25, 1.0),

		# Array types
		"PackedByteArray": PackedByteArray([1, 2, 4, 8]),
		"PackedColorArray": PackedColorArray([
			Color(1, 0, 0),
			Color(0, 1, 0),
			Color(0, 0, 1)
		]),
		"PackedFloat32Array": PackedFloat32Array([PI, 2*PI, 4*PI]),
		"PackedFloat64Array": PackedFloat64Array([PI, 2*PI, 4*PI]),
		"PackedInt32Array": PackedInt32Array([1, 2, 4, 8]),
		"PackedInt64Array": PackedInt64Array([1, 2, 4, 8]),
		"PackedStringArray": PackedStringArray([
			"one",
			"one\ntwo",
			"one\ntwo\nthree"
		]),
		"PackedVector2Array": PackedVector2Array([
			Vector2(1, 2),
			Vector2(4, 8)
		]),
		"PackedVector3Array": PackedVector3Array([
			Vector3(1, 2, 4),
			Vector3(8, 16, 32)
		]),

		# Matrix type
		"Projection": Projection(
			Vector4(1, 2, 4, 8),
			Vector4(16, 32, 64, 128),
			Vector4(256, 512, 1024, 2048),
			Vector4(4096, 8192, 16384, 32768)
		),

		# Reference types
		"NodePath": NodePath("root/level/player"),
		"StringName": &"test_string_name",
	}
