extends YAMLTest
## Test suite for Basis YAML serialization and styling

# Test values with different complexity
var test_values = {
	"Identity": Basis.IDENTITY,
	"Simple": Basis(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1)),
	"Rotated": Basis(Vector3.RIGHT, PI/4),
	"Scaled": Basis().scaled(Vector3(2, 3, 4)),
	"Complex": Basis(Vector3(1.5, 2.5, 3.5), Vector3(-1.5, -2.5, -3.5), Vector3(4.5, 5.5, 6.5))
}

func _init():
	test_name = "Basis"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	run_basic_serialization_test(test_values, is_basis_equal)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var basis = Basis(Vector3(1, 2, 3), Vector3(4, 5, 6), Vector3(7, 8, 9))
	run_container_forms_test(basis, is_basis_equal, ["x", "y", "z"])

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var basis = Basis(Vector3(1, 2, 3), Vector3(4, 5, 6), Vector3(7, 8, 9))
	run_flow_styles_test(basis, is_basis_equal)

## Test nested styles for x, y, z components
func test_nested_styles() -> void:
	var basis = Basis(Vector3(1, 2, 3), Vector3(4, 5, 6), Vector3(7, 8, 9))

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for each column
	parent_style.set_child("x", YAML.create_style().set_flow_style(YAMLStyle.FLOW_SINGLE))
	parent_style.set_child("y", YAML.create_style().set_flow_style(YAMLStyle.FLOW_NONE))
	parent_style.set_child("z", YAML.create_style().set_flow_style(YAMLStyle.FLOW_SINGLE))

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

	run_style_roundtrip_test(
		basis,
		is_basis_equal,
		func(parsed_basis): return parsed_basis.rotated(Vector3.UP, PI/6)
	)

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test Basis-specific parsing errors
	var invalid_basis_yaml = """
!Basis
x: {x: 1, y: 2, z: 3}
y: {x: 4, y: 5, z: 6}
# Missing z field
"""
	assert_parse_error(invalid_basis_yaml, "Missing required field detection")

	var wrong_type_yaml = """
!Basis "not a basis"
"""
	assert_parse_error(wrong_type_yaml, "Wrong type/format detection")

	var invalid_sequence_length = """
!Basis
- {x: 1, y: 2, z: 3}
- {x: 4, y: 5, z: 6}
# Missing third vector
"""
	assert_parse_error(invalid_sequence_length, "Invalid sequence length detection")

## Helper function to check if Basis instances are equal (with floating point precision)
func is_basis_equal(a: Basis, b: Basis) -> bool:
	return (
		a.x.is_equal_approx(b.x) &&
		a.y.is_equal_approx(b.y) &&
		a.z.is_equal_approx(b.z)
	)
