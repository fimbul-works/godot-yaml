class_name TestKitCLI extends SceneTree
## Command-line test runner for TestKit
## Usage: godot --headless --script res://addons/TestKit/cli_runner.gd -- --test <path>

var exit_code := 0
var verbose := false
var _instantiated_scenes: Array[Node] = []

func _init() -> void:
	call_deferred("_run")

func _run() -> void:
	var args = _parse_arguments()

	if not args.has("path"):
		printerr("Usage: godot --headless --script res://addons/TestKit/cli_runner.gd -- --path <path> [--verbose]")
		quit(1)
		return

	verbose = args.get("verbose", false)

	# Load the test scene or find test suites
	var test_path: String = args["path"]
	var test_suites: Array[TestSuite] = []

	if test_path.ends_with(".tscn"):
		# Load a specific test scene
		var scene = load(test_path)
		if not scene:
			printerr("Failed to load test scene: ", test_path)
			quit(1)
			return
		var instance = scene.instantiate()
		root.add_child(instance)
		_instantiated_scenes.append(instance)
		test_suites = _find_test_suites(instance)
	elif DirAccess.dir_exists_absolute(test_path):
		# Load all test scenes from directory
		test_suites = _load_tests_from_directory(test_path)
	else:
		printerr("Invalid test path: ", test_path)
		quit(1)
		return

	if test_suites.is_empty():
		printerr("No test suites found")
		quit(1)
		return

	var runner := TestRunner.new()

	# Connect signals for progress tracking
	runner.test_suite_started.connect(_on_suite_started)
	runner.test_method_started.connect(_on_method_started)
	runner.test_method_completed.connect(_on_method_completed)
	runner.test_suite_completed.connect(_on_suite_completed)
	runner.all_tests_completed.connect(_on_all_completed)

	# Run the tests
	await runner.run_tests(test_suites)

	# Clean up instantiated scenes to prevent leaks at exit
	for instance in _instantiated_scenes:
		if is_instance_valid(instance):
			root.remove_child(instance)
			instance.free()
	_instantiated_scenes.clear()

	# Exit with appropriate code
	quit(exit_code)

func _parse_arguments() -> Dictionary:
	var result := {}
	var cmd_args := OS.get_cmdline_args() + OS.get_cmdline_user_args()

	# Find the -- separator
	var start_idx := 0
	while !cmd_args[start_idx].ends_with("cli_runner.gd"):
		start_idx += 1
		if start_idx == cmd_args.size() - 1:
			return result

	# Parse arguments after --
	var i := start_idx + 1
	while i < cmd_args.size():
		var arg: String = cmd_args[i]

		if arg.begins_with("--"):
			var key := arg.substr(2)

			# Check if next arg is a value or another flag
			if i + 1 < cmd_args.size() and not cmd_args[i + 1].begins_with("--"):
				result[key] = cmd_args[i + 1]
				i += 2
			else:
				result[key] = true
				i += 1
		else:
			i += 1

	return result

func _find_test_suites(node: Node) -> Array[TestSuite]:
	var suites: Array[TestSuite] = []

	if node is TestSuite and node.visible:
		suites.append(node)

	for child in node.get_children():
		suites.append_array(_find_test_suites(child))

	return suites

func _load_tests_from_directory(dir_path: String) -> Array[TestSuite]:
	var suites: Array[TestSuite] = []
	var dir := DirAccess.open(dir_path)

	if not dir:
		printerr("Failed to open directory: ", dir_path)
		return suites

	dir.list_dir_begin()
	var file_name := dir.get_next()

	while file_name != "":
		if not dir.current_is_dir() and file_name.ends_with(".tscn"):
			var full_path := dir_path.path_join(file_name)
			var scene = load(full_path)
			if scene:
				var instance = scene.instantiate()
				root.add_child(instance)
				_instantiated_scenes.append(instance)
				suites.append_array(_find_test_suites(instance))

		file_name = dir.get_next()

	dir.list_dir_end()
	return suites

func _on_suite_started(test_class: TestSuite) -> void:
	print("\n%s %s" % [test_class.icon if test_class.icon else "📋", test_class.name])

func _on_method_started(test_class: TestSuite, method_name: String) -> void:
	if verbose:
		print("  Running %s..." % method_name)

func _on_method_completed(test_class: TestSuite, method_name: String, result: Dictionary) -> void:
	if result.passed:
		if verbose:
			print("  ✅ %s" % method_name)
	else:
		print("  ❌ %s" % method_name)
		for error in result.errors:
			print("     %s" % error)
		exit_code = 1

func _on_suite_completed(test_class: TestSuite, results: Dictionary) -> void:
	var status := "✅" if results.all_passed else "❌"
	print("  %s %d/%d tests passed (%d/%d expectations)" % [
		status,
		results.passed,
		results.total,
		results.passed_expectations,
		results.total_expectations
	])

func _on_all_completed(final_results: Array) -> void:
	var total_passed := 0
	var total_tests := 0
	var total_passed_expectations := 0
	var total_expectations := 0

	for result in final_results:
		total_passed += result.passed
		total_tests += result.total
		total_passed_expectations += result.passed_expectations
		total_expectations += result.total_expectations

	print("\n" + "=".repeat(50))

	if total_passed == total_tests:
		print("✅ All tests passed!")
	else:
		var failed := total_tests - total_passed
		print("❌ %d test(s) failed" % failed)

	print("%d/%d tests passed (%d/%d expectations)" % [
		total_passed,
		total_tests,
		total_passed_expectations,
		total_expectations
	])
	print("=".repeat(50))
