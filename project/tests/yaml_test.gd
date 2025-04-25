class_name YAMLTest extends Node2D
## Base class for YAML test suites with auto-test discovery and enhanced helpers

var test_name: String = "Unnamed"
var passed_tests := 0
var total_tests := 0
var test_times := {}

func _before_each() -> void:
	# Child class can override this
	pass

func _after_each() -> void:
	# Child class can override this
	pass

func _ready():
	# If the node is not visible, do not run the test
	if not visible:
		return

	# If the parent is not visible, do not run the test
	if get_parent() and not get_parent().visible:
		return

	# If the scene is not visible, do not run the test
	if owner and not owner.visible:
		return

	print_rich("\n[b]🧪 %s Tests[/b]" % test_name)

	# Discover the test methods
	var methods = get_method_list()
	var test_methods = methods.filter(func(method): return method.name.begins_with("test_"))

	# Sort methods by name for consistent execution order
	test_methods.sort_custom(func(a, b): return a.name < b.name)

	var suite_start := Time.get_ticks_usec()

	# Execute each test method
	for method in test_methods:
		print_rich("\n[b]Running: %s %s[/b]" % [test_name, method.name.replace("test_", "").replace("_", " ").capitalize()])
		var test_start := Time.get_ticks_usec()
		_before_each()
		call(method.name)
		_after_each()
		var test_elapsed := Time.get_ticks_usec() - test_start
		test_times[method] = test_elapsed

	# Report time
	var elapsed: float = Time.get_ticks_usec() - suite_start
	var tunit := "µsec"
	if elapsed > 1000:
		elapsed /= 1000
		tunit = "ms"

	print_rich("\n[b]%s Tests Results: %d/%d tests passed in %d %s[/b]" % [test_name, passed_tests, total_tests, elapsed, tunit])

## Assert values are equal
func assert_equal(actual, expected, message: String) -> void:
	total_tests += 1
	var condition = actual == expected
	if condition:
		passed_tests += 1
		print_rich("[color=green]✓ %s[/color]" % message)
	else:
		print_rich("[color=red]✗ %s[/color]" % message)
		print_rich("  Expected: %s" % expected)
		print_rich("  Actual: %s" % actual)
	assert(condition, message)

## Assert values are not equal
func assert_not_equal(actual, expected, message: String) -> void:
	total_tests += 1
	var condition = actual != expected
	if condition:
		passed_tests += 1
		print_rich("[color=green]✓ %s[/color]" % message)
	else:
		print_rich("[color=red]✗ %s[/color]" % message)
		print_rich("  Expected different than: %s" % expected)
		print_rich("  Actual: %s" % actual)
	assert(condition, message)

## Assert a condition is true
func assert_true(condition: bool, message: String) -> void:
	total_tests += 1
	if condition:
		passed_tests += 1
		print_rich("[color=green]✓ %s[/color]" % message)
	else:
		print_rich("[color=red]✗ %s[/color]" % message)
	assert(condition, message)

## Assert a condition is false
func assert_false(condition: bool, message: String) -> void:
	total_tests += 1
	if not condition:
		passed_tests += 1
		print_rich("[color=green]✓ %s[/color]" % message)
	else:
		print_rich("[color=red]✗ %s[/color]" % message)
	assert(not condition, message)

## Helper for custom equality checks
func assert_custom_equal(actual, expected, equal_func: Callable, message: String) -> void:
	total_tests += 1
	var condition = equal_func.call(actual, expected)
	if condition:
		passed_tests += 1
		print_rich("[color=green]✓ %s[/color]" % message)
	else:
		print_rich("[color=red]✗ %s[/color]" % message)
		print_rich("  Expected: %s" % expected)
		print_rich("  Actual: %s" % actual)
	assert(condition, message)

## Test YAML parse result
func assert_parse_success(result, message: String) -> void:
	assert_false(result.has_error(), "%s: Parse succeeded" % message)
	if result.has_error():
		print_rich("  Error: %s" % result.get_error_message())

## Test YAML stringify result
func assert_stringify_success(result, message: String) -> void:
	assert_false(result.has_error(), "%s: Stringify succeeded" % message)
	if result.has_error():
		print_rich("  Error: %s" % result.get_error_message())

## Test YAML parsing result with custom equality check
func assert_roundtrip(result, expected_value, equal_func: Callable, message: String) -> void:
	if result.has_error():
		print_rich("  Error: %s" % result.get_error_message())
		assert_false(result.has_error(), "%s: Parse succeeded" % message)
		return

	var parsed_value = result.get_data()
	assert_custom_equal(parsed_value, expected_value, equal_func, "%s: Roundtrip matches" % message)
	print("") # Add an empty line between cases

## Helper to test if a YAML string has a specific formatting feature
func assert_yaml_has_feature(yaml_string: String, feature: String, message: String) -> void:
	var has_feature = yaml_string.find(feature) != -1
	assert_true(has_feature, message)

## Helper to test if a YAML string doesn't have a specific formatting feature
func assert_yaml_lacks_feature(yaml_string: String, feature: String, message: String) -> void:
	var lacks_feature = yaml_string.find(feature) == -1
	assert_true(lacks_feature, message)

## Test a YAML parse error is returned
func assert_parse_error(yaml_string: String, message: String) -> void:
	var result = YAML.parse(yaml_string)
	assert_true(result.has_error(), message)
	if result.has_error():
		print_rich("  [i]Error:[/i] %s" % result.get_error_message())
		print_rich("  [i]Location:[/i] line %d, column %d" % [result.get_error_line(), result.get_error_column()])

## Test a stringify error is returned
func assert_stringify_error(value, message: String) -> void:
	var result = YAML.stringify(value)
	assert_true(result.has_error(), message)
	if result.has_error():
		print_rich("  [i]Error:[/i] %s" % result.get_error_message())

## Test YAML validation
func assert_validate(yaml_string: String, should_be_valid: bool, message: String) -> void:
	var result = YAML.validate(yaml_string)
	if should_be_valid:
		assert_false(result.has_error(), message)
	else:
		assert_true(result.has_error(), message)
		if result.has_error():
			print_rich("  [i]Error:[/i] %s" % result.get_error_message())
			print_rich("  [i]Location:[/i] line %d, column %d" % [result.get_error_line(), result.get_error_column()])

## Run standard serialization test for all test values
func run_basic_serialization_test(test_values: Dictionary, equality_func: Callable) -> void:
	for name in test_values:
		var value = test_values[name]
		var result = YAML.stringify(value)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s: %s" % [name, truncate(result.get_data())])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, value, equality_func, name)

## Test different container forms (map vs sequence)
func run_container_forms_test(value, equality_func: Callable, map_keys: Array = []) -> void:
	# Map form
	var map_style = YAML.create_style()
	map_style.set_container_form(YAMLStyle.FORM_MAP)
	var map_result = YAML.stringify(value, map_style)

	assert_stringify_success(map_result, "Map form")
	if not map_result.has_error():
		print_rich("• Map form:")
		print_rich(map_result.get_data())

		# Verify it contains map indicators
		for key in map_keys:
			assert_yaml_has_feature(map_result.get_data(), key + ":", "Contains '%s:' key" % key)
	assert_roundtrip(YAML.parse(map_result.get_data()), value, equality_func, "Map form")

	# Sequence form
	var seq_style = YAML.create_style()
	seq_style.set_container_form(YAMLStyle.FORM_SEQ)
	var seq_result = YAML.stringify(value, seq_style)

	assert_stringify_success(seq_result, "Sequence form")
	if not seq_result.has_error():
		print_rich("• Sequence form:")
		print_rich(seq_result.get_data())

		# Verify it doesn't contain map indicators
		for key in map_keys:
			assert_yaml_lacks_feature(seq_result.get_data(), key + ":", "Does not contain '%s:' key" % key)
	assert_roundtrip(YAML.parse(seq_result.get_data()), value, equality_func, "Sequence form")

## Test different flow styles (block vs flow)
func run_flow_styles_test(value, equality_func: Callable) -> void:
	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(value, flow_style)

	assert_stringify_success(flow_result, "Flow style")
	if not flow_result.has_error():
		print_rich("• Flow style:")
		print_rich(flow_result.get_data())

		# Verify it contains flow indicators (brackets)
		assert_yaml_has_feature(flow_result.get_data(), "{", "Contains opening brace")
		assert_yaml_has_feature(flow_result.get_data(), "}", "Contains closing brace")

	# Test block style (expanded)
	var block_style = YAML.create_style()
	block_style.set_flow_style(YAMLStyle.FLOW_NONE)
	var block_result = YAML.stringify(value, block_style)

	assert_stringify_success(block_result, "Block style")
	if not block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify indentation
		assert_yaml_has_feature(block_result.get_data(), "\n", "Contains proper indentation")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), value, equality_func, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), value, equality_func, "block style")

## Test string style variations
func run_string_styles_test(value, equality_func: Callable) -> void:
	# No quotes (plain string)
	var plain_style = YAML.create_style()
	plain_style.set_string_style(YAMLStyle.STRING_PLAIN)
	var plain_result = YAML.stringify(value, plain_style)

	assert_stringify_success(plain_result, "Plain string")
	if not plain_result.has_error():
		print_rich("• No quotes:")
		print_rich(plain_result.get_data())
	assert_roundtrip(YAML.parse(plain_result.get_data()), value, equality_func, "Plain string")

	# Single quotes
	var single_style = YAML.create_style()
	single_style.set_string_style(YAMLStyle.STRING_QUOTE_SINGLE)
	var single_result = YAML.stringify(value, single_style)

	assert_stringify_success(single_result, "Single quotes")
	if not single_result.has_error():
		print_rich("• Single quotes:")
		print_rich(plain_result.get_data())
		assert_yaml_has_feature(single_result.get_data(), "'", "Contains single quotes")
	assert_roundtrip(YAML.parse(single_result.get_data()), value, equality_func, "Single quotes")

	# Double quotes
	var double_style = YAML.create_style()
	double_style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)
	var double_result = YAML.stringify(value, double_style)

	assert_stringify_success(double_result, "Double quotes")
	if not double_result.has_error():
		print_rich("• Double quotes:")
		print_rich(double_result.get_data())
		assert_yaml_has_feature(double_result.get_data(), "\"", "Contains double quotes")
	assert_roundtrip(YAML.parse(double_result.get_data()), value, equality_func, "Double quotes")

	# Literal style (|)
	var literal_style = YAML.create_style()
	literal_style.set_string_style(YAMLStyle.STRING_LITERAL)
	var literal_result = YAML.stringify(value, literal_style)

	assert_stringify_success(literal_result, "Literal style")
	if not literal_result.has_error():
		print_rich("• Literal style:")
		print_rich(literal_result.get_data())
		assert_yaml_has_feature(literal_result.get_data(), "|", "Uses literal style indicator")
	assert_roundtrip(YAML.parse(literal_result.get_data()), value, equality_func, "Literal style")

	# Folded style (>)
	var folded_style = YAML.create_style()
	folded_style.set_string_style(YAMLStyle.STRING_FOLDED)
	var folded_result = YAML.stringify(value, folded_style)

	assert_stringify_success(folded_result, "Folded style")
	if not folded_result.has_error():
		print_rich("• Folded style:")
		print_rich(folded_result.get_data())
		assert_yaml_has_feature(folded_result.get_data(), ">", "Uses folded style indicator")
	assert_roundtrip(YAML.parse(folded_result.get_data()), value, equality_func, "Folded style")

## Test roundtrip with style detection
func run_style_roundtrip_test(value, equality_func: Callable, modifier_func: Callable) -> void:
	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Emit YAML with the style
	var emit_result = YAML.stringify(value, original_style)
	assert_stringify_success(emit_result, "Initial stringify")
	if emit_result.has_error():
		return

	var yaml_text = emit_result.get_data()
	print_rich("• Original YAML (with flow style):")
	print_rich(yaml_text)

	# Parse with style detection enabled
	var parse_result = YAML.parse(yaml_text, true)  # true enables style detection
	assert_parse_success(parse_result, "Parse with style detection")
	if parse_result.has_error():
		return

	# Check if style was detected
	if not parse_result.has_style():
		print_rich("[color=yellow]⚠ No style was detected[/color]")
	else:
		print_rich("[color=green]✓ Style detected successfully[/color]")

		# Get the detected style and data
		var detected_style = parse_result.get_style()
		var parsed_value = parse_result.get_data()

		# Modify the value using the provided modifier function
		var modified_value = modifier_func.call(parsed_value)

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_value, detected_style)
		assert_stringify_success(re_emit_result, "Re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved (flow style should be maintained)
		assert_yaml_has_feature(re_emit_result.get_data(), "{", "Flow style was preserved (opening brace)")
		assert_yaml_has_feature(re_emit_result.get_data(), "}", "Flow style was preserved (closing brace)")

## Deeply compare values of any type
func is_deep_equal(a: Variant, b: Variant, epsilon: float = 0.00001) -> bool:
	# Handle different types
	if typeof(a) != typeof(b) and not typeof(a) in [TYPE_INT, TYPE_FLOAT] and not typeof(b) in [TYPE_INT, TYPE_FLOAT]:
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

## Helper to truncate long text
func truncate(str: String, len := 100) -> String:
	if str.length() > len:
		return str.substr(0, len) + "..."
	return str
