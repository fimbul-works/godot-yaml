extends YAMLTest
## Test suite for Projection YAML serialization and styling

# Test values with different projection configurations
var test_values = {
	"identity": Projection.IDENTITY,
	"perspective": Projection.create_perspective(70.0, 1.0, 0.05, 100.0),
	"orthogonal": Projection.create_orthogonal(-10, 10, -10, 10, 0.1, 100.0),
	"frustum": Projection.create_frustum(-1, 1, -1, 1, 0.1, 100.0),
	"perspective_fov": Projection.create_perspective(45.0, 1.777, 0.1, 100.0),
	"custom": Projection(
		Vector4(1.5, 0.0, 0.0, 0.0),
		Vector4(0.0, 2.5, 0.0, 0.0),
		Vector4(0.0, 0.0, 3.5, 0.0),
		Vector4(0.0, 0.0, 0.0, 4.5)
	)
}

func _init():
	test_name = "🧩 Projection YAML Tests"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	for name in test_values:
		var projection = test_values[name]
		var result = YAML.stringify(projection)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s: %s" % [name, truncate(result.get_data())])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, projection, is_projection_equal, name)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var projection = test_values["custom"]

	# Map form (default)
	var map_style = YAML.create_style()
	map_style.set_container_form(YAMLStyle.FORM_MAP)
	var map_result = YAML.stringify(projection, map_style)

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
	var seq_result = YAML.stringify(projection, seq_style)

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
	assert_roundtrip(YAML.parse(map_result.get_data()), projection, is_projection_equal, "map form")
	assert_roundtrip(YAML.parse(seq_result.get_data()), projection, is_projection_equal, "sequence form")

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var projection = test_values["perspective"]

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(projection, flow_style)

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
	var block_result = YAML.stringify(projection, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify indentation
		assert_yaml_has_feature(block_result.get_data(), "\n ", "Contains proper indentation")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), projection, is_projection_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), projection, is_projection_equal, "block style")

## Test nested styles for each column
func test_nested_styles() -> void:
	var projection = test_values["orthogonal"]

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for each column
	var x_style = YAML.create_style()
	x_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	var y_style = YAML.create_style()
	y_style.set_flow_style(YAMLStyle.FLOW_NONE)

	var z_style = YAML.create_style()
	z_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	var w_style = YAML.create_style()
	w_style.set_flow_style(YAMLStyle.FLOW_NONE)

	# Apply nested styles
	parent_style.set_child("x", x_style)
	parent_style.set_child("y", y_style)
	parent_style.set_child("z", z_style)
	parent_style.set_child("w", w_style)

	var result = YAML.stringify(projection, parent_style)

	assert_stringify_success(result, "nested styles")
	if !result.has_error():
		print_rich("• Nested styles (x=flow, y=block, z=flow, w=block):")
		print_rich(result.get_data())

		# Verify each column has the expected style
		assert_yaml_has_feature(result.get_data(), "x: {", "X column has flow style")
		assert_true(
			result.get_data().find("y:") != -1 && result.get_data().find("y: {") == -1,
			"Y column has block style"
		)
		assert_yaml_has_feature(result.get_data(), "z: {", "Z column has flow style")
		assert_true(
			result.get_data().find("w:") != -1 && result.get_data().find("w: {") == -1,
			"W column has block style"
		)

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), projection, is_projection_equal, "nested styles")

## Test projection-specific operations and roundtrip
func test_projection_operations() -> void:
	var perspective = test_values["perspective"]

	# Test determinant calculation before and after roundtrip
	var original_determinant = perspective.determinant()

	var result = YAML.stringify(perspective)
	assert_stringify_success(result, "perspective projection")

	if !result.has_error():
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse perspective projection")

		if !parse_result.has_error():
			var parsed_projection = parse_result.get_data()
			var parsed_determinant = parsed_projection.determinant()

			print_rich("• Original determinant: %f" % original_determinant)
			print_rich("• Parsed determinant: %f" % parsed_determinant)

			# Verify determinant is preserved
			assert_true(
				abs(original_determinant - parsed_determinant) < 0.0001,
				"Determinant is preserved after roundtrip"
			)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var projection = test_values["frustum"]

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Emit YAML with the style
	var emit_result = YAML.stringify(projection, original_style)
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
		var parsed_projection = parse_result.get_data()

		# Modify the projection (create a slightly different one)
		var modified_projection = Projection.create_frustum(-1.2, 1.2, -1.2, 1.2, 0.2, 120.0)

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_projection, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved (flow style should be maintained)
		assert_yaml_has_feature(re_emit_result.get_data(), "{", "Flow style was preserved (opening brace)")
		assert_yaml_has_feature(re_emit_result.get_data(), "}", "Flow style was preserved (closing brace)")

## Helper function to check if Projection instances are equal
func is_projection_equal(a: Projection, b: Projection) -> bool:
	return (
		a.x.is_equal_approx(b.x) &&
		a.y.is_equal_approx(b.y) &&
		a.z.is_equal_approx(b.z) &&
		a.w.is_equal_approx(b.w)
	)
