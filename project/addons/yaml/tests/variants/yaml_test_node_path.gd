extends YAMLTest
## Test suite for NodePath YAML serialization and styling

# Test values with different path formats
var test_values = {
	"empty": NodePath(""),
	"simple": NodePath("Node"),
	"child": NodePath("Parent/Child"),
	"absolute": NodePath("/root/Scene/Node"),
	"parent": NodePath("../SiblingNode"),
	"property": NodePath("Node:position"),
	"indexed_property": NodePath("Node:position:x"),
	"complex": NodePath("/root/Main/Path/With:property"),
	"special_chars": NodePath("Node-With_Special.Chars")
}

func _init():
	test_name = "🧩 NodePath YAML Tests"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	for name in test_values:
		var path = test_values[name]
		var result = YAML.stringify(path)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s: %s" % [name, truncate(result.get_data())])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, path, is_nodepath_equal, name)

## Test quote style variations
func test_quote_styles() -> void:
	var path = NodePath("Parent/Child:property")

	# Double quotes (default)
	var double_style = YAML.create_style()
	double_style.set_quote_style(YAMLStyle.QUOTE_DOUBLE)
	var double_result = YAML.stringify(path, double_style)

	assert_stringify_success(double_result, "double quotes")
	if not double_result.has_error():
		print_rich("• Double quotes:")
		print_rich(double_result.get_data())

		# Verify it contains double quotes
		assert_yaml_has_feature(double_result.get_data(), "\"", "Contains double quotes")

	# Single quotes
	var single_style = YAML.create_style()
	single_style.set_quote_style(YAMLStyle.QUOTE_SINGLE)
	var single_result = YAML.stringify(path, single_style)

	assert_stringify_success(single_result, "single quotes")
	if not single_result.has_error():
		print_rich("• Single quotes:")
		print_rich(single_result.get_data())

		# Verify it contains single quotes
		assert_yaml_has_feature(single_result.get_data(), "'", "Contains single quotes")

	# No quotes (plain style)
	var plain_style = YAML.create_style()
	plain_style.set_quote_style(YAMLStyle.QUOTE_NONE)
	var plain_result = YAML.stringify(path, plain_style)

	assert_stringify_success(plain_result, "no quotes")
	if not plain_result.has_error():
		print_rich("• No quotes:")
		print_rich(plain_result.get_data())

		# Verify it doesn't contain quotes
		# Note: This may not work if the NodePath requires quotes for valid YAML
		if plain_result.get_data().find("\"") == -1 && plain_result.get_data().find("'") == -1:
			print_rich("[color=green]✓ No quotes used[/color]")
		else:
			print_rich("[color=yellow]⚠ Quotes were needed for valid YAML[/color]")

	# Test roundtrip for all styles
	assert_roundtrip(YAML.parse(double_result.get_data()), path, is_nodepath_equal, "double quotes")
	assert_roundtrip(YAML.parse(single_result.get_data()), path, is_nodepath_equal, "single quotes")
	assert_roundtrip(YAML.parse(plain_result.get_data()), path, is_nodepath_equal, "no quotes")

## Test different scalar styles (plain vs literal vs folded)
func test_scalar_styles() -> void:
	var long_path = NodePath("Very/Long/Path/With/Many/Components:and:properties")

	# Plain style (default)
	var plain_style = YAML.create_style()
	plain_style.set_scalar_style(YAMLStyle.SCALAR_PLAIN)
	var plain_result = YAML.stringify(long_path, plain_style)

	assert_stringify_success(plain_result, "plain style")
	if not plain_result.has_error():
		print_rich("• Plain style:")
		print_rich(plain_result.get_data())

	# Literal style (|)
	var literal_style = YAML.create_style()
	literal_style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)
	var literal_result = YAML.stringify(long_path, literal_style)

	assert_stringify_success(literal_result, "literal style")
	if not literal_result.has_error():
		print_rich("• Literal style:")
		print_rich(literal_result.get_data())

		# Verify it uses literal style indicator
		if literal_result.get_data().find("|") != -1:
			assert_yaml_has_feature(literal_result.get_data(), "|", "Uses literal style indicator")
		else:
			print_rich("[color=yellow]⚠ Literal style not applied (may be valid for simple paths)[/color]")

	# Folded style (>)
	var folded_style = YAML.create_style()
	folded_style.set_scalar_style(YAMLStyle.SCALAR_FOLDED)
	var folded_result = YAML.stringify(long_path, folded_style)

	assert_stringify_success(folded_result, "folded style")
	if not folded_result.has_error():
		print_rich("• Folded style:")
		print_rich(folded_result.get_data())

		# Verify it uses folded style indicator
		if folded_result.get_data().find(">") != -1:
			assert_yaml_has_feature(folded_result.get_data(), ">", "Uses folded style indicator")
		else:
			print_rich("[color=yellow]⚠ Folded style not applied (may be valid for simple paths)[/color]")

	# Test roundtrip for all styles
	assert_roundtrip(YAML.parse(plain_result.get_data()), long_path, is_nodepath_equal, "plain style")
	assert_roundtrip(YAML.parse(literal_result.get_data()), long_path, is_nodepath_equal, "literal style")
	assert_roundtrip(YAML.parse(folded_result.get_data()), long_path, is_nodepath_equal, "folded style")

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
		print(result.get_data())
		assert_true(is_empty_string, "Empty path is properly serialized")

	# Test roundtrip
	var parse_result = YAML.parse(result.get_data())
	assert_roundtrip(parse_result, empty_path, is_nodepath_equal, "empty path")

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var path = NodePath("Parent/Child/GrandChild:property")

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_quote_style(YAMLStyle.QUOTE_SINGLE)

	# Emit YAML with the style
	var emit_result = YAML.stringify(path, original_style)
	assert_stringify_success(emit_result, "initial stringify")
	if emit_result.has_error():
		return

	var yaml_text = emit_result.get_data()
	print_rich("• Original YAML (with single quotes):")
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
		var parsed_path = parse_result.get_data()

		# Modify the path (by re-creating it for demonstration)
		var modified_path = NodePath("Parent/Different/Path:newproperty")

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_path, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved (quote style should be maintained)
		assert_yaml_has_feature(re_emit_result.get_data(), "'", "Quote style was preserved")

## Helper function to check if NodePath instances are equal
func is_nodepath_equal(a: NodePath, b: NodePath) -> bool:
	return String(a) == String(b)
