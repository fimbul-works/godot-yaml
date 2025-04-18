extends YAMLTest
## Test suite for Plane YAML serialization and styling

# Test values with different plane configurations
var test_values = {
	"simple_x": Plane(Vector3.RIGHT, 5),
	"simple_y": Plane(Vector3.UP, 10),
	"simple_z": Plane(Vector3.BACK, 15),
	"negative_dist": Plane(Vector3.LEFT, -20),
	"with_normal": Plane(Vector3(1, 2, 3).normalized(), 25),
	"at_origin": Plane(Vector3(0.5, 0.5, 0.5).normalized(), 0),
	"decimal": Plane(Vector3(0.1, 0.2, 0.3).normalized(), 1.5)
}

func _init():
	test_name = "🧩 Plane YAML Tests"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	for name in test_values:
		var plane = test_values[name]
		var result = YAML.stringify(plane)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s: %s" % [name, truncate(result.get_data())])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, plane, is_plane_equal, name)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var plane = Plane(Vector3(1, 0, 0), 10)

	# Map form (default)
	var map_style = YAML.create_style()
	map_style.set_container_form(YAMLStyle.FORM_MAP)
	var map_result = YAML.stringify(plane, map_style)

	assert_stringify_success(map_result, "map form")
	if !map_result.has_error():
		print_rich("• Map form:")
		print_rich(map_result.get_data())

		# Verify it contains map indicators
		assert_yaml_has_feature(map_result.get_data(), "normal:", "Contains 'normal:' key")
		assert_yaml_has_feature(map_result.get_data(), "d:", "Contains 'd:' key")

	# Sequence form
	var seq_style = YAML.create_style()
	seq_style.set_container_form(YAMLStyle.FORM_SEQ)
	var seq_result = YAML.stringify(plane, seq_style)

	assert_stringify_success(seq_result, "sequence form")
	if !seq_result.has_error():
		print_rich("• Sequence form:")
		print_rich(seq_result.get_data())

		# Verify it doesn't contain map indicators
		assert_yaml_lacks_feature(seq_result.get_data(), "normal:", "Does not contain 'normal:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "d:", "Does not contain 'd:' key")

	# Test roundtrip for both forms
	assert_roundtrip(YAML.parse(map_result.get_data()), plane, is_plane_equal, "map form")
	assert_roundtrip(YAML.parse(seq_result.get_data()), plane, is_plane_equal, "sequence form")

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var plane = Plane(Vector3(0, 1, 0), 5)

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(plane, flow_style)

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
	var block_result = YAML.stringify(plane, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify indentation
		assert_yaml_has_feature(block_result.get_data(), "\n ", "Contains proper indentation")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), plane, is_plane_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), plane, is_plane_equal, "block style")

## Test nested styles for normal and d components
func test_nested_styles() -> void:
	var plane = Plane(Vector3(0, 0, 1), 15)

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for normal and d
	var normal_style = YAML.create_style()
	normal_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	var d_style = YAML.create_style()
	d_style.set_quote_style(YAMLStyle.QUOTE_DOUBLE)
	d_style.set_number_format(YAMLStyle.NUM_DECIMAL)

	# Apply nested styles
	parent_style.set_child("normal", normal_style)
	parent_style.set_child("d", d_style)

	var result = YAML.stringify(plane, parent_style)

	assert_stringify_success(result, "nested styles")
	if !result.has_error():
		print_rich("• Nested styles (normal=flow, d=special format):")
		print_rich(result.get_data())

		# Verify normal has flow style
		assert_yaml_has_feature(result.get_data(), "normal: {", "Normal has flow style")

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), plane, is_plane_equal, "nested styles")

## Test different representations of the normal vector
func test_normal_representations() -> void:
	# Create a plane with a normalized vector
	var normalized_normal = Vector3(1, 2, 3).normalized()
	var plane = Plane(normalized_normal, 10)

	var result = YAML.stringify(plane)
	assert_stringify_success(result, "plane with normalized normal")

	if !result.has_error():
		print_rich("• Plane with normalized normal:")
		print_rich(result.get_data())

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, plane, is_plane_equal, "normalized normal roundtrip")

		if !parse_result.has_error():
			var parsed_plane = parse_result.get_data()

			# Check that the normal is still normalized after roundtrip
			assert_true(
				parsed_plane.normal.is_normalized(),
				"Normal vector remains normalized after roundtrip"
			)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var plane = Plane(Vector3(0.5, 0.5, 0.5).normalized(), 7.5)

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Emit YAML with the style
	var emit_result = YAML.stringify(plane, original_style)
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
		var parsed_plane = parse_result.get_data()

		# Modify the plane
		var modified_plane = Plane(parsed_plane.normal, parsed_plane.d + 5.0)

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_plane, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved (flow style should be maintained)
		assert_yaml_has_feature(re_emit_result.get_data(), "{", "Flow style was preserved (opening brace)")
		assert_yaml_has_feature(re_emit_result.get_data(), "}", "Flow style was preserved (closing brace)")

## Helper function to check if Plane instances are equal (with floating point precision)
func is_plane_equal(a: Plane, b: Plane, epsilon: float = 0.00001) -> bool:
	return a.normal.is_equal_approx(b.normal) && abs(a.d - b.d) < epsilon
