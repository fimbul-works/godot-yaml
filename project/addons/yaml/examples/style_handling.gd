extends YAMLTest
## Test suite for YAML style handling and customization options

# Dictionary with test data for style tests
var test_data = {
	"basic_map": {
		"key1": "value1",
		"key2": "value2"
	},
	"basic_sequence": [1, 2, 3, 4, 5],
	"mixed_types": {
		"string": "Hello World",
		"multiline": "line 1\nline 2\nline 3",
		"number": 42,
		"float": 3.14159,
		"boolean": true,
		"list": [10, 20, 30],
		"nested": {
			"a": 1,
			"b": 2
		}
	},
	"integers": {
		"decimal": 255,
		"byte": 15,
		"large": 1048576  # 2^20
	},
	"floats": {
		"pi": 3.14159,
		"small": 0.000314159,
		"large": 31415.9
	}
}

func _init():
	test_name = "YAML Style Handling Tests"

## Test style detection and preservation
func test_style_detection() -> void:
	print_rich("\n[b]Testing Style Detection:[/b]")

	# Create sample YAML with various styling
	var yaml_text = """
# Sample YAML with multiple style elements
scalar: plain scalar
quoted: "quoted string"
sequence:
  - item1
  - item2
  - item3
compact: [1,2,3]
multiline: |
  This is a multiline
  literal block
  that preserves newlines
folded: >
  This is a folded
  multiline block
  that folds newlines
nested:
  map: {key1: value1, key2: value2}
  indented:
	sub1: value
	sub2: value
"""

	# Parse with style detection enabled
	var result = YAML.parse(yaml_text, true)  # true enables style detection
	assert_parse_success(result, "parse with style detection")
	if result.has_error():
		return

	# Check if style was detected
	if !result.has_style():
		print_rich("[color=yellow]⚠ No style was detected[/color]")
	else:
		print_rich("[color=green]✓ Style detected successfully[/color]")

		# Get the detected style and data
		var style = result.get_style()
		var data = result.get_data()

		# Modify the data slightly
		data.sequence.append("new_item")
		data.scalar = "modified scalar"

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(data, style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify style was preserved
		assert_yaml_has_feature(re_emit_result.get_data(), "multiline: |", "Literal block style was preserved")
		assert_yaml_has_feature(re_emit_result.get_data(), "folded: >", "Folded block style was preserved")
		assert_yaml_has_feature(re_emit_result.get_data(), "[1,2,3]", "Flow sequence style was preserved")

## Test creating and applying custom styles
func test_style_customization() -> void:
	var data = test_data.mixed_types.duplicate(true)

	# Test default style (reference)
	var default_result = YAML.stringify(data)
	assert_stringify_success(default_result, "default style")
	if default_result.has_error():
		return

	print_rich("\n• Default style:")
	print_rich(default_result.get_data())

	# Test custom root style with child styles
	var root_style = YAML.create_style()

	var string_style := YAML.create_style()
	string_style.set_quote_style(YAMLStyle.QUOTE_DOUBLE) # Use " for string
	root_style.set_child("string", string_style)

	var multiline_style = YAML.create_style()
	multiline_style.set_scalar_style(YAMLStyle.SCALAR_LITERAL) # Use | for multiline strings
	root_style.set_child("multiline", multiline_style)

	var nested_style = YAML.create_style()
	nested_style.set_flow_style(YAMLStyle.FLOW_NONE) # Use block style for nested
	root_style.set_child("nested", nested_style)

	var list_style = YAML.create_style()
	list_style.set_flow_style(YAMLStyle.FLOW_SINGLE) # Use flow style for list
	root_style.set_child("list", list_style)

	var custom_result = YAML.stringify(data, root_style)
	assert_stringify_success(custom_result, "custom style")
	if custom_result.has_error():
		return

	print_rich("\n• Custom style with child styles:")
	print_rich(custom_result.get_data())

	var parse_result = YAML.parse(custom_result.get_data())
	# Verify style features
	assert_yaml_has_feature(custom_result.get_data(), "multiline: |", "Literal style applied")
	assert_yaml_has_feature(custom_result.get_data(), "\"", "Double quotes applied")
	assert_yaml_has_feature(custom_result.get_data(), "[", "Flow style applied to list")

	# Parse the results to ensure it's still valid
	var reparse_result = YAML.parse(custom_result.get_data())
	assert_parse_success(reparse_result, "parse styled output")
	if reparse_result.has_error():
		return

	# Verify data is preserved
	assert_true(
		is_deep_equal(data, reparse_result.get_data()),
		"Data integrity is maintained through custom styling"
	)

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var data = {
		"mapping": {"key1": "value1", "key2": "value2"},
		"sequence": [1, 2, 3, 4, 5]
	}

	# Test block style (expanded)
	var block_style = YAML.create_style()
	block_style.set_flow_style(YAMLStyle.FLOW_NONE)
	var block_result = YAML.stringify(data, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("\n• Block style:")
		print_rich(block_result.get_data())

		# Verify it has indentation for sequences and maps
		assert_yaml_has_feature(block_result.get_data(), "mapping:", "Contains mapping key")
		assert_yaml_has_feature(block_result.get_data(), "  key1:", "Contains indented mapping keys")
		assert_yaml_lacks_feature(block_result.get_data(), "{", "Lacks flow indicators for mapping")
		assert_yaml_lacks_feature(block_result.get_data(), "[", "Lacks flow indicators for sequence")

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(data, flow_style)

	assert_stringify_success(flow_result, "flow style")
	if !flow_result.has_error():
		print_rich("\n• Flow style:")
		print_rich(flow_result.get_data())

		# Verify it contains flow indicators (brackets)
		assert_yaml_has_feature(flow_result.get_data(), "{", "Contains opening brace")
		assert_yaml_has_feature(flow_result.get_data(), "}", "Contains closing brace")
		assert_yaml_has_feature(flow_result.get_data(), "[", "Contains opening bracket")
		assert_yaml_has_feature(flow_result.get_data(), "]", "Contains closing bracket")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(block_result.get_data()), data, is_deep_equal, "block style")
	assert_roundtrip(YAML.parse(flow_result.get_data()), data, is_deep_equal, "flow style")

## Test different scalar styles
func test_scalar_styles() -> void:
	var multiline_text = "line 1\nline 2\nline 3\nline 4"
	var data = {"text": multiline_text}

	# Test plain style
	var plain_style = YAML.create_style()
	plain_style.set_scalar_style(YAMLStyle.SCALAR_PLAIN)
	var plain_result = YAML.stringify(data, plain_style)

	assert_stringify_success(plain_result, "plain style")
	if !plain_result.has_error():
		print_rich("\n• Plain scalar style:")
		print_rich(plain_result.get_data())

	# Test literal style (|) - preserves newlines
	var literal_style = YAML.create_style()
	literal_style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)
	var literal_result = YAML.stringify(data, literal_style)

	assert_stringify_success(literal_result, "literal style")
	if !literal_result.has_error():
		print_rich("\n• Literal scalar style (|):")
		print_rich(literal_result.get_data())

		# Verify it contains the literal style indicator
		assert_yaml_has_feature(literal_result.get_data(), "text: |", "Contains literal block style indicator")
		assert_yaml_has_feature(literal_result.get_data(), "  line 1", "Contains indented content with preserved newlines")

	# Test folded style (>) - folds newlines to spaces
	var folded_style = YAML.create_style()
	folded_style.set_scalar_style(YAMLStyle.SCALAR_FOLDED)
	var folded_result = YAML.stringify(data, folded_style)

	assert_stringify_success(folded_result, "folded style")
	if !folded_result.has_error():
		print_rich("\n• Folded scalar style (>):")
		print_rich(folded_result.get_data())

		# Verify it contains the folded style indicator
		assert_yaml_has_feature(folded_result.get_data(), "text: >", "Contains folded block style indicator")

	# Test roundtrip for all styles
	assert_roundtrip(YAML.parse(plain_result.get_data()), data, is_deep_equal, "plain style")
	assert_roundtrip(YAML.parse(literal_result.get_data()), data, is_deep_equal, "literal style")
	assert_roundtrip(YAML.parse(folded_result.get_data()), data, is_deep_equal, "folded style")

## Test different number formats
func test_number_formats() -> void:
	var data = test_data.integers.duplicate(true)

	print_rich("\n[b]Testing Number Formats:[/b]")

	# Test different number formats
	var formats = [
		{"name": "Decimal", "format": YAMLStyle.NUM_DECIMAL},
		{"name": "Hexadecimal", "format": YAMLStyle.NUM_HEX},
		{"name": "Octal", "format": YAMLStyle.NUM_OCTAL},
		{"name": "Binary", "format": YAMLStyle.NUM_BINARY}
	]

	for format_info in formats:
		var style = YAML.create_style()
		style.set_number_format(format_info.format)

		var result = YAML.stringify(data, style)
		assert_stringify_success(result, format_info.name + " format")

		if !result.has_error():
			print_rich("\n• %s format:" % format_info.name)
			print_rich(result.get_data())

			# Verify format specific markers (except decimal which has no special marker)
			if format_info.format == YAMLStyle.NUM_HEX:
				assert_yaml_has_feature(result.get_data(), "0x", "Contains hex marker")
			elif format_info.format == YAMLStyle.NUM_OCTAL:
				assert_yaml_has_feature(result.get_data(), "0o", "Contains octal marker")
			elif format_info.format == YAMLStyle.NUM_BINARY:
				assert_yaml_has_feature(result.get_data(), "0b", "Contains binary marker")

			# Test roundtrip
			var parse_result = YAML.parse(result.get_data())
			assert_roundtrip(parse_result, data, is_deep_equal, format_info.name + " format")

	# Test float formats
	data = test_data.floats.duplicate(true)

	var float_formats = [
		{"name": "Decimal", "format": YAMLStyle.NUM_DECIMAL},
		{"name": "Scientific", "format": YAMLStyle.NUM_SCIENTIFIC}
	]

	for format_info in float_formats:
		var style = YAML.create_style()
		style.set_number_format(format_info.format)

		var result = YAML.stringify(data, style)
		assert_stringify_success(result, format_info.name + " format for floats")

		if !result.has_error():
			print_rich("\n• %s format for floats:" % format_info.name)
			print_rich(result.get_data())

			# For scientific format, look for exponent notation
			if format_info.format == YAMLStyle.NUM_SCIENTIFIC:
				assert_yaml_has_feature(result.get_data(), "e", "Contains scientific notation")

			# Test roundtrip
			var parse_result = YAML.parse(result.get_data())
			assert_roundtrip(parse_result, data, is_deep_equal, format_info.name + " format for floats")

## Test quote styles
func test_quote_styles() -> void:
	var data = {
		"simple": "simple text",
		"with_quotes": "text with \"quotes\" inside",
		"with_special": "text with special : characters",
		"multiline": "line 1\nline 2"
	}

	# Test no quotes style
	var no_quotes_style = YAML.create_style()
	no_quotes_style.set_quote_style(YAMLStyle.QUOTE_NONE)
	var no_quotes_result = YAML.stringify(data, no_quotes_style)

	assert_stringify_success(no_quotes_result, "no quotes style")
	if !no_quotes_result.has_error():
		print_rich("\n• No quotes style:")
		print_rich(no_quotes_result.get_data())

	# Test single quotes style
	var single_quotes_style = YAML.create_style()
	single_quotes_style.set_quote_style(YAMLStyle.QUOTE_SINGLE)
	var single_quotes_result = YAML.stringify(data, single_quotes_style)

	assert_stringify_success(single_quotes_result, "single quotes style")
	if !single_quotes_result.has_error():
		print_rich("\n• Single quotes style:")
		print_rich(single_quotes_result.get_data())

		# Verify it contains single quotes
		assert_yaml_has_feature(single_quotes_result.get_data(), "'", "Contains single quotes")

	# Test double quotes style
	var double_quotes_style = YAML.create_style()
	double_quotes_style.set_quote_style(YAMLStyle.QUOTE_DOUBLE)
	var double_quotes_result = YAML.stringify(data, double_quotes_style)

	assert_stringify_success(double_quotes_result, "double quotes style")
	if !double_quotes_result.has_error():
		print_rich("\n• Double quotes style:")
		print_rich(double_quotes_result.get_data())

		# Verify it contains double quotes
		assert_yaml_has_feature(double_quotes_result.get_data(), "\"", "Contains double quotes")

	# Test roundtrip for all styles
	assert_roundtrip(YAML.parse(no_quotes_result.get_data()), data, is_deep_equal, "no quotes style")
	assert_roundtrip(YAML.parse(single_quotes_result.get_data()), data, is_deep_equal, "single quotes style")
	assert_roundtrip(YAML.parse(double_quotes_result.get_data()), data, is_deep_equal, "double quotes style")

## Helper function to deeply compare values of any type
func is_deep_equal(a: Variant, b: Variant, epsilon: float = 0.00001) -> bool:
	# Handle different types
	if typeof(a) != typeof(b):
		return false

	match typeof(a):
		TYPE_ARRAY:
			if a.size() != b.size():
				return false
			for i in range(a.size()):
				if not is_deep_equal(a[i], b[i]):
					return false
			return true

		TYPE_DICTIONARY:
			if a.size() != b.size():
				return false
			for key in a:
				if not b.has(key) or not is_deep_equal(a[key], b[key]):
					return false
			return true

		TYPE_FLOAT:
			return abs(a - b) < epsilon

		_:  # Default case for other types
			return a == b
