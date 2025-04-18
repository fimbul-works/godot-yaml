extends YAMLTest
## Test suite for Rect2i YAML serialization and styling

# Test values with different rectangle configurations
var test_values = {
	"simple": Rect2i(10, 20, 30, 40),
	"zero": Rect2i(0, 0, 0, 0),
	"negative_pos": Rect2i(-10, -20, 30, 40),
	"negative_size": Rect2i(10, 20, -30, -40),  # Note: Negative size might get normalized
	"with_vectors": Rect2i(Vector2i(15, 25), Vector2i(35, 45)),
	"large": Rect2i(1000, 2000, 3000, 4000),
	"max_values": Rect2i(2147483647, 2147483647, 2147483647, 2147483647)  # INT32_MAX
}

func _init():
	test_name = "🧩 Rect2i YAML Tests"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	for name in test_values:
		var rect = test_values[name]
		var result = YAML.stringify(rect)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s: %s" % [name, truncate(result.get_data())])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, rect, is_rect2i_equal, name)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var rect = Rect2i(10, 20, 30, 40)

	# Map form (default)
	var map_style = YAML.create_style()
	map_style.set_container_form(YAMLStyle.FORM_MAP)
	var map_result = YAML.stringify(rect, map_style)

	assert_stringify_success(map_result, "map form")
	if !map_result.has_error():
		print_rich("• Map form:")
		print_rich(map_result.get_data())

		# Verify it contains map indicators
		assert_yaml_has_feature(map_result.get_data(), "position:", "Contains 'position:' key")
		assert_yaml_has_feature(map_result.get_data(), "size:", "Contains 'size:' key")

	# Sequence form
	var seq_style = YAML.create_style()
	seq_style.set_container_form(YAMLStyle.FORM_SEQ)
	var seq_result = YAML.stringify(rect, seq_style)

	assert_stringify_success(seq_result, "sequence form")
	if !seq_result.has_error():
		print_rich("• Sequence form:")
		print_rich(seq_result.get_data())

		# Verify it doesn't contain map indicators
		assert_yaml_lacks_feature(seq_result.get_data(), "position:", "Does not contain 'position:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "size:", "Does not contain 'size:' key")

	# Test roundtrip for both forms
	assert_roundtrip(YAML.parse(map_result.get_data()), rect, is_rect2i_equal, "map form")
	assert_roundtrip(YAML.parse(seq_result.get_data()), rect, is_rect2i_equal, "sequence form")

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var rect = Rect2i(10, 20, 30, 40)

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(rect, flow_style)

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
	var block_result = YAML.stringify(rect, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify indentation
		assert_yaml_has_feature(block_result.get_data(), "\n ", "Contains proper indentation")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), rect, is_rect2i_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), rect, is_rect2i_equal, "block style")

## Test nested styles for position and size components
func test_nested_styles() -> void:
	var rect = Rect2i(10, 20, 30, 40)

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for position and size
	var position_style = YAML.create_style()
	position_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	var size_style = YAML.create_style()
	size_style.set_flow_style(YAMLStyle.FLOW_NONE)

	# Apply nested styles
	parent_style.set_child("position", position_style)
	parent_style.set_child("size", size_style)

	var result = YAML.stringify(rect, parent_style)

	assert_stringify_success(result, "nested styles")
	if !result.has_error():
		print_rich("• Nested styles (position=flow, size=block):")
		print_rich(result.get_data())

		# Verify position has flow style and size has block style
		assert_yaml_has_feature(result.get_data(), "position: {", "Position has flow style")
		assert_true(
			result.get_data().find("size:") != -1 && result.get_data().find("size: {") == -1,
			"Size has block style"
		)

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), rect, is_rect2i_equal, "nested styles")

## Test Rect2i operations after serialization
func test_rect2i_operations() -> void:
	# Create a rectangle
	var rect = Rect2i(10, 20, 30, 40)

	# Serialize and deserialize
	var result = YAML.stringify(rect)
	assert_stringify_success(result, "rectangle")

	if !result.has_error():
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse rectangle")

		if !parse_result.has_error():
			var parsed_rect = parse_result.get_data()

			# Test various rect operations

			# 1. Area
			var original_area = rect.get_area()
			var parsed_area = parsed_rect.get_area()
			print_rich("• Area test: Original=%d, Parsed=%d" % [original_area, parsed_area])
			assert_equal(original_area, parsed_area, "Area calculation matches after serialization")

			# 2. Intersection
			var test_rect = Rect2i(25, 35, 20, 30)
			var original_intersection = rect.intersection(test_rect)
			var parsed_intersection = parsed_rect.intersection(test_rect)

			assert_true(
				is_rect2i_equal(original_intersection, parsed_intersection),
				"Intersection operation matches after serialization"
			)

			# 3. Contains point
			var test_point = Vector2i(15, 25)
			var original_contains = rect.has_point(test_point)
			var parsed_contains = parsed_rect.has_point(test_point)

			assert_equal(
				original_contains,
				parsed_contains,
				"Contains point operation matches after serialization"
			)

			# 4. Exact integer values (no floating point issues)
			assert_equal(parsed_rect.position.x, rect.position.x, "Integer X preserved exactly")
			assert_equal(parsed_rect.position.y, rect.position.y, "Integer Y preserved exactly")
			assert_equal(parsed_rect.size.x, rect.size.x, "Integer width preserved exactly")
			assert_equal(parsed_rect.size.y, rect.size.y, "Integer height preserved exactly")

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var rect = Rect2i(10, 20, 30, 40)

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Emit YAML with the style
	var emit_result = YAML.stringify(rect, original_style)
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
		var parsed_rect = parse_result.get_data()

		# Modify the rectangle (grow it)
		var modified_rect = Rect2i(
			parsed_rect.position - Vector2i(5, 5),
			parsed_rect.size + Vector2i(10, 10)
		)

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_rect, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved (flow style should be maintained)
		assert_yaml_has_feature(re_emit_result.get_data(), "{", "Flow style was preserved (opening brace)")
		assert_yaml_has_feature(re_emit_result.get_data(), "}", "Flow style was preserved (closing brace)")

## Helper function to check if Rect2i instances are equal
func is_rect2i_equal(a: Rect2i, b: Rect2i) -> bool:
	return a.position == b.position && a.size == b.size
