extends YAMLTest
## Test suite for Basis YAML serialization and styling

# Test values with different complexity
var test_values = {
	"identity": Basis.IDENTITY,
	"simple": Basis(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1)),
	"rotated": Basis(Vector3.RIGHT, PI/4),
	"scaled": Basis().scaled(Vector3(2, 3, 4)),
	"complex": Basis(Vector3(1.5, 2.5, 3.5), Vector3(-1.5, -2.5, -3.5), Vector3(4.5, 5.5, 6.5))
}

func _init():
	test_name = "🧩 Basis YAML Tests"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	for name in test_values:
		var basis = test_values[name]
		var result = YAML.stringify(basis)

		assert_stringify_success(result, name)
		if result.has_error():
			continue
		var foo: String

		print_rich("• %s: %s" % [name, truncate(result.get_data())])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, basis, is_basis_equal, name)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var basis = Basis(Vector3(1, 2, 3), Vector3(4, 5, 6), Vector3(7, 8, 9))

	# Map form (default)
	var map_style = YAML.create_style()
	map_style.set_container_form(YAMLStyle.FORM_MAP)
	var map_result = YAML.stringify(basis, map_style)

	assert_stringify_success(map_result, "map form")
	if not map_result.has_error():
		print_rich("• Map form:")
		print_rich(map_result.get_data())

		# Verify it contains map indicators
		assert_yaml_has_feature(map_result.get_data(), "x:", "Contains 'x:' key")
		assert_yaml_has_feature(map_result.get_data(), "y:", "Contains 'y:' key")
		assert_yaml_has_feature(map_result.get_data(), "z:", "Contains 'z:' key")

	# Sequence form
	var seq_style = YAML.create_style()
	seq_style.set_container_form(YAMLStyle.FORM_SEQ)
	var seq_result = YAML.stringify(basis, seq_style)

	assert_stringify_success(seq_result, "sequence form")
	if not seq_result.has_error():
		print_rich("• Sequence form:")
		print_rich(seq_result.get_data())

		# Verify it doesn't contain map indicators
		assert_yaml_lacks_feature(seq_result.get_data(), "x:", "Does not contain 'x:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "y:", "Does not contain 'y:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "z:", "Does not contain 'z:' key")

	# Test roundtrip for both forms
	assert_roundtrip(YAML.parse(map_result.get_data()), basis, is_basis_equal, "map form")
	assert_roundtrip(YAML.parse(seq_result.get_data()), basis, is_basis_equal, "sequence form")

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var basis = Basis(Vector3(1, 2, 3), Vector3(4, 5, 6), Vector3(7, 8, 9))

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(basis, flow_style)

	assert_stringify_success(flow_result, "flow style")
	if not flow_result.has_error():
		print_rich("• Flow style:")
		print_rich(flow_result.get_data())

		# Verify it contains flow indicators (brackets)
		assert_yaml_has_feature(flow_result.get_data(), "{", "Contains opening brace")
		assert_yaml_has_feature(flow_result.get_data(), "}", "Contains closing brace")

	# Test block style (expanded)
	var block_style = YAML.create_style()
	block_style.set_flow_style(YAMLStyle.FLOW_NONE)
	var block_result = YAML.stringify(basis, block_style)

	assert_stringify_success(block_result, "block style")
	if not block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify indentation
		assert_yaml_has_feature(block_result.get_data(), "\n ", "Contains proper indentation")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), basis, is_basis_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), basis, is_basis_equal, "block style")

## Test nested styles for x, y, z components
func test_nested_styles() -> void:
	var basis = Basis(Vector3(1, 2, 3), Vector3(4, 5, 6), Vector3(7, 8, 9))

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for each column
	var x_style = YAML.create_style()
	x_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	var y_style = YAML.create_style()
	y_style.set_flow_style(YAMLStyle.FLOW_NONE)

	var z_style = YAML.create_style()
	z_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Apply nested styles
	parent_style.set_child("x", x_style)
	parent_style.set_child("y", y_style)
	parent_style.set_child("z", z_style)

	var result = YAML.stringify(basis, parent_style)

	assert_stringify_success(result, "nested styles")
	if not result.has_error():
		print_rich("• Nested styles (x=flow, y=block, z=flow):")
		print_rich(result.get_data())

		# Verify each column has the expected style
		assert_yaml_has_feature(result.get_data(), "x: {", "X column has flow style")
		assert_true(
			result.get_data().find("y:") != -1 && result.get_data().find("y: {") == -1,
			"Y column has block style"
		)
		assert_yaml_has_feature(result.get_data(), "z: {", "Z column has flow style")

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), basis, is_basis_equal, "nested styles")

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var basis = Basis(Vector3(1, 2, 3), Vector3(4, 5, 6), Vector3(7, 8, 9))

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Emit YAML with the style
	var emit_result = YAML.stringify(basis, original_style)
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
	if not parse_result.has_style():
		print_rich("[color=yellow]⚠ No style was detected[/color]")
	else:
		print_rich("[color=green]✓ Style detected successfully[/color]")

		# Get the detected style and data
		var detected_style = parse_result.get_style()
		var parsed_basis = parse_result.get_data()

		# Modify the Basis (rotate around Y axis)
		var modified_basis = parsed_basis.rotated(Vector3.UP, PI/6)

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_basis, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved (flow style should be maintained)
		assert_yaml_has_feature(re_emit_result.get_data(), "{", "Flow style was preserved (opening brace)")
		assert_yaml_has_feature(re_emit_result.get_data(), "}", "Flow style was preserved (closing brace)")

## Helper function to check if Basis instances are equal (with floating point precision)
func is_basis_equal(a: Basis, b: Basis) -> bool:
	return (
		a.x.is_equal_approx(b.x) &&
		a.y.is_equal_approx(b.y) &&
		a.z.is_equal_approx(b.z)
	)
