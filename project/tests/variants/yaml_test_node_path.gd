extends YAMLTest
## Test suite for NodePath YAML serialization and styling

# Test values with different path formats
var test_values = {
	"Empty": NodePath(""),
	"simple": NodePath("Node"),
	"Child": NodePath("Parent/Child"),
	"Absolute": NodePath("/root/Scene/Node"),
	"Parent": NodePath("../SiblingNode"),
	"Property": NodePath("Node:position"),
	"Indexed_property": NodePath("Node:position:x"),
	"Complex": NodePath("/root/Main/Path/With:property"),
	"Special_chars": NodePath("Node-With_Special.Chars")
}

func _init():
	test_name = "NodePath"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	run_basic_serialization_test(test_values, is_nodepath_equal)

## Test string style variations
func test_string_styles() -> void:
	var path = NodePath("Parent/Child:property")
	run_string_styles_test(path, is_nodepath_equal)

## Test empty path handling
func test_empty_path() -> void:
	var empty_path = NodePath("")

	var result = YAML.stringify(empty_path)
	assert_stringify_success(result, "empty path")

	if not result.has_error():
		print_rich("• Empty path serialization:")
		print_rich(result.get_data())
		# Verify it's either an empty string or null value in YAML
		var is_empty_string = result.get_data().strip_edges() == "!NodePath"
		assert_true(is_empty_string, "Empty path is properly serialized")

	# Test roundtrip
	var parse_result = YAML.parse(result.get_data())
	assert_roundtrip(parse_result, empty_path, is_nodepath_equal, "empty path")

## Helper function to check if NodePath instances are equal
func is_nodepath_equal(a: NodePath, b: NodePath) -> bool:
	return String(a) == String(b)
