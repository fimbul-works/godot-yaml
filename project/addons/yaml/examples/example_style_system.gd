extends ExampleBase

const YAML_FILE = "res://addons/yaml/examples/data/supported_syntax.yaml"
const STYLE_FILE = "user://supported_syntax.style.yaml"

var data
var style: YAMLStyle

func _init() -> void:
	icon = "🎨"

func run_examples() -> void:
	run_example("Style Extraction", style_extraction)
	run_example("Stringify with Style", stringify_with_style)
	run_example("Style Cloning", style_cloning)
	run_example("Style Merging", style_merging)
	run_example("Child Styles", child_styles)
	run_example("Path-based Styles", get_at_path)
	run_example("Style Propagation", propagate_scalar_styles)
	run_example("Various Style Combinations", various_style_combinations)
	run_example("Style Serialization", to_from_dictionary)

func style_extraction() -> void:
	log_info("Loading YAML file with style detection...")

	var result = YAML.load_file(YAML_FILE, null, true)
	if result.has_error():
		log_error("Failed to load file: " + result.get_error())
		return

	data = result.get_data()
	style = result.get_style()

	if !style:
		log_error("Could not extract style")
		return

	log_success("Style extracted successfully")

	if LOG_VERBOSE:
		log_result("Extracted Styles:\n" + style.get_debug_string())

	log_info("Saving style to file: " + STYLE_FILE)
	var save_result = style.save_file(STYLE_FILE)

	if save_result.has_error():
		log_error("Failed to save style: " + save_result.get_error())
		return

	log_success("Style saved to file")

func stringify_with_style() -> void:
	log_info("Loading style from file...")

	var load_result = YAMLStyle.load_file(STYLE_FILE)
	if load_result.has_error():
		log_error("Failed to load style: " + load_result.get_error())
		return

	var load_style = load_result.get_style()

	if style && load_style.hash() == style.hash():
		log_success("Loaded style matches the original style")
	else:
		log_warning("Loaded style does not match the original style")

	if data == null:
		data = {"test": "value"}

	log_info("Stringifying data with loaded style...")
	var stringify_result = YAML.stringify(data, load_style)

	if stringify_result.has_error():
		log_error("Stringify failed: " + stringify_result.get_error())
		return

	log_success("Data stringified with style")

	if LOG_VERBOSE:
		log_result("Styled YAML output:\n" + stringify_result.get_data())

func style_cloning() -> void:
	log_info("Creating and configuring base style...")

	var style := YAML.create_style()
	style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)
	style.set_flow_style(YAMLStyle.FLOW_NONE)

	log_info("Cloning style...")
	var cloned_style := style.clone()

	if style.get_string_style() == cloned_style.get_string_style() && \
	   style.get_flow_style() == cloned_style.get_flow_style():
		log_success("Cloned style has same properties as original")
	else:
		log_error("Cloned style properties don't match original")

	log_info("Modifying the cloned style...")
	cloned_style.set_string_style(YAMLStyle.STRING_QUOTE_SINGLE)

	if style.get_string_style() != cloned_style.get_string_style():
		log_success("Modifying clone doesn't affect original")
	else:
		log_error("Modifying clone affected original")

	if LOG_VERBOSE:
		var test_data = {"message": "Hello, World!"}

		var orig_result = YAML.stringify(test_data, style)
		var clone_result = YAML.stringify(test_data, cloned_style)

		log_result("Original style output:\n" + orig_result.get_data())
		log_result("Cloned style output:\n" + clone_result.get_data())

func style_merging() -> void:
	log_info("Creating two different styles...")

	var style1 := YAML.create_style()
	style1.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)

	var style2 := YAML.create_style()
	style2.set_flow_style(YAMLStyle.FLOW_SINGLE)

	log_info("Merging style2 into style1...")
	style1.merge_with(style2)

	if style1.get_string_style() == YAMLStyle.STRING_QUOTE_DOUBLE:
		log_success("Original style properties preserved")
	else:
		log_error("Original style properties were lost")

	if style1.get_flow_style() == YAMLStyle.FLOW_SINGLE:
		log_success("Properties from merged style were added")
	else:
		log_error("Properties from merged style were not added")

	if LOG_VERBOSE:
		var test_data = {
			"message": "Hello, World!",
			"items": ["one", "two", "three"]
		}

		var result = YAML.stringify(test_data, style1)
		log_result("Merged style output:\n" + result.get_data())

func child_styles() -> void:
	log_info("Creating parent style with child styles...")

	var style := YAML.create_style()
	style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)

	log_info("Creating child style for 'list' key...")
	var list_style := style.create_child("list")
	list_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	if style.get_child("list") == list_style:
		log_success("get_child() retrieves the correct child style")
	else:
		log_error("get_child() failed to retrieve the correct child style")

	if style.has_child("list"):
		log_success("has_child() correctly identifies existing child")
	else:
		log_error("has_child() failed to identify existing child")

	var child_keys := style.list_children()
	if child_keys.has("list"):
		log_success("list_children() includes the child key")
	else:
		log_error("list_children() failed to include the child key")

	if LOG_VERBOSE:
		var test_data = {
			"name": "Example",
			"list": ["one", "two", "three"]
		}

		var result = YAML.stringify(test_data, style)
		log_result("Output with child styles:\n" + result.get_data())

func get_at_path() -> void:
	log_info("Creating nested style structure...")

	var style := YAML.create_style()

	# Create a nested style structure
	var maps_style := style.create_child("maps")
	var items_style := maps_style.create_child("items")
	var first_item_style := items_style.create_child("0")
	first_item_style.set_string_style(YAMLStyle.STRING_LITERAL)

	log_info("Getting style at path 'maps/items/0'...")
	var path_style := style.get_at_path("maps/items/0")

	if path_style && path_style.get_string_style() == YAMLStyle.STRING_LITERAL:
		log_success("get_at_path() correctly retrieved the style")
	else:
		log_error("get_at_path() failed to retrieve the correct style")

	log_info("Creating missing path 'maps/items/1/properties'...")
	var new_path_style := style.get_at_path("maps/items/1/properties", true)

	if new_path_style != null:
		log_success("Successfully created missing path nodes")
	else:
		log_error("Failed to create missing path nodes")

	log_info("Getting non-existent path without creating...")
	var missing_style := style.get_at_path("non/existent/path", false)

	if missing_style == null:
		log_success("Correctly returned null for non-existent path")
	else:
		log_error("Incorrectly returned non-null for non-existent path")

	if LOG_VERBOSE:
		var test_data = {
			"maps": {
				"items": [
					{"name": "Item 1", "description": "Line 1\nLine 2\nLine 3"},
					{"name": "Item 2", "properties": {"a": 1, "b": 2}}
				]
			}
		}

		var result = YAML.stringify(test_data, style)
		log_result("Output with path-based styles:\n" + result.get_data())

func propagate_scalar_styles() -> void:
	log_info("Creating parent style with scalar formats...")

	var parent_style := YAML.create_style()
	parent_style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)
	parent_style.set_integer_format(YAMLStyle.INT_HEX)
	parent_style.set_float_format(YAMLStyle.FLOAT_SCIENTIFIC)

	log_info("Creating child style and propagating scalar styles...")
	var child_style := YAML.create_style()
	parent_style.propagate_scalar_styles(child_style)

	if child_style.get_string_style() == YAMLStyle.STRING_QUOTE_DOUBLE:
		log_success("String style was propagated")
	else:
		log_error("String style was not propagated")

	if child_style.get_integer_format() == YAMLStyle.INT_HEX:
		log_success("Integer format was propagated")
	else:
		log_error("Integer format was not propagated")

	if child_style.get_float_format() == YAMLStyle.FLOAT_SCIENTIFIC:
		log_success("Float format was propagated")
	else:
		log_error("Float format was not propagated")

	if LOG_VERBOSE:
		var test_data = {
			"text": "Hello, World!",
			"number": 255,
			"decimal": 3.14159
		}

		var result = YAML.stringify(test_data, child_style)
		log_result("Output with propagated styles:\n" + result.get_data())

func various_style_combinations() -> void:
	log_info("Creating data with various types...")

	var data := {
		"string_value": "Test string with \"quotes\" and newlines\nto test",
		"int_value": 255,
		"float_value": 3.14159,
		"list": ["item1", "item2", "item3"],
		"dict": {"key1": "val1", "key2": "val2"}
	}

	log_info("Creating style with various formatting options...")
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

	log_info("Stringifying with custom styles...")
	var result := YAML.stringify(data, style)

	if result.has_error():
		log_error("Stringify failed: " + result.get_error())
		return

	log_success("Data stringified with custom styles")

	if LOG_VERBOSE:
		log_result("Styled YAML output:\n" + result.get_data())

	log_info("Parsing styled output with style detection...")
	var parse_result := YAML.parse(result.get_data(), null, true)

	if parse_result.has_error():
		log_error("Parse failed: " + parse_result.get_error())
		return

	var detected_style := parse_result.get_style()

	if detected_style != null:
		log_success("Style was detected during parsing")

		if LOG_VERBOSE:
			log_result("Detected Style Tree:\n" + detected_style.get_debug_string())
	else:
		log_error("Style was not detected during parsing")

func to_from_dictionary() -> void:
	log_info("Creating style for serialization...")

	var style := YAML.create_style()
	style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)

	var child := style.create_child("child")
	child.set_flow_style(YAMLStyle.FLOW_SINGLE)

	log_info("Converting style to dictionary...")
	var dict := style.to_dictionary()

	if dict.has("string") && dict.has("children"):
		log_success("Dictionary contains style properties and children")
	else:
		log_error("Dictionary missing expected keys")

	log_info("Rebuilding style from dictionary...")
	var rebuilt_style := YAMLStyle.from_dictionary(dict)

	if rebuilt_style.get_string_style() == YAMLStyle.STRING_QUOTE_DOUBLE:
		log_success("Rebuilt style maintained properties")
	else:
		log_error("Rebuilt style lost properties")

	if rebuilt_style.has_child("child"):
		log_success("Rebuilt style maintains children")
	else:
		log_error("Rebuilt style lost children")

	var rebuilt_hash := rebuilt_style.hash()
	var original_hash := style.hash()

	if rebuilt_hash == original_hash:
		log_success("Style hashes match")
	else:
		log_error("Style hashes don't match")

	if LOG_VERBOSE:
		log_result("Style dictionary:\n" + str(dict))
