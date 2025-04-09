class_name YAMLTest extends Node2D
## Base class for YAML test suites with auto-test discovery

var test_name: String = "Unnamed Test"
var passed_tests := 0
var total_tests := 0

func _ready():
	if not visible:
		return
	if get_parent() and not get_parent().visible:
		return
	print_rich("\n[b]🧪 %s[/b]" % test_name)

	# Discover and run all test methods
	var methods = get_method_list()
	var test_methods = methods.filter(func(method): return method.name.begins_with("test_"))

	# Sort methods by name for consistent execution order
	test_methods.sort_custom(func(a, b): return a.name < b.name)

	# Execute each test method
	for method in test_methods:
		print_rich("\n[b]Running: %s[/b]" % method.name.replace("test_", "").replace("_", " ").capitalize())
		call(method.name)

	print_rich("\n[b]Results: %d/%d tests passed[/b]" % [passed_tests, total_tests])

# Helper assertion methods that use Godot's assert
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

func assert_true(condition: bool, message: String) -> void:
	total_tests += 1
	if condition:
		passed_tests += 1
		print_rich("[color=green]✓ %s[/color]" % message)
	else:
		print_rich("[color=red]✗ %s[/color]" % message)
	assert(condition, message)

func assert_false(condition: bool, message: String) -> void:
	total_tests += 1
	if not condition:
		passed_tests += 1
		print_rich("[color=green]✓ %s[/color]" % message)
	else:
		print_rich("[color=red]✗ %s[/color]" % message)
	assert(not condition, message)

# Helper for custom equality checks
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

## Helper to test if a YAML string has a specific formatting feature
func assert_yaml_has_feature(yaml_string: String, feature: String, message: String) -> void:
	var has_feature = yaml_string.find(feature) != -1
	assert_true(has_feature, message)

## Helper to test if a YAML string doesn't have a specific formatting feature
func assert_yaml_lacks_feature(yaml_string: String, feature: String, message: String) -> void:
	var lacks_feature = yaml_string.find(feature) == -1
	assert_true(lacks_feature, message)

func truncate(str: String) -> String:
	if str.length() > 60:
		return str.substr(0, 60) + "..."
	return str
