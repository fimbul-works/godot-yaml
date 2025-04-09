extends YAMLTest
## Test suite for Transform2D YAML serialization and styling

# Test values with different transform configurations
var test_values = {
	"identity": Transform2D.IDENTITY,
	"translation": Transform2D(0, Vector2(100, 200)),
	"rotation": Transform2D(PI/4, Vector2.ZERO),
	"scale": Transform2D(0, Vector2(2, 3)),
	"flip_x": Transform2D(Vector2(-1, 0), Vector2(0, 1), Vector2.ZERO),
	"flip_y": Transform2D(Vector2(1, 0), Vector2(0, -1), Vector2.ZERO),
	"combined": Transform2D(PI/6, Vector2(50, 75)).scaled(Vector2(1.5, 2.5)),
	"custom": Transform2D(Vector2(1.5, 0.5), Vector2(-0.5, 1.5), Vector2(100, 150))
}

func _init():
	test_name = "🧩 Transform2D YAML Tests"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	for name in test_values:
		var transform = test_values[name]
		var result = YAML.stringify(transform)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s: %s" % [name, truncate(result.get_data())])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, transform, is_transform2d_equal, name)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var transform = Transform2D(PI/4, Vector2(50, 75))

	# Map form (default)
	var map_style = YAML.create_style()
	map_style.set_container_form(YAMLStyle.FORM_MAP)
	var map_result = YAML.stringify(transform, map_style)

	assert_stringify_success(map_result, "map form")
	if !map_result.has_error():
		print_rich("• Map form:")
		print_rich(map_result.get_data())

		# Verify it contains map indicators
		assert_yaml_has_feature(map_result.get_data(), "x:", "Contains 'x:' key")
		assert_yaml_has_feature(map_result.get_data(), "y:", "Contains 'y:' key")
		assert_yaml_has_feature(map_result.get_data(), "origin:", "Contains 'origin:' key")

	# Sequence form
	var seq_style = YAML.create_style()
	seq_style.set_container_form(YAMLStyle.FORM_SEQ)
	var seq_result = YAML.stringify(transform, seq_style)

	assert_stringify_success(seq_result, "sequence form")
	if !seq_result.has_error():
		print_rich("• Sequence form:")
		print_rich(seq_result.get_data())

		# Verify it doesn't contain map indicators
		assert_yaml_lacks_feature(seq_result.get_data(), "x:", "Does not contain 'x:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "y:", "Does not contain 'y:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "origin:", "Does not contain 'origin:' key")

	# Test roundtrip for both forms
	assert_roundtrip(YAML.parse(map_result.get_data()), transform, is_transform2d_equal, "map form")
	assert_roundtrip(YAML.parse(seq_result.get_data()), transform, is_transform2d_equal, "sequence form")

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var transform = Transform2D(PI/4, Vector2(50, 75))

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(transform, flow_style)

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
	var block_result = YAML.stringify(transform, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify indentation
		assert_yaml_has_feature(block_result.get_data(), "\n ", "Contains proper indentation")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), transform, is_transform2d_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), transform, is_transform2d_equal, "block style")

## Test nested styles for x, y, and origin components
func test_nested_styles() -> void:
	var transform = Transform2D(PI/4, Vector2(50, 75))

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for each component
	var x_style = YAML.create_style()
	x_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	var y_style = YAML.create_style()
	y_style.set_flow_style(YAMLStyle.FLOW_NONE)

	var origin_style = YAML.create_style()
	origin_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Apply nested styles
	parent_style.set_child("x", x_style)
	parent_style.set_child("y", y_style)
	parent_style.set_child("origin", origin_style)

	var result = YAML.stringify(transform, parent_style)

	assert_stringify_success(result, "nested styles")
	if !result.has_error():
		print_rich("• Nested styles (x=flow, y=block, origin=flow):")
		print_rich(result.get_data())

		# Verify each component has the expected style
		assert_yaml_has_feature(result.get_data(), "x: {", "X column has flow style")
		assert_true(
			result.get_data().find("y:") != -1 && result.get_data().find("y: {") == -1,
			"Y column has block style"
		)
		assert_yaml_has_feature(result.get_data(), "origin: {", "Origin has flow style")

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), transform, is_transform2d_equal, "nested styles")

## Test Transform2D operations after serialization
func test_transform2d_operations() -> void:
	# Create a transform with rotation and translation
	var transform = Transform2D(PI/6, Vector2(50, 75))

	# Serialize and deserialize
	var result = YAML.stringify(transform)
	assert_stringify_success(result, "transform")

	if !result.has_error():
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse transform")

		if !parse_result.has_error():
			var parsed_transform = parse_result.get_data()

			# Test transform operations

			# 1. Transform a point
			var test_point = Vector2(10, 20)
			var original_transformed = transform * test_point
			var parsed_transformed = parsed_transform * test_point

			assert_true(
				original_transformed.is_equal_approx(parsed_transformed),
				"Point transformation matches after serialization"
			)

			# 2. Get rotation angle
			var original_rotation = transform.get_rotation()
			var parsed_rotation = parsed_transform.get_rotation()

			assert_true(
				abs(original_rotation - parsed_rotation) < 0.0001,
				"Rotation extraction matches after serialization"
			)

			# 3. Get scale
			var original_scale = transform.get_scale()
			var parsed_scale = parsed_transform.get_scale()

			assert_true(
				original_scale.is_equal_approx(parsed_scale),
				"Scale extraction matches after serialization"
			)

			# 4. Determinant
			var original_determinant = transform.determinant()
			var parsed_determinant = parsed_transform.determinant()

			assert_true(
				abs(original_determinant - parsed_determinant) < 0.0001,
				"Determinant calculation matches after serialization"
			)

## Test combining transforms
func test_combining_transforms() -> void:
	# Create transforms to combine
	var transform_a = Transform2D(PI/4, Vector2(10, 20))  # 45 degree rotation + translation
	var transform_b = Transform2D(0, Vector2.ZERO).scaled(Vector2(2, 3))  # Scaling

	# Combine them
	var original_combined = transform_a * transform_b

	# Serialize and deserialize both transforms
	var result_a = YAML.stringify(transform_a)
	var result_b = YAML.stringify(transform_b)

	assert_stringify_success(result_a, "transform A")
	assert_stringify_success(result_b, "transform B")

	if !result_a.has_error() && !result_b.has_error():
		var parse_a = YAML.parse(result_a.get_data())
		var parse_b = YAML.parse(result_b.get_data())

		assert_parse_success(parse_a, "parse transform A")
		assert_parse_success(parse_b, "parse transform B")

		if !parse_a.has_error() && !parse_b.has_error():
			var parsed_a = parse_a.get_data()
			var parsed_b = parse_b.get_data()

			# Combine the parsed transforms
			var parsed_combined = parsed_a * parsed_b

			# Compare the results
			assert_true(
				is_transform2d_equal(original_combined, parsed_combined),
				"Combined transformation matches after serialization"
			)

## Test special transforms (rotation, scaling, etc.)
func test_special_transforms() -> void:
	# Test cases with special properties
	var special_cases = {
		"orthogonal": Transform2D(PI/2, Vector2.ZERO),  # 90-degree rotation (orthogonal matrix)
		"reflection": Transform2D(Vector2(-1, 0), Vector2(0, 1), Vector2.ZERO),  # X-axis reflection
		"uniform_scale": Transform2D(0, Vector2.ZERO).scaled(Vector2(2, 2)),  # Uniform scaling
		"shear": Transform2D(Vector2(1, 0.5), Vector2(0, 1), Vector2.ZERO)  # Shear transformation
	}

	for name in special_cases:
		var transform = special_cases[name]
		var result = YAML.stringify(transform)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse " + name)

		if !parse_result.has_error():
			var parsed_transform = parse_result.get_data()

			# Verify the parsed transform maintains special properties
			assert_true(
				is_transform2d_equal(transform, parsed_transform),
				"%s properties preserved after serialization" % name
			)

			# Test specific properties
			match name:
				"orthogonal":
					assert_true(
						abs(abs(parsed_transform.determinant()) - 1.0) < 0.0001,
						"Orthogonal transform has determinant of ±1"
					)
				"reflection":
					assert_true(
						parsed_transform.determinant() < 0,
						"Reflection transform has negative determinant"
					)
				"uniform_scale":
					var scale = parsed_transform.get_scale()
					assert_true(
						abs(scale.x - scale.y) < 0.0001,
						"Uniform scale has equal X and Y scaling factors"
					)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var transform = Transform2D(PI/4, Vector2(50, 75))

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Emit YAML with the style
	var emit_result = YAML.stringify(transform, original_style)
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
		var parsed_transform = parse_result.get_data()

		# Modify the transform (additional rotation)
		var modified_transform = parsed_transform.rotated(PI/6)

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_transform, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved (flow style should be maintained)
		assert_yaml_has_feature(re_emit_result.get_data(), "{", "Flow style was preserved (opening brace)")
		assert_yaml_has_feature(re_emit_result.get_data(), "}", "Flow style was preserved (closing brace)")

## Helper function to check if Transform2D instances are equal (with floating point precision)
func is_transform2d_equal(a: Transform2D, b: Transform2D, epsilon: float = 0.00001) -> bool:
	return (
		a.x.is_equal_approx(b.x) &&
		a.y.is_equal_approx(b.y) &&
		a.origin.is_equal_approx(b.origin)
	)
