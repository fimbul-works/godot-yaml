extends BaseTest

const YAML_FILE = "res://addons/yaml/data/supported_syntax.yaml"
const STYLE_FILE = "user://supported_syntax.style.yaml"

var data
var style: YAMLStyle

func _init() -> void:
	icon = "🎨"

func test_style_extraction() -> void:
	var result = YAML.load_file(YAML_FILE, null, true)
	expect(not result.has_error(), result.get_error())

	data = result.get_data()
	style = result.get_style()
	if !style:
		return push_error("Could not extract style")
	if LOG_VERBOSE:
		print_rich("\n[b]Extracted Styles:[/b]\n%s" % style.get_debug_string())

	var save_result = style.save_file(STYLE_FILE)
	expect(not save_result.has_error(), save_result.get_error())

	if save_result.has_error():
		push_error(save_result.get_error())
		return

func test_stringify_with_style() -> void:
	var load_result = YAMLStyle.load_file(STYLE_FILE)
	expect(not load_result.has_error(), load_result.get_error())

	var load_style = load_result.get_data()
	expect_equal(style.hash(), load_style.hash(), "Style hashes should match")

	var stringify_result = YAML.stringify(data, load_style)
	expect(not stringify_result.has_error(), stringify_result.get_error())

	if LOG_VERBOSE:
		var yaml_text = stringify_result.get_data()
		print_rich("\n[b]Stringify with YAMLStyle:[/b]\n%s" % yaml_text)

func test_style_cloning() -> void:
	var style := YAML.create_style()
	style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)
	style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	var cloned_style := style.clone()
	expect_equal(style.get_string_style(), cloned_style.get_string_style(),
		"Cloned style should have same string style")
	expect_equal(style.get_flow_style(), cloned_style.get_flow_style(),
		"Cloned style should have same flow style")

	# Modify the clone, shouldn't affect original
	cloned_style.set_string_style(YAMLStyle.STRING_QUOTE_SINGLE)
	expect_not_equal(style.get_string_style(), cloned_style.get_string_style(),
		"Modifying clone shouldn't affect original")

func test_style_merging() -> void:
	var style1 := YAML.create_style()
	style1.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)

	var style2 := YAML.create_style()
	style2.set_flow_style(YAMLStyle.FLOW_SINGLE)

	style1.merge_with(style2)
	expect_equal(style1.get_string_style(), YAMLStyle.STRING_QUOTE_DOUBLE,
		"Original style properties should be preserved")
	expect_equal(style1.get_flow_style(), YAMLStyle.FLOW_SINGLE,
		"Properties from merged style should be added")

func test_child_styles() -> void:
	var style := YAML.create_style()
	style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)

	var list_style := style.create_child("list")
	list_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	expect_equal(style.get_child("list"), list_style,
		"Should retrieve the same child style")
	expect(style.has_child("list"), "Should have the child style")

	var child_keys := style.list_children()
	expect(child_keys.has("list"), "list_children should include the child key")

func test_get_at_path() -> void:
	var style := YAML.create_style()

	# Create a nested style structure
	var maps_style := style.create_child("maps")
	var items_style := maps_style.create_child("items")
	var first_item_style := items_style.create_child("0")
	first_item_style.set_string_style(YAMLStyle.STRING_LITERAL)

	# Get style at path
	var path_style := style.get_at_path("maps/items/0")
	expect_equal(path_style.get_string_style(), YAMLStyle.STRING_LITERAL,
		"Should get style at specified path")

	# Create missing path
	var new_path_style := style.get_at_path("maps/items/1/properties", true)
	expect_not_equal(new_path_style, null, "Should create missing path nodes")

	# Try to get non-existent path without creating
	var missing_style := style.get_at_path("non/existent/path", false)
	expect_equal(missing_style, null, "Should return null for non-existent path")

func test_propagate_scalar_styles() -> void:
	var parent_style := YAML.create_style()
	parent_style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)
	parent_style.set_integer_format(YAMLStyle.INT_HEX)
	parent_style.set_float_format(YAMLStyle.FLOAT_SCIENTIFIC)

	var child_style := YAML.create_style()
	parent_style.propagate_scalar_styles(child_style)

	expect_equal(child_style.get_string_style(), YAMLStyle.STRING_QUOTE_DOUBLE,
		"String style should be propagated")
	expect_equal(child_style.get_integer_format(), YAMLStyle.INT_HEX,
		"Integer format should be propagated")
	expect_equal(child_style.get_float_format(), YAMLStyle.FLOAT_SCIENTIFIC,
		"Float format should be propagated")

func test_apply_scalar_styles_to_children() -> void:
	var style := YAML.create_style()
	style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)
	style.set_integer_format(YAMLStyle.INT_HEX)

	var child1 := style.create_child("child1")
	var child2 := style.create_child("child2")

	style.apply_scalar_styles_to_children()

	expect_equal(child1.get_string_style(), YAMLStyle.STRING_QUOTE_DOUBLE,
		"String style should be applied to child1")
	expect_equal(child2.get_integer_format(), YAMLStyle.INT_HEX,
		"Integer format should be applied to child2")

func test_various_style_combinations() -> void:
	var data := {
		"string_value": "Test string with \"quotes\" and newlines\nto test",
		"int_value": 255,
		"float_value": 3.14159,
		"list": ["item1", "item2", "item3"],
		"dict": {"key1": "val1", "key2": "val2"}
	}

	var style := YAML.create_style()
	style.set_string_style(YAMLStyle.STRING_LITERAL)
	style.set_integer_format(YAMLStyle.INT_HEX)
	style.set_float_format(YAMLStyle.FLOAT_SCIENTIFIC)

	# List should be compact
	var list_style := style.create_child("list")
	list_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Dict should be expanded
	var dict_style := style.create_child("dict")
	dict_style.set_flow_style(YAMLStyle.FLOW_NONE)

	var result := YAML.stringify(data, style)
	expect(not result.has_error(), result.get_error_message())

	if LOG_VERBOSE:
		print_rich("\n[b]Styled YAML output:[/b]\n%s" % result.get_data())

	# Now parse it back with style detection
	var parse_result := YAML.parse(result.get_data(), null, true)
	expect(not parse_result.has_error(), parse_result.get_error_message())

	var detected_style := parse_result.get_style()
	expect(detected_style != null, "Style should be detected")

	if LOG_VERBOSE and detected_style:
		print_rich("\n[b]Detected Style Tree:[/b]\n%s" % detected_style.get_debug_string())

func test_to_from_dictionary() -> void:
	var style := YAML.create_style()
	style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)

	var child := style.create_child("child")
	child.set_flow_style(YAMLStyle.FLOW_SINGLE)

	var dict := style.to_dictionary()
	expect(dict.has("string"), "Dictionary should contain style properties")
	expect(dict.has("children"), "Dictionary should contain children")

	var rebuilt_style := YAMLStyle.from_dictionary(dict)
	expect_equal(rebuilt_style.get_string_style(), YAMLStyle.STRING_QUOTE_DOUBLE,
		"Rebuilt style should maintain properties")
	expect(rebuilt_style.has_child("child"), "Rebuilt style should have children")

	var rebuilt_hash := rebuilt_style.hash()
	var original_hash := style.hash()
	expect_equal(rebuilt_hash, original_hash, "Style hashes should match")
