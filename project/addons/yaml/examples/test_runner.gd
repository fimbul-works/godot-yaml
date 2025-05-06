class_name TestRunner extends Node2D

var test_suites := {}
var suite_results := []

func _ready() -> void:
	if !visible:
		return

	var test_classes = _find_test_classes(self)
	run_tests(test_classes)
	print_final_report()

func run_tests(test_classes: Array[Node]) -> void:
	for test_class in test_classes:
		if !test_class.visible or (test_class.owner and !test_class.owner.visible):
			continue

		var test_methods = _find_test_methods(test_class)

		if test_methods.is_empty():
			continue

		print_rich("\n[b][font_size=16]%s%s[/font_size][/b]" % ["%s " % test_class.icon if test_class.icon.length() > 0 else "", test_class.name])

		# Run all test methods for this class
		for method_name in test_methods:
			test_class._start_test(method_name)
			test_class.call(method_name)
			test_class._end_test()

			var result = test_class._test_results[method_name]

			# Print test result with checkmark or cross
			if result.passed:
				print_rich("[color=green]✓[/color] [b]%s()[/b]" % method_name)
			else:
				print_rich("[color=red]✗[/color] [b]%s()[/b]" % method_name)

				# Print error details if test failed
				for error in result.errors:
					# Check if error contains expected/actual values
					if error.contains("Expected:") and error.contains("Actual:"):
						# Extract and format the expected/actual values nicely
						var lines = error.split("\n")
						for line in lines:
							if line.begins_with("  Expected:"):
								print_rich("  [i]expected:[/i] %s" % line.trim_prefix("  Expected: "))
							elif line.begins_with("  Actual:"):
								print_rich("  [i]received:[/i] %s" % line.trim_prefix("  Actual: "))
							else:
								print(line)
					else:
						# Just print the error message
						print_rich("  %s" % error)

		# Report results for this test class
		_report_test_class(test_class)

func _report_test_class(test_class: BaseTest) -> void:
	var passed_tests = test_class.get_passed_tests()
	var total_tests = test_class.get_total_tests()
	var test_results = test_class.get_test_results()

	# Calculate total expectations
	var total_expectations := 0
	var passed_expectations := 0
	for method_name in test_results:
		var result = test_results[method_name]
		total_expectations += result.expectation_count
		passed_expectations += result.expectation_passed

	# Calculate total time
	var total_time := 0
	for method_name in test_results:
		var result = test_results[method_name]
		total_time += result.end_time - result.start_time

	var t: float = total_time
	var tl = "µsec"
	if t > 1000:
		t *= 0.001
		tl = "ms"

	# Store results for final report
	suite_results.append({
		"suite_name": test_class.name,
		"passed": passed_tests,
		"total": total_tests,
		"passed_expectations": passed_expectations,
		"total_expectations": total_expectations
	})

	if passed_tests == total_tests:
		print_rich("[color=green]%d/%d tests passed ([i]%d/%d expectations[/i]) in %d %s[/color]" %
				  [passed_tests, total_tests, passed_expectations, total_expectations, t, tl])
	else:
		print_rich("[color=red]%d/%d tests failed ([i]%d/%d expectations failed[/i]) in %d %s[/color]" %
				  [total_tests - passed_tests, total_tests,
				  total_expectations - passed_expectations, total_expectations, t, tl])

func print_final_report() -> void:
	print_rich("\n[b][font_size=18]Test Summary Report[/font_size][/b]")

	var total_passed := 0
	var total_tests := 0
	var total_passed_expectations := 0
	var total_expectations := 0

	for result in suite_results:
		var status = "succeeded" if result.passed == result.total else "failed"
		var color = "green" if result.passed == result.total else "red"
		var warn = "yellow" if result.passed < result.total && result.passed > 0 else "green"
		print_rich("[b]%s[/b] [color=%s](%d/%d %s,[/color] [color=%s][i]%d/%d expectations[/i])" %
			[result.suite_name, color, result.passed, result.total, status,
			warn, result.passed_expectations, result.total_expectations])

		total_passed += result.passed
		total_tests += result.total
		total_passed_expectations += result.passed_expectations
		total_expectations += result.total_expectations

	var total_status = "succeeded" if total_passed == total_tests else "failed"
	var total_color = "green" if total_passed == total_tests else "red"
	var warn_color = "yellow" if total_passed < total_tests && total_tests > 0 else "green"
	print_rich("\n[b]Total:[/b] [color=%s]%d/%d tests %s[/color] [color=%s]([i]%d/%d expectations[/i])[/color]" %
		[total_color, total_passed, total_tests, total_status,
		warn_color, total_passed_expectations, total_expectations])

func _find_test_classes(parent_node: Node = self) -> Array[Node]:
	var classes: Array[Node] = []
	for child in parent_node.get_children():
		if child is BaseTest and child != self:
			classes.append(child)
		if child.get_child_count() > 0:
			classes.append_array(_find_test_classes(child))
	return classes

func _find_test_methods(test_class: Node) -> Array:
	var test_methods = []

	# Get all methods that start with "test_"
	for method in test_class.get_method_list():
		if method.name.begins_with("test_"):
			test_methods.append(method.name)

	return test_methods
