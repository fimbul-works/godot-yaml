extends YAMLTest
## Test suite for Vector2 YAML serialization and styling

# Test values with different vector configurations
var test_values = {
	"zero": Vector2.ZERO,
	"one": Vector2.ONE,
	"up": Vector2.UP,
	"down": Vector2.DOWN,
	"left": Vector2.LEFT,
	"right": Vector2.RIGHT,
	"simple": Vector2(10, 20),
	"negative": Vector2(-30, -40),
	"mixed": Vector2(50, -60),
	"decimal": Vector2(1.5, 2.75),
	"large": Vector2(1000, 2000),
	"very_small": Vector2(0.0001, 0.0002),
	"normalized": Vector2(3, 4).normalized(),
	"inf": Vector2(INF, INF),
	"nan": Vector2(NAN, NAN)
}

func _init():
	test_name = "🧩 Vector2 YAML Tests"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	for name in test_values:
		var vec = test_values[name]
		var result = YAML.stringify(vec)

		# Skip NaN testing if needed since NaN != NaN by definition
		if name == "nan":
			print_rich("• %s: %s (not testing equality due to NaN properties)" % [name, result.get_data()])
			continue

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s: %s" % [name, result.get_data()])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, vec, is_vector2_equal, name)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var vec = Vector2(10, 20)

	# Map form (default)
	var map_style = YAML.create_style()
	map_style.set_container_form(YAMLStyle.FORM_MAP)
	var map_result = YAML.stringify(vec, map_style)

	assert_stringify_success(map_result, "map form")
	if !map_result.has_error():
		print_rich("• Map form:")
		print_rich(map_result.get_data())

		# Verify it contains map indicators
		assert_yaml_has_feature(map_result.get_data(), "x:", "Contains 'x:' key")
		assert_yaml_has_feature(map_result.get_data(), "y:", "Contains 'y:' key")

	# Sequence form
	var seq_style = YAML.create_style()
	seq_style.set_container_form(YAMLStyle.FORM_SEQ)
	var seq_result = YAML.stringify(vec, seq_style)

	assert_stringify_success(seq_result, "sequence form")
	if !seq_result.has_error():
		print_rich("• Sequence form:")
		print_rich(seq_result.get_data())

		# Verify it doesn't contain map indicators
		assert_yaml_lacks_feature(seq_result.get_data(), "x:", "Does not contain 'x:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "y:", "Does not contain 'y:' key")

	# Test roundtrip for both forms
	assert_roundtrip(YAML.parse(map_result.get_data()), vec, is_vector2_equal, "map form")
	assert_roundtrip(YAML.parse(seq_result.get_data()), vec, is_vector2_equal, "sequence form")

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var vec = Vector2(10, 20)

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(vec, flow_style)

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
	var block_result = YAML.stringify(vec, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), vec, is_vector2_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), vec, is_vector2_equal, "block style")

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var vec = Vector2(10, 20)

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Emit YAML with the style
	var emit_result = YAML.stringify(vec, original_style)
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
		var parsed_vec = parse_result.get_data()

		# Modify the vector
		var modified_vec = parsed_vec * 2

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_vec, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved (flow style should be maintained)
		assert_yaml_has_feature(re_emit_result.get_data(), "{", "Flow style was preserved (opening brace)")
		assert_yaml_has_feature(re_emit_result.get_data(), "}", "Flow style was preserved (closing brace)")

## Helper function to check if Vector2 instances are equal (with floating point precision)
func is_vector2_equal(a: Vector2, b: Vector2, epsilon: float = 0.00001) -> bool:
	return a.is_equal_approx(b)
