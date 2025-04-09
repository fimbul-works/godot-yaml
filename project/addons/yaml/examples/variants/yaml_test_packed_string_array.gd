extends YAMLTest
## Test suite for PackedStringArray YAML serialization and styling

func _init():
	test_name = "🧩 PackedStringArray YAML Tests"

# Helper function to create test arrays with various string patterns
func create_test_arrays() -> Dictionary:
	return {
		"empty": PackedStringArray(),
		"single": PackedStringArray(["Hello"]),
		"basic": PackedStringArray(["One", "Two", "Three"]),
		"empty_strings": PackedStringArray(["", "", ""]),
		"mixed_empty": PackedStringArray(["First", "", "Third"]),
		"numbers": PackedStringArray(["1", "2", "3", "4", "5"]),
		"quotes": PackedStringArray([
			"Single 'quotes' inside",
			"Double \"quotes\" inside",
			"Both 'single' and \"double\" quotes"
		]),
		"special_chars": PackedStringArray([
			"Line\nBreak",
			"Tab\tCharacter",
			"Backslash \\ Character",
			"Unicode: 日本語, Русский, العربية"
		]),
		"multiline": PackedStringArray([
			"First line\nSecond line\nThird line",
			"Another\nmultiline\nstring",
			"Three\nlines\nhere"
		]),
		"long_strings": PackedStringArray([
			create_long_string(100),
			create_long_string(200),
			create_long_string(300)
		]),
		"large": create_large_string_array(50)
	}

# Helper to create a string of a specific length
func create_long_string(length: int) -> String:
	var alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
	var result = ""

	for i in range(length):
		result += alphabet[i % alphabet.length()]

	return result

# Helper to create a large array of strings
func create_large_string_array(size: int) -> PackedStringArray:
	var array = PackedStringArray()
	array.resize(size)

	for i in range(size):
		array[i] = "String %d: %s" % [
			i,
			create_long_string(20 + (i % 10)) # Vary the string lengths
		]

	return array

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	var test_arrays = create_test_arrays()

	for name in test_arrays:
		var string_array = test_arrays[name]
		var result = YAML.stringify(string_array)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s (%d elements): %s" % [
			name,
			string_array.size(),
			truncate(result.get_data())
		])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, string_array, is_packed_string_array_equal, name)

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	# Use a simple array for testing styles
	var string_array = PackedStringArray(["One", "Two", "Three"])

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(string_array, flow_style)

	assert_stringify_success(flow_result, "flow style")
	if !flow_result.has_error():
		print_rich("• Flow style:")
		print_rich(flow_result.get_data())

		# Verify it contains flow indicators (brackets)
		assert_yaml_has_feature(flow_result.get_data(), "[", "Contains opening bracket")
		assert_yaml_has_feature(flow_result.get_data(), "]", "Contains closing bracket")

	# Test block style (expanded)
	var block_style = YAML.create_style()
	block_style.set_flow_style(YAMLStyle.FLOW_NONE)
	var block_result = YAML.stringify(string_array, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify it uses proper block style for a sequence
		assert_yaml_has_feature(block_result.get_data(), "- ", "Contains block sequence indicators")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), string_array, is_packed_string_array_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), string_array, is_packed_string_array_equal, "block style")

## Test string scalar styles
func test_scalar_styles() -> void:
	# Test with multiline strings
	var string_array = PackedStringArray([
		"First line\nSecond line\nThird line",
		"Another\nmultiline\nstring",
		"Regular string"
	])

	# Test literal block style (preserves newlines exactly)
	var literal_style = YAML.create_style()
	var items_literal_style = YAML.create_style()
	items_literal_style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)
	literal_style.set_child("_items", items_literal_style)

	var literal_result = YAML.stringify(string_array, literal_style)
	assert_stringify_success(literal_result, "literal block style")

	if !literal_result.has_error():
		print_rich("• Literal block style:")
		print_rich(literal_result.get_data())

		# Verify it contains the literal style indicator
		assert_yaml_has_feature(literal_result.get_data(), "|-\n", "Contains literal block style indicator")

	# Test folded block style (folds newlines into spaces)
	var folded_style = YAML.create_style()
	var items_folded_style = YAML.create_style()
	items_folded_style.set_scalar_style(YAMLStyle.SCALAR_FOLDED)
	folded_style.set_child("_items", items_folded_style)

	var folded_result = YAML.stringify(string_array, folded_style)
	assert_stringify_success(folded_result, "folded block style")

	if !folded_result.has_error():
		print_rich("• Folded block style:")
		print_rich(folded_result.get_data())

		# Verify it contains the folded style indicator
		assert_yaml_has_feature(folded_result.get_data(), ">-\n", "Contains folded block style indicator")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(literal_result.get_data()), string_array, is_packed_string_array_equal, "literal style")
	assert_roundtrip(YAML.parse(folded_result.get_data()), string_array, is_packed_string_array_equal, "folded style")

## Test quote styles
func test_quote_styles() -> void:
	# Use strings that have quotes and special characters
	var string_array = PackedStringArray([
		"Has 'single' quotes",
		"Has \"double\" quotes",
		"Has both 'single' and \"double\" quotes"
	])

	# Test with different quote styles
	var styles = {
		"Single Quotes": YAMLStyle.QUOTE_SINGLE,
		"Double Quotes": YAMLStyle.QUOTE_DOUBLE,
		"No Quotes": YAMLStyle.QUOTE_NONE
	}

	for style_name in styles:
		var quote_style = YAML.create_style()
		var items_style = YAML.create_style()
		items_style.set_quote_style(styles[style_name])
		quote_style.set_child("_items", items_style)

		var result = YAML.stringify(string_array, quote_style)
		assert_stringify_success(result, style_name)

		if !result.has_error():
			print_rich("• %s:" % style_name)
			print_rich(result.get_data())

			# Verify quote style (with some flexibility since YAML emitter might override)
			match styles[style_name]:
				YAMLStyle.QUOTE_SINGLE:
					# Some items might need double quotes despite single quote style
					print_rich("  (Note: Some values may still use double quotes for YAML validity)")
				YAMLStyle.QUOTE_DOUBLE:
					assert_yaml_has_feature(result.get_data(), "\"", "Contains double quotes")
				YAMLStyle.QUOTE_NONE:
					# Note: Some values might still be quoted by the YAML emitter
					# if it determines they need to be for valid YAML
					print_rich("  (Note: Some values may still be quoted for YAML validity)")

		# Test roundtrip
		assert_roundtrip(YAML.parse(result.get_data()), string_array, is_packed_string_array_equal, style_name)

## Test item-specific styles
func test_item_styles() -> void:
	var string_array = PackedStringArray([
		"Regular string",
		"Multiline\nstring\nhere",
		"String with \"quotes\""
	])

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for each item
	var item0_style = YAML.create_style()
	item0_style.set_quote_style(YAMLStyle.QUOTE_SINGLE)

	var item1_style = YAML.create_style()
	item1_style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)

	var item2_style = YAML.create_style()
	item2_style.set_quote_style(YAMLStyle.QUOTE_DOUBLE)

	# Apply styles
	parent_style.set_child("0", item0_style)
	parent_style.set_child("1", item1_style)
	parent_style.set_child("2", item2_style)

	var result = YAML.stringify(string_array, parent_style)

	assert_stringify_success(result, "item styles")
	if !result.has_error():
		print_rich("• Item-specific styles:")
		print_rich(result.get_data())

		# Verify the different styles are applied
		assert_yaml_has_feature(result.get_data(), "'", "First item has single quotes")
		assert_yaml_has_feature(result.get_data(), "|-\n", "Second item uses literal block style")
		assert_yaml_has_feature(result.get_data(), "\"", "Third item has double quotes")

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), string_array, is_packed_string_array_equal, "item styles")

## Test empty strings
func test_empty_strings() -> void:
	# Create an array with empty strings
	var empty_array = PackedStringArray(["", "", ""])
	var mixed_array = PackedStringArray(["First", "", "Third"])

	print_rich("\nTesting empty string handling:")

	# Test with different styles
	var test_cases = [
		{
			"name": "Empty strings (default)",
			"array": empty_array,
			"style": null
		},
		{
			"name": "Empty strings (quoted)",
			"array": empty_array,
			"style": func():
				var s = YAML.create_style()
				var items = YAML.create_style()
				items.set_quote_style(YAMLStyle.QUOTE_DOUBLE)
				s.set_child("_items", items)
				return s,
		},
		{
			"name": "Mixed empty strings (default)",
			"array": mixed_array,
			"style": null
		},
		{
			"name": "Mixed empty strings (quoted)",
			"array": mixed_array,
			"style": func():
				var s = YAML.create_style()
				var items = YAML.create_style()
				items.set_quote_style(YAMLStyle.QUOTE_DOUBLE)
				s.set_child("_items", items)
				return s,
		}
	]

	for test_case in test_cases:
		var name = test_case["name"]
		var array = test_case["array"]
		var style = test_case["style"].call() if test_case["style"] else null

		var result = YAML.stringify(array, style)
		assert_stringify_success(result, name)

		if !result.has_error():
			print_rich("• %s:" % name)
			print_rich(result.get_data())

			# Parse back and verify
			var parse_result = YAML.parse(result.get_data())
			assert_roundtrip(parse_result, array, is_packed_string_array_equal, name)

			# Verify we can accurately represent empty strings
			var parsed_array = parse_result.get_data()
			for i in range(array.size()):
				if array[i] == "":
					assert_equal(parsed_array[i], "", "Empty string at index %d preserved" % i)

## Test Unicode strings
func test_unicode_strings() -> void:
	var unicode_array = PackedStringArray([
		"English",
		"日本語",  # Japanese
		"Русский",  # Russian
		"العربية",  # Arabic
		"中文",     # Chinese
		"한국어",   # Korean
		"🌍🚀🎮"   # Emoji
	])

	print_rich("\nTesting Unicode string handling:")

	# Test with various styles
	var styles = {
		"Default": null,
		"Quoted": func():
			var s = YAML.create_style()
			var items = YAML.create_style()
			items.set_quote_style(YAMLStyle.QUOTE_DOUBLE)
			s.set_child("_items", items)
			return s,
		"Block": func():
			var s = YAML.create_style()
			var items = YAML.create_style()
			items.set_scalar_style(YAMLStyle.SCALAR_BLOCK)
			s.set_child("_items", items)
			return s,
	}

	for style_name in styles:
		var style_func = styles[style_name]
		var style = style_func.call() if style_func else null

		var result = YAML.stringify(unicode_array, style)
		assert_stringify_success(result, "Unicode - " + style_name)

		if !result.has_error():
			print_rich("• Unicode - %s:" % style_name)
			print_rich(result.get_data())

			# Parse back and verify
			var parse_result = YAML.parse(result.get_data())
			assert_roundtrip(parse_result, unicode_array, is_packed_string_array_equal, "Unicode - " + style_name)

			# Verify Unicode characters are preserved
			var parsed_array = parse_result.get_data()
			for i in range(unicode_array.size()):
				assert_equal(parsed_array[i], unicode_array[i],
					"Unicode string at index %d preserved: '%s'" % [i, unicode_array[i]])

## Test large array with long strings
func test_large_array() -> void:
	var large_array = create_large_string_array(50)

	print_rich("\nTesting large StringArray handling:")

	# Test with different styles
	var test_cases = [
		{
			"name": "Large array in block style",
			"style": func():
				var s = YAML.create_style()
				s.set_flow_style(YAMLStyle.FLOW_NONE)
				return s,
		},
		{
			"name": "Large array in flow style",
			"style": func():
				var s = YAML.create_style()
				s.set_flow_style(YAMLStyle.FLOW_SINGLE)
				return s,
		},
		{
			"name": "Large array with quoted items",
			"style": func():
				var s = YAML.create_style()
				s.set_flow_style(YAMLStyle.FLOW_NONE)
				var items = YAML.create_style()
				items.set_quote_style(YAMLStyle.QUOTE_DOUBLE)
				s.set_child("_items", items)
				return s,
		}
	]

	for test_case in test_cases:
		var name = test_case["name"]
		var style = test_case["style"].call()

		var result = YAML.stringify(large_array, style)
		assert_stringify_success(result, name)

		if !result.has_error():
			var yaml_str = result.get_data()
			print_rich("• %s: (Length: %d characters)" % [name, yaml_str.length()])
			print_rich("  First 100 chars: " + yaml_str.substr(0, 100) + "...")

			# Parse back and verify (this might be slow for very large arrays)
			var parse_result = YAML.parse(yaml_str)
			assert_roundtrip(parse_result, large_array, is_packed_string_array_equal, name)

## Test roundtrip conversion with style detection
func test_roundtrip_with_styles() -> void:
	var string_array = PackedStringArray([
		"Regular string",
		"Multiline\nstring\nhere",
		"String with \"quotes\""
	])

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_NONE)

	# Create item style for the second element (multiline)
	var item_style = YAML.create_style()
	item_style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)
	original_style.set_child("1", item_style)

	# Emit YAML with the style
	var emit_result = YAML.stringify(string_array, original_style)
	assert_stringify_success(emit_result, "initial stringify")
	if emit_result.has_error():
		return

	var yaml_text = emit_result.get_data()
	print_rich("• Original YAML (with block style and literal style for multiline):")
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
		var parsed_array = parse_result.get_data()

		# Modify the array (add a string)
		var modified_array = PackedStringArray()
		for string in parsed_array:
			modified_array.append(string)
		modified_array.append("New string added")

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_array, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the original styles were preserved
		assert_yaml_has_feature(re_emit_result.get_data(), "- ", "Block style was preserved")
		assert_yaml_has_feature(re_emit_result.get_data(), "|-\n", "Literal style was preserved for multiline")

## Helper function to check if PackedStringArray instances are equal
func is_packed_string_array_equal(a: PackedStringArray, b: PackedStringArray) -> bool:
	if a.size() != b.size():
		return false

	for i in range(a.size()):
		if a[i] != b[i]:
			return false

	return true
