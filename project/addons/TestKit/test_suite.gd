class_name TestSuite extends Node2D

## TestSuite is part of TestKit, and offers testing facilities for Godot 4.
## It uses Node2D's visibility to enable easy on-off toggling of test suites.
##
## Test cases begin with the prefix "test_", and are executed in order by
## the TestRunnerUI scene.

## Counter that helps count the number of signal invocations.
class Counter extends RefCounted:
	## Instance counter
	var count := 0

	## Increment counter[br]
	## - [param amount]: Amount to increment by
	func increment(amount: int = 1) -> void:
		count += amount

	## Reset the counter
	## - [param value]: Value to reset the counter to
	func reset(value: int = 0) -> void:
		count = value

	## Convenience property for boolean checks
	var triggered: bool:
		get: return count > 0

	## Convenience method for lambda-friendly increment with return
	func mark() -> void:
		count += 1

	## String representation for debugging
	func _to_string() -> String:
		return "Counter(%d)" % count

## Enable verbose logging
@export var LOG_VERBOSE := false

## Limit test cases to those that contain these strings
@export var FILTER_PATTERNS: PackedStringArray = []

## Extra emoji to make scrolling logs easier
var icon := ""

# Currently executing method
var _current_method := ""
# Track test results within this class
var _test_results := {}
# Dynamic tests registered via add_test()
var _dynamic_tests := {}
# Ensure before_all only runs once
var _before_all_executed := false

## Children should override
# Run before each test
func before_each() -> void:
	# Run before each test
	pass

## Children should override
## Cleanup after each test
func after_each() -> void:
	pass

## Run once before all tests in suite
## Children should override
func before_all() -> void:
	pass

## Add a dynamic test case
## - [param test_name]: Name of the test
## - [param test_callable]: Callable to execute. Can take 0 arguments or 1 argument receiving this TestSuite instance.
func add_test(test_name: String, test_callable: Callable) -> void:
	if test_name.is_empty():
		push_error("Cannot add dynamic test with an empty name in suite '%s'" % name)
		return

	if has_method(test_name) or _dynamic_tests.has(test_name):
		push_error("Test '%s' already exists in suite '%s'" % [test_name, name])
		return

	_dynamic_tests[test_name] = test_callable

## Clear all registered dynamic tests
func clear_dynamic_tests() -> void:
	_dynamic_tests.clear()

## Get all test names (both static test_* methods and dynamic tests) matching FILTER_PATTERNS
func get_test_names() -> PackedStringArray:
	var test_names: PackedStringArray = []
	for method in get_method_list():
		if method.name.begins_with("test_"):
			if _matches_filter(method.name):
				test_names.append(method.name)
	for dyn_name in _dynamic_tests.keys():
		if _matches_filter(dyn_name):
			test_names.append(dyn_name)
	return test_names

func _matches_filter(test_name: String) -> bool:
	if FILTER_PATTERNS.is_empty():
		return true
	for pattern in FILTER_PATTERNS:
		if test_name.contains(pattern):
			return true
	return false

## Execute a test by name (static method or dynamic callable)
func run_test(test_name: String) -> void:
	_start_test(test_name)
	if _dynamic_tests.has(test_name):
		var callable: Callable = _dynamic_tests[test_name]
		await callable.call(self)
	elif has_method(test_name):
		await call(test_name)
	else:
		push_error("Test '%s' not found in suite '%s'" % [test_name, name])
	_end_test()

## Get formatted display name reflecting hierarchy if nested
func get_suite_display_name() -> String:
	var parent_node := get_parent()
	if parent_node != null and (parent_node is TestSuite or (parent_node.name != "root" and parent_node.name != "Tests" and parent_node.name != "TestRunnerUI")):
		return "%s > %s" % [parent_node.name, name]
	return name

## Check if this suite and all ancestor nodes are visible
func is_suite_visible() -> bool:
	if is_inside_tree():
		return is_visible_in_tree()
	var curr: Node = self
	while curr != null:
		if curr is TestSuite and not curr.visible:
			return false
		curr = curr.get_parent()
	return true

# Called when a test begins
func _start_test(method_name: String) -> void:
	if not _before_all_executed:
		before_all()
		_before_all_executed = true
	before_each()
	_current_method = method_name
	_test_results[method_name] = {
		"passed": true,
		"errors": [],
		"expectation_count": 0,
		"expectation_passed": 0,
		"start_time": Time.get_ticks_usec(),
		"end_time": 0
	}

# Called when a test ends
func _end_test() -> void:
	if _current_method in _test_results:
		_test_results[_current_method].end_time = Time.get_ticks_usec()
	_current_method = ""
	after_each()

## Basic expectation function
func expect(condition: bool, message := "") -> bool:
	if _current_method == "":
		push_error("Expect called outside of a test method")
		return false

	if _current_method in _test_results:
		var result = _test_results[_current_method]
		result.expectation_count += 1

		if condition:
			result.expectation_passed += 1
			return true
		else:
			var error_msg = "Expectation failed"
			if message:
				error_msg += ": " + message

			if LOG_VERBOSE:
				push_error("✗ %s" % error_msg)
			else:
				print_verbose("✗ %s" % error_msg)

			result.passed = false
			result.errors.append(error_msg)
			return false

	return false

## Equality expectation function
func expect_equal(actual: Variant, expected: Variant, message := "") -> bool:
	if _current_method == "":
		push_error("Expect called outside of a test method")
		return false

	# Get types for comparison
	var actual_type := typeof(actual)
	var expected_type := typeof(expected)

	# Handle null object reference
	if expected_type == TYPE_NIL and actual_type == TYPE_OBJECT:
		actual = null
		actual_type = TYPE_NIL

	# Perform safe equality check
	var condition := false

	# Both null
	if expected_type == TYPE_NIL and actual_type == TYPE_NIL:
		condition = true
	# One is null, the other isn't
	elif expected_type == TYPE_NIL or actual_type == TYPE_NIL:
		condition = false
	# Numeric values
	elif (expected_type == TYPE_INT and actual_type == TYPE_FLOAT) or \
		(expected_type == TYPE_FLOAT and actual_type == TYPE_INT):
		condition = actual == expected
	# String and StringName are compatible
	elif (actual_type == TYPE_STRING or actual_type == TYPE_STRING_NAME) and \
		 (expected_type == TYPE_STRING or expected_type == TYPE_STRING_NAME):
		condition = str(actual) == str(expected)
	# Type mismatch
	elif actual_type != expected_type:
		condition = false
	# Same types, safe to compare directly
	else:
		condition = actual == expected

	if _current_method in _test_results:
		var result = _test_results[_current_method]
		result.expectation_count += 1

		if condition:
			result.expectation_passed += 1
			return true
		else:
			var error_msg := "Values do not match"
			if message:
				error_msg += ": " + message

			# Safely format values for display
			var expected_str := _format_value(expected, expected_type)
			var actual_str := _format_value(actual, actual_type)

			# Format for consistent parsing in test_runner.gd
			error_msg += "\n  Expected: %s (type: %s)\n  Actual: %s (type: %s)" % [
				expected_str,
				type_string(expected_type),
				actual_str,
				type_string(actual_type)
			]

			if LOG_VERBOSE:
				print_rich("✗ %s" % error_msg)
			else:
				print_verbose("✗ %s" % error_msg)

			result.passed = false
			result.errors.append(error_msg)
			return false

	return false

## Helper function to safely format a value for display
func _format_value(value: Variant, value_type: int) -> String:
	if value_type == TYPE_NIL:
		return "null"
	elif value_type == TYPE_STRING or value_type == TYPE_STRING_NAME:
		return '"%s"' % str(value)
	else:
		return str(value)

## Inequality expectation function
func expect_not_equal(actual, expected, message := "") -> bool:
	if _current_method == "":
		push_error("Expect called outside of a test method")
		return false

	# Perform equality check
	var condition: bool = actual != expected

	if _current_method in _test_results:
		var result = _test_results[_current_method]
		result.expectation_count += 1

		if condition:
			result.expectation_passed += 1
			return true
		else:
			var error_msg := "Values should not match"
			if LOG_VERBOSE:
				print_rich("[color=red]✗ %s[/color]" % error_msg)
			else:
				print_verbose("✗ %s" % error_msg)

			result.passed = false
			result.errors.append(error_msg)
			return false

	return false

## Get all the test results for this class
func get_test_results() -> Dictionary:
	return _test_results

## Get the total number of tests that passed
func get_passed_tests() -> int:
	var passed := 0
	for method_name in _test_results:
		if _test_results[method_name].passed:
			passed += 1
	return passed

## Get the total number of tests
func get_total_tests() -> int:
	return _test_results.size()
