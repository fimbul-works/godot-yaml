class_name TestSuite extends Node2D
## TestSuite is part of TestKit, and offers testing facilities for Godot 4.
## It uses Node2D's visibility to enable easy on-off toggling of test suites.
##
## Test cases begin with the prefix "test_", and are executed in order by
## the TestRunnerUI scene.

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

# Called when a test begins
func _start_test(method_name: String) -> void:
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
				print_rich("[color=red]✗ %s[/color]" % error_msg)

			result.passed = false
			result.errors.append(error_msg)
			return false

	return false

## Equality expectation function
func expect_equal(actual, expected, message := "") -> bool:
	if _current_method == "":
		push_error("Expect called outside of a test method")
		return false

	# Perform equality check
	var condition = actual == expected if typeof(expected) != TYPE_NIL else typeof(actual) == TYPE_NIL

	if _current_method in _test_results:
		var result = _test_results[_current_method]
		result.expectation_count += 1

		if condition:
			result.expectation_passed += 1
			return true
		else:
			var error_msg = "Values do not match"
			if message:
				error_msg += ": " + message
			# Format for consistent parsing in test_runner.gd
			error_msg += "\n  Expected: [i]%s[/i]\n  Actual: [i]%s[/i]" % [expected, actual]

			if LOG_VERBOSE:
				print_rich("[color=red]✗ %s" % error_msg)

			result.passed = false
			result.errors.append(error_msg)
			return false

	return false

## Inequality expectation function
func expect_not_equal(actual, expected, message := "") -> bool:
	if _current_method == "":
		push_error("Expect called outside of a test method")
		return false

	# Perform equality check
	var condition = actual != expected

	if _current_method in _test_results:
		var result = _test_results[_current_method]
		result.expectation_count += 1

		if condition:
			result.expectation_passed += 1
			return true
		else:
			var error_msg = "Values should not match"
			if LOG_VERBOSE:
				print_rich("[color=red]✗ %s[/color]" % error_msg)

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
