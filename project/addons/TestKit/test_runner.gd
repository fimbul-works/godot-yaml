class_name TestRunner extends RefCounted

# Signals for communicating with UI
signal tests_started(total_tests: int)
signal test_method_started(test_class: TestSuite, method_name: String)
signal test_method_completed(test_class: TestSuite, method_name: String, result: Dictionary)
signal test_suite_started(test_class: TestSuite)
signal test_suite_completed(test_class: TestSuite, results: Dictionary)
signal all_tests_completed(final_results: Array)
signal progress_updated(completed: int, total: int)

var test_suites := {}
var suite_results := []
var _total_test_methods := 0
var _completed_test_methods := 0

func find_test_classes(parent_node: Node) -> Array[TestSuite]:
	var classes: Array[TestSuite] = []
	for child in parent_node.get_children():
		if child is TestSuite and not child.visible:
			continue
		if child is TestSuite:
			classes.append(child)
		if child.get_child_count() > 0:
			classes.append_array(find_test_classes(child))
	return classes

func get_suite_key(test_class: TestSuite) -> String:
	var key: String = test_class.get_suite_display_name() if test_class.has_method("get_suite_display_name") else test_class.name
	return key

func find_test_methods(test_class: TestSuite) -> PackedStringArray:
	if test_class.has_method("get_test_names"):
		return test_class.get_test_names()

	var test_methods: PackedStringArray = []
	for method in test_class.get_method_list():
		if method.name.begins_with("test_"):
			# Check for "run only these" patterns
			if test_class.FILTER_PATTERNS.size():
				var found := false
				for pattern in test_class.FILTER_PATTERNS:
					if method.name.contains(pattern):
						found = true
						break
				# Skip to the next
				if !found:
					continue
			test_methods.append(method.name)
	return test_methods

func prepare_test_suites(test_classes: Array[TestSuite]) -> Dictionary:
	test_suites.clear()
	_total_test_methods = 0

	for test_class in test_classes:
		if test_class.has_method("is_suite_visible"):
			if not test_class.is_suite_visible():
				continue
		elif test_class is TestSuite and not test_class.is_visible_in_tree():
			continue
		elif not test_class.visible or (test_class.owner and !test_class.owner.visible):
			continue

		var test_methods := find_test_methods(test_class)
		if test_methods.is_empty():
			continue

		var suite_key := get_suite_key(test_class)
		if test_suites.has(suite_key):
			suite_key = "%s (%d)" % [suite_key, test_class.get_instance_id()]
		test_class.set_meta("suite_key", suite_key)

		test_suites[suite_key] = {
			"test_class": test_class,
			"test_methods": test_methods,
			"status": "pending"
		}

		_total_test_methods += test_methods.size()

	return test_suites

func run_tests(test_classes: Array[TestSuite]) -> void:
	suite_results.clear()
	_completed_test_methods = 0

	# Prepare test suites
	prepare_test_suites(test_classes)

	# Emit start signal
	tests_started.emit(_total_test_methods)

	# Run tests for each class
	for test_class in test_classes:
		var suite_key: String = test_class.get_meta("suite_key", get_suite_key(test_class))
		if suite_key not in test_suites:
			continue

		await _run_suite_tests(test_class)

	# Emit completion signal
	all_tests_completed.emit(suite_results)

func _run_suite_tests(test_class: TestSuite) -> void:
	var suite_key: String = test_class.get_meta("suite_key", get_suite_key(test_class))
	var suite_data = test_suites[suite_key]
	var test_methods: Array = suite_data.test_methods

	if test_methods.is_empty():
		return

	# Emit suite started
	test_suite_started.emit(test_class)

	# Run each test method
	for method_name in test_methods:
		var display_method: String = method_name.substr(5).replace("_", " ") if method_name.begins_with("test_") else method_name
		if test_class.LOG_VERBOSE:
			print_rich("[b]Running %s - %s[/b]" % [suite_key, display_method])
		else:
			print_verbose("Running %s - %s" % [suite_key, display_method])

		# Emit method started
		test_method_started.emit(test_class, method_name)

		# Run the test
		if test_class.has_method("run_test"):
			await test_class.run_test(method_name)
		else:
			test_class._start_test(method_name)
			await test_class.call(method_name)
			test_class._end_test()

		var result = test_class._test_results[method_name]

		# Print failure message if needed
		if not result.passed:
			if test_class.LOG_VERBOSE:
				print_rich("[color=red]Failed %s - %s: %s[/color]:\n  " % [suite_key, display_method, "\n  ".join(result.errors)])
			else:
				print_verbose("Failed %s - %s: %s:\n  " % [suite_key, display_method, "\n  ".join(result.errors)])

		# Emit method completed
		test_method_completed.emit(test_class, method_name, result)

		# Update progress
		_completed_test_methods += 1
		progress_updated.emit(_completed_test_methods, _total_test_methods)

		# Allow processing
		var main_loop := Engine.get_main_loop()
		if main_loop:
			await main_loop.process_frame

	# Calculate suite results
	var passed_tests := test_class.get_passed_tests()
	var total_tests := test_class.get_total_tests()
	var test_results := test_class.get_test_results()

	# Calculate total expectations
	var total_expectations := 0
	var passed_expectations := 0
	for method_name in test_results:
		var result = test_results[method_name]
		total_expectations += result.expectation_count
		passed_expectations += result.expectation_passed

	var suite_result = {
		"suite_name": suite_key,
		"passed": passed_tests,
		"total": total_tests,
		"passed_expectations": passed_expectations,
		"total_expectations": total_expectations,
		"all_passed": passed_tests == total_tests
	}

	# Store results
	suite_results.append(suite_result)

	# Emit suite completed
	test_suite_completed.emit(test_class, suite_result)

func get_test_suites() -> Dictionary:
	return test_suites

func get_suite_results() -> Array:
	return suite_results

func get_total_test_methods() -> int:
	return _total_test_methods

func get_completed_test_methods() -> int:
	return _completed_test_methods
