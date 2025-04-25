extends YAMLTest
## Test suite for Transform3D YAML serialization and styling

# Test values with different transform configurations
var test_values = {
	"identity": Transform3D.IDENTITY,
	"translation": Transform3D(Basis(), Vector3(100, 200, 300)),
	"rotation_x": Transform3D(Basis.from_euler(Vector3(PI/2, 0, 0))),
	"rotation_y": Transform3D(Basis.from_euler(Vector3(0, PI/2, 0))),
	"rotation_z": Transform3D(Basis.from_euler(Vector3(0, 0, PI/2))),
	"rotation_xyz": Transform3D(Basis.from_euler(Vector3(PI/4, PI/3, PI/6))),
	"scaling": Transform3D(Basis().scaled(Vector3(2, 3, 4))),
	"combined": Transform3D(Basis.from_euler(Vector3(PI/4, PI/3, PI/6)), Vector3(50, 75, 100)).scaled(Vector3(1.5, 2, 2.5)),
	"looking_at": Transform3D().looking_at(Vector3(10, 5, -10), Vector3.UP),
	"custom": Transform3D(Basis(
		Vector3(1.5, 0.5, 0.25),
		Vector3(-0.5, 1.5, 0.25),
		Vector3(0.25, 0.25, 1.5)),
		Vector3(100, 150, 200))
}

func _init():
	test_name = "🧩 Transform3D YAML Tests"

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
		assert_roundtrip(parse_result, transform, is_transform3d_equal, name)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var transform = Transform3D(Basis.from_euler(Vector3(PI/4, PI/6, PI/8)), Vector3(50, 75, 100))

	# Map form (default)
	var map_style = YAML.create_style()
	map_style.set_container_form(YAMLStyle.FORM_MAP)
	var map_result = YAML.stringify(transform, map_style)

	assert_stringify_success(map_result, "map form")
	if !map_result.has_error():
		print_rich("• Map form:")
		print_rich(map_result.get_data())

		# Verify it contains map indicators
		assert_yaml_has_feature(map_result.get_data(), "basis:", "Contains 'basis:' key")
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
		assert_yaml_lacks_feature(seq_result.get_data(), "basis:", "Does not contain 'basis:' key")
		assert_yaml_lacks_feature(seq_result.get_data(), "origin:", "Does not contain 'origin:' key")

	# Test roundtrip for both forms
	assert_roundtrip(YAML.parse(map_result.get_data()), transform, is_transform3d_equal, "map form")
	assert_roundtrip(YAML.parse(seq_result.get_data()), transform, is_transform3d_equal, "sequence form")

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var transform = Transform3D(Basis.from_euler(Vector3(PI/4, PI/6, PI/8)), Vector3(50, 75, 100))

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
	assert_roundtrip(YAML.parse(flow_result.get_data()), transform, is_transform3d_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), transform, is_transform3d_equal, "block style")

## Test nested styles for basis and origin components
func test_nested_styles() -> void:
	var transform = Transform3D(Basis.from_euler(Vector3(PI/4, PI/6, PI/8)), Vector3(50, 75, 100))

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for basis and origin
	var basis_style = YAML.create_style()
	basis_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	var origin_style = YAML.create_style()
	origin_style.set_flow_style(YAMLStyle.FLOW_NONE)

	# Apply nested styles
	parent_style.set_child("basis", basis_style)
	parent_style.set_child("origin", origin_style)

	var result = YAML.stringify(transform, parent_style)

	assert_stringify_success(result, "nested styles")
	if !result.has_error():
		print_rich("• Nested styles (basis=flow, origin=block):")
		print_rich(result.get_data())

		# Verify each component has the expected style
		assert_yaml_has_feature(result.get_data(), "basis: {", "Basis has flow style")
		assert_true(
			result.get_data().find("origin:") != -1 && result.get_data().find("origin: {") == -1,
			"Origin has block style"
		)

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), transform, is_transform3d_equal, "nested styles")

## Test Transform3D operations after serialization
func test_transform3d_operations() -> void:
	# Create a transform with rotation and translation
	var transform = Transform3D(Basis.from_euler(Vector3(PI/4, PI/6, PI/8)), Vector3(50, 75, 100))

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
			var test_point = Vector3(10, 20, 30)
			var original_transformed = transform * test_point
			var parsed_transformed = parsed_transform * test_point

			assert_true(
				original_transformed.is_equal_approx(parsed_transformed),
				"Point transformation matches after serialization"
			)

			# 2. Get basis and origin
			assert_true(
				parsed_transform.basis.is_equal_approx(transform.basis),
				"Basis extraction matches after serialization"
			)

			assert_true(
				parsed_transform.origin.is_equal_approx(transform.origin),
				"Origin extraction matches after serialization"
			)

			# 3. Determinant
			var original_determinant = transform.basis.determinant()
			var parsed_determinant = parsed_transform.basis.determinant()

			assert_true(
				abs(original_determinant - parsed_determinant) < 0.0001,
				"Determinant calculation matches after serialization"
			)

			# 4. Inverse
			var original_inverse = transform.affine_inverse()
			var parsed_inverse = parsed_transform.affine_inverse()

			assert_true(
				is_transform3d_equal(original_inverse, parsed_inverse),
				"Inverse calculation matches after serialization"
			)

## Test combining transforms
func test_combining_transforms() -> void:
	# Create transforms to combine
	var transform_a = Transform3D(Basis.from_euler(Vector3(PI/4, 0, 0)), Vector3(10, 20, 30))  # X rotation + translation
	var transform_b = Transform3D(Basis().scaled(Vector3(2, 3, 4)), Vector3.ZERO)  # Scaling

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
				is_transform3d_equal(original_combined, parsed_combined),
				"Combined transformation matches after serialization"
			)

## Test special transforms (rotation, scaling, etc.)
func test_special_transforms() -> void:
	# Test cases with special properties
	var special_cases = {
		"orthogonal": Transform3D(Basis.from_euler(Vector3(PI/2, PI/2, 0))),  # 90-degree rotations (orthogonal matrix)
		"reflection": Transform3D(Basis(Vector3(-1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1))),  # X-axis reflection
		"uniform_scale": Transform3D(Basis().scaled(Vector3(3, 3, 3))),  # Uniform scaling
		"looking_at_origin": Transform3D().looking_at(Vector3.FORWARD, Vector3.UP),  # Look at forward
		"looking_at_point": Transform3D(Basis(), Vector3(5, 5, 0)).looking_at(Vector3.ZERO, Vector3.UP)  # Look at origin
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
				is_transform3d_equal(transform, parsed_transform),
				"%s properties preserved after serialization" % name
			)

			# Test specific properties
			match name:
				"orthogonal":
					assert_true(
						abs(abs(parsed_transform.basis.determinant()) - 1.0) < 0.0001,
						"Orthogonal transform has determinant of ±1"
					)
				"reflection":
					assert_true(
						parsed_transform.basis.determinant() < 0,
						"Reflection transform has negative determinant"
					)
				"uniform_scale":
					var x_scale = parsed_transform.basis.get_scale().x
					var y_scale = parsed_transform.basis.get_scale().y
					var z_scale = parsed_transform.basis.get_scale().z
					assert_true(
						abs(x_scale - y_scale) < 0.0001 && abs(x_scale - z_scale) < 0.0001,
						"Uniform scale has equal X, Y and Z scaling factors"
					)

## Test 3D-specific transform operations
func test_3d_specific_operations() -> void:
	# Test operations that are specific to 3D transforms

	# 1. Looking at
	var look_transform = Transform3D().looking_at(Vector3(10, 5, -10), Vector3.UP)
	var look_result = YAML.stringify(look_transform)
	assert_stringify_success(look_result, "looking_at transform")

	if !look_result.has_error():
		var parse_result = YAML.parse(look_result.get_data())
		assert_parse_success(parse_result, "parse looking_at transform")

		if !parse_result.has_error():
			var parsed_look = parse_result.get_data()

			# Test forward direction is maintained
			var original_forward = -look_transform.basis.z  # Forward is -Z in Godot
			var parsed_forward = -parsed_look.basis.z

			assert_true(
				original_forward.is_equal_approx(parsed_forward),
				"Forward direction maintained after serialization"
			)

			# Test up direction is maintained
			var original_up = look_transform.basis.y
			var parsed_up = parsed_look.basis.y

			assert_true(
				original_up.is_equal_approx(parsed_up),
				"Up direction maintained after serialization"
			)

	# 2. Transforming basis vectors
	var transform = Transform3D(Basis.from_euler(Vector3(PI/4, PI/3, PI/6)), Vector3(50, 75, 100))
	var result = YAML.stringify(transform)

	if !result.has_error():
		var parse_result = YAML.parse(result.get_data())

		if !parse_result.has_error():
			var parsed_transform = parse_result.get_data()

			# Test transforming each basis vector
			var vectors = [Vector3.RIGHT, Vector3.UP, Vector3.BACK]
			var names = ["right", "up", "back"]

			for i in range(vectors.size()):
				var vec = vectors[i]
				var original_transformed = transform.basis * vec
				var parsed_transformed = parsed_transform.basis * vec

				assert_true(
					original_transformed.is_equal_approx(parsed_transformed),
					"Basis vector %s transformation maintained after serialization" % names[i]
				)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var transform = Transform3D(Basis.from_euler(Vector3(PI/4, PI/6, PI/8)), Vector3(50, 75, 100))

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
		var modified_transform = parsed_transform.rotated(Vector3.UP, PI/6)

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

## Helper function to check if Transform3D instances are equal (with floating point precision)
func is_transform3d_equal(a: Transform3D, b: Transform3D, epsilon: float = 0.00001) -> bool:
	return a.basis.is_equal_approx(b.basis) && a.origin.is_equal_approx(b.origin)
