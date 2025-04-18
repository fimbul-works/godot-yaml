extends YAMLTest
## Test suite for Quaternion YAML serialization and styling

# Test values with different quaternion configurations
var test_values = {
	"identity": Quaternion.IDENTITY,
	"x_90": Quaternion.from_euler(Vector3(PI/2, 0, 0)),
	"y_90": Quaternion.from_euler(Vector3(0, PI/2, 0)),
	"z_90": Quaternion.from_euler(Vector3(0, 0, PI/2)),
	"xyz_rotation": Quaternion.from_euler(Vector3(PI/4, PI/3, PI/6)),
	"axis_angle": Quaternion(Vector3(1, 1, 1).normalized(), PI/3),
	"custom": Quaternion(0.5, 0.5, 0.5, 0.5).normalized()
}

func _init():
	test_name = "Quaternion YAML Tests"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	for name in test_values:
		var quat = test_values[name]
		var result = YAML.stringify(quat)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s: %s" % [name, truncate(result.get_data())])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, quat, is_quaternion_equal, name)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var quat = Quaternion(0.1, 0.2, 0.3, 0.9).normalized()

	# Map form (default)
	var map_style = YAML.create_style()
	map_style.set_container_form(YAMLStyle.FORM_MAP)
	var map_result = YAML.stringify(quat, map_style)

	assert_stringify_success(map_result, "map form")
	if !map_result.has_error():
		print_rich("• Map form:")
		print_rich(map_result.get_data())

		# Verify it contains map indicators
		assert_yaml_has_feature(map_result.get_data(), "x:", "Contains 'x:' key")
		assert_yaml_has_feature(map_result.get_data(), "y:", "Contains 'y:' key")
		assert_yaml_has_feature(map_result.get_data(), "z:", "Contains 'z:' key")
		assert_yaml_has_feature(map_result.get_data(), "w:", "Contains 'w:' key")

	# Sequence form
	var seq_style = YAML.create_style()
	seq_style.set_container_form(YAMLStyle.FORM_SEQ)
	var seq_result = YAML.stringify(quat, seq_style)

	assert_stringify_success(seq_result, "sequence form")
	if !seq_result.has_error():
		print_rich("• Sequence form:")
		print_rich(seq_result.get_data())

		# Verify it doesn't contain map indicators
		assert_yaml_lacks_feature(seq_result.get_data(), "x:", "Does not contain 'x:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "y:", "Does not contain 'y:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "z:", "Does not contain 'z:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "w:", "Does not contain 'w:' key")

	# Test roundtrip for both forms
	assert_roundtrip(YAML.parse(map_result.get_data()), quat, is_quaternion_equal, "map form")
	assert_roundtrip(YAML.parse(seq_result.get_data()), quat, is_quaternion_equal, "sequence form")

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var quat = Quaternion(0.1, 0.2, 0.3, 0.9).normalized()

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(quat, flow_style)

	assert_stringify_success(flow_result, "flow style")
	if !flow_result.has_error():
		print_rich("• Flow style:")
		print_rich(flow_result.get_data())

		# Verify it contains flow indicators (brackets)
		assert_yaml_has_feature(flow_result.get_data(), "{", "Contains opening brace")
		assert_yaml_has_feature(flow_result.get_data(), "}", "Contains closing brace")

	# Test block style (expanded)
	var block_style = YAML.create_style()
	block_style.set_flow_style(YAMLStyle.FLOW_NONE)
	var block_result = YAML.stringify(quat, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), quat, is_quaternion_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), quat, is_quaternion_equal, "block style")

## Test normalization preservation during serialization
func test_normalization_preservation() -> void:
	# Create various quaternions and check that they remain normalized after serialization
	var test_quats = {
		"from_euler": Quaternion.from_euler(Vector3(0.4, 0.8, 1.2)),
		"from_axis_angle": Quaternion(Vector3(1, 2, 3).normalized(), 0.75),
		"manually_normalized": Quaternion(0.1, 0.2, 0.3, 0.9).normalized()
	}

	for name in test_quats:
		var quat = test_quats[name]

		# First check that our starting quaternion is normalized
		assert_true(is_normalized(quat), "Original %s is normalized" % name)

		# Now serialize and deserialize
		var result = YAML.stringify(quat)
		assert_stringify_success(result, name)

		if !result.has_error():
			var parse_result = YAML.parse(result.get_data())
			assert_parse_success(parse_result, "parse %s" % name)

			if !parse_result.has_error():
				var parsed_quat = parse_result.get_data()

				# Check that deserialized quaternion is still normalized
				assert_true(
					is_normalized(parsed_quat),
					"Deserialized %s is still normalized" % name
				)

				# Additional validation: check that it represents the same rotation
				assert_true(
					is_quaternion_equal(quat, parsed_quat),
					"Deserialized %s represents the same rotation" % name
				)

## Test quaternion operations after serialization
func test_quaternion_operations() -> void:
	# Create two quaternions
	var quat1 = Quaternion.from_euler(Vector3(0.1, 0.2, 0.3))
	var quat2 = Quaternion.from_euler(Vector3(0.4, 0.5, 0.6))

	# Serialize and deserialize them
	var result1 = YAML.stringify(quat1)
	var result2 = YAML.stringify(quat2)

	assert_stringify_success(result1, "quat1")
	assert_stringify_success(result2, "quat2")

	if !result1.has_error() && !result2.has_error():
		var parse1 = YAML.parse(result1.get_data())
		var parse2 = YAML.parse(result2.get_data())

		assert_parse_success(parse1, "parse quat1")
		assert_parse_success(parse2, "parse quat2")

		if !parse1.has_error() && !parse2.has_error():
			var parsed_quat1 = parse1.get_data()
			var parsed_quat2 = parse2.get_data()

			# Original multiplication
			var orig_mult = quat1 * quat2

			# Parsed multiplication
			var parsed_mult = parsed_quat1 * parsed_quat2

			# Check operations
			assert_true(
				is_quaternion_equal(orig_mult, parsed_mult),
				"Multiplication results match after serialization"
			)

			# Check inverse operation
			var orig_inv = quat1.inverse()
			var parsed_inv = parsed_quat1.inverse()

			assert_true(
				is_quaternion_equal(orig_inv, parsed_inv),
				"Inverse results match after serialization"
			)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var quat = Quaternion.from_euler(Vector3(PI/6, PI/4, PI/3))

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Emit YAML with the style
	var emit_result = YAML.stringify(quat, original_style)
	assert_stringify_success(emit_result, "initial stringify")
	if emit_result.has_error():
		return

	var yaml_text = emit_result.get_data()
	print_rich("• Original YAML (with flow style):")
	print_rich(yaml_text)

	# Parse with style detection enabled
	var parse_result = YAML.parse(yaml_text, true)  # true enables style detection
	assert_parse_success(parse_result, "parse with style detection")
	if parse_result.has_error():
		return

	# Check if style was detected
	if !parse_result.has_style():
		print_rich("[color=yellow]⚠ No style was detected[/color]")
	else:
		print_rich("[color=green]✓ Style detected successfully[/color]")

		# Get the detected style and data
		var detected_style = parse_result.get_style()
		var parsed_quat = parse_result.get_data()

		# Modify the quaternion (rotate more around Z)
		var z_rotation = Quaternion(Vector3.BACK, PI/6)
		var modified_quat = parsed_quat * z_rotation

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_quat, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved (flow style should be maintained)
		assert_yaml_has_feature(re_emit_result.get_data(), "{", "Flow style was preserved (opening brace)")
		assert_yaml_has_feature(re_emit_result.get_data(), "}", "Flow style was preserved (closing brace)")

## Helper function to check if a quaternion is normalized
func is_normalized(q: Quaternion, epsilon: float = 0.00001) -> bool:
	var len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w
	return abs(len_sq - 1.0) < epsilon

## Helper function to check if Quaternion instances are equal (with floating point precision)
func is_quaternion_equal(a: Quaternion, b: Quaternion, epsilon: float = 0.00001) -> bool:
	# We need to handle the fact that q and -q represent the same rotation
	# So we check both possibilities
	var direct_match = (
		abs(a.x - b.x) < epsilon and
		abs(a.y - b.y) < epsilon and
		abs(a.z - b.z) < epsilon and
		abs(a.w - b.w) < epsilon
	)

	var negated_match = (
		abs(a.x + b.x) < epsilon and
		abs(a.y + b.y) < epsilon and
		abs(a.z + b.z) < epsilon and
		abs(a.w + b.w) < epsilon
	)

	return direct_match or negated_match
