class_name TestRunnerUI extends Control

@onready var test_suites_list: VBoxContainer = %TestSuitesList
@onready var test_details_scroll: ScrollContainer = %TestDetailsScroll
@onready var test_details_content: VBoxContainer = %TestDetailsContent
@onready var summary_label: RichTextLabel = %SummaryLabel
@onready var progress_bar: ProgressBar = %ProgressBar
@onready var run_button: Button = %RunButton

@export var run_on_launch: bool = true

var test_suites := {}
var suite_results := []
var current_suite_button: Button = null

# Colors for different states
const COLOR_PASS = Color.GREEN
const COLOR_FAIL = Color.RED
const COLOR_PENDING = Color.GRAY
const COLOR_RUNNING = Color.YELLOW

func _ready() -> void:
	run_button.pressed.connect(_on_run_tests_pressed)
	_setup_ui()
	if run_on_launch:
		_on_run_tests_pressed()

func _setup_ui() -> void:
	# Find and prepare test classes
	var test_classes = _find_test_classes()
	_populate_test_suites(test_classes)
	_update_summary()

func _populate_test_suites(test_classes: Array[Node]) -> void:
	# Clear existing items
	for child in test_suites_list.get_children():
		child.queue_free()

	test_suites.clear()

	for test_class in test_classes:
		if !test_class.visible or (test_class.owner and !test_class.owner.visible):
			continue

		var test_methods = _find_test_methods(test_class)
		if test_methods.is_empty():
			continue

		# Create suite button
		var suite_button = _create_suite_button(test_class, test_methods)
		test_suites_list.add_child(suite_button)

		# Store suite data
		test_suites[test_class.name] = {
			"test_class": test_class,
			"test_methods": test_methods,
			"button": suite_button,
			"status": "pending"
		}

func _create_suite_button(test_class: BaseTest, test_methods: Array) -> Button:
	var button = Button.new()
	button.alignment = HORIZONTAL_ALIGNMENT_LEFT
	button.custom_minimum_size.y = 40

	var icon_text = test_class.icon if test_class.icon.length() > 0 else "📁"
	var method_count = test_methods.size()

	button.text = "%s %s (%d tests)" % [icon_text, test_class.name, method_count]
	button.add_theme_color_override("font_color", COLOR_PENDING)

	# Connect button press
	button.pressed.connect(_on_suite_selected.bind(test_class.name))

	return button

func _on_suite_selected(suite_name: String) -> void:
	if suite_name not in test_suites:
		return

	# Update button selection visual
	if current_suite_button:
		current_suite_button.add_theme_color_override("font_pressed_color", Color.WHITE)

	var suite_data = test_suites[suite_name]
	current_suite_button = suite_data.button
	current_suite_button.add_theme_color_override("font_pressed_color", Color.CYAN)

	# Display suite details
	_display_suite_details(suite_data)

func _display_suite_details(suite_data: Dictionary) -> void:
	# Clear existing content
	for child in test_details_content.get_children():
		child.queue_free()

	var test_class: BaseTest = suite_data.test_class
	var test_methods: Array = suite_data.test_methods

	# Suite header
	var header = RichTextLabel.new()
	header.custom_minimum_size.y = 60
	header.fit_content = true
	header.bbcode_enabled = true

	var icon_text = test_class.icon if test_class.icon.length() > 0 else "📁"
	header.text = "[font_size=20][b]%s %s[/b][/font_size]\n[i]%d test methods[/i]" % [
		icon_text, test_class.name, test_methods.size()
	]

	test_details_content.add_child(header)

	# Separator
	var separator1 = HSeparator.new()
	test_details_content.add_child(separator1)

	# Test methods list
	for method_name in test_methods:
		var method_container = _create_test_method_item(test_class, method_name)
		test_details_content.add_child(method_container)

func _create_test_method_item(test_class: BaseTest, method_name: String) -> Control:
	var container = VBoxContainer.new()
	container.custom_minimum_size.y = 30

	# Method header
	var method_header = HBoxContainer.new()

	# Status icon
	var status_icon = Label.new()
	status_icon.text = "⏳"  # Default pending
	status_icon.custom_minimum_size.x = 30
	method_header.add_child(status_icon)

	# Method name
	var method_label = Label.new()
	method_label.text = method_name + "()"
	method_label.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	method_header.add_child(method_label)

	container.add_child(method_header)

	# Error container (initially hidden)
	var error_container = VBoxContainer.new()
	error_container.visible = false
	error_container.add_theme_color_override("font_color", COLOR_FAIL)
	container.add_child(error_container)

	# Store references for later updates
	container.set_meta("status_icon", status_icon)
	container.set_meta("method_label", method_label)
	container.set_meta("error_container", error_container)
	container.set_meta("test_class", test_class)
	container.set_meta("method_name", method_name)

	# Check if this test has already been run and update accordingly
	_update_method_display_from_results(container, test_class, method_name)

	return container

func _on_run_tests_pressed() -> void:
	run_button.disabled = true
	run_button.text = "Running Tests..."

	suite_results.clear()

	# Reset all UI elements
	_reset_ui_state()

	# Run tests
	var test_classes = []
	for suite_name in test_suites:
		test_classes.append(test_suites[suite_name].test_class)

	# Run tests with UI updates
	await _run_tests_with_ui(test_classes)

	# Update final summary
	_update_final_summary()

	run_button.disabled = false
	run_button.text = "Run Tests"

func _reset_ui_state() -> void:
	progress_bar.value = 0

	for suite_name in test_suites:
		var suite_data = test_suites[suite_name]
		suite_data.button.add_theme_color_override("font_color", COLOR_PENDING)
		suite_data.status = "pending"

func _run_tests_with_ui(test_classes: Array) -> void:
	var total_methods = 0
	var completed_methods = 0

	# Count total test methods for progress
	for test_class in test_classes:
		var methods = _find_test_methods(test_class)
		total_methods += methods.size()

	progress_bar.max_value = total_methods

	for test_class in test_classes:
		var test_methods := _find_test_methods(test_class)
		if test_methods.is_empty():
			continue

		# Update suite status to running
		_update_suite_status(test_class.name, "running")

		# Run each test method
		for method_name in test_methods:
			print_rich("[b]Running %s - %s[/b]" % [test_class.name, method_name.substr(5).replace("_", " ")])
			# Update method status to running
			_update_method_status(test_class, method_name, "running")

			# Run the test
			test_class._start_test(method_name)
			test_class.call(method_name)
			test_class._end_test()

			var result = test_class._test_results[method_name]

			# Update method status with result
			if result.passed:
				_update_method_status(test_class, method_name, "passed")
			else:
				print_rich("[color=red]Failed %s - %s: %s[/color]" % [test_class.name, method_name.substr(5).replace("_", " "), ", ".join(result.errors)])
				_update_method_status(test_class, method_name, "failed", result.errors)

			completed_methods += 1
			progress_bar.value = completed_methods

			# Allow UI to update
			await get_tree().process_frame

		# Update suite final status
		var passed_tests = test_class.get_passed_tests()
		var total_tests = test_class.get_total_tests()

		if passed_tests == total_tests:
			_update_suite_status(test_class.name, "passed")
		else:
			_update_suite_status(test_class.name, "failed")

		# Store results for final report
		_store_suite_results(test_class)

func _update_suite_status(suite_name: String, status: String) -> void:
	if suite_name not in test_suites:
		return

	var suite_data = test_suites[suite_name]
	suite_data.status = status

	var button = suite_data.button
	var color = COLOR_PENDING
	var icon = "📁"

	match status:
		"running":
			color = COLOR_RUNNING
			icon = "⏳"
		"passed":
			color = COLOR_PASS
			icon = "✅"
		"failed":
			color = COLOR_FAIL
			icon = "❌"

	# Update button text and color
	var test_methods = suite_data.test_methods
	var method_count = test_methods.size()
	button.text = "%s %s (%d tests)" % [icon, suite_name, method_count]
	button.add_theme_color_override("font_color", color)

func _update_method_display_from_results(container: Control, test_class: BaseTest, method_name: String) -> void:
	# Check if this test has already been run
	var test_results = test_class.get_test_results()
	if method_name in test_results:
		var result = test_results[method_name]
		var status_icon = container.get_meta("status_icon")
		var method_label = container.get_meta("method_label")
		var error_container = container.get_meta("error_container")

		if result.passed:
			status_icon.text = "✅"
			method_label.add_theme_color_override("font_color", COLOR_PASS)
			error_container.visible = false
		else:
			status_icon.text = "❌"
			method_label.add_theme_color_override("font_color", COLOR_FAIL)
			_display_method_errors(error_container, result.errors)

func _update_method_status(test_class: BaseTest, method_name: String, status: String, errors: Array = []) -> void:
	# Find the method item in the current display (if it's currently shown)
	for child in test_details_content.get_children():
		if child.has_meta("method_name") and child.has_meta("test_class"):
			if child.get_meta("method_name") == method_name and child.get_meta("test_class") == test_class:
				var status_icon = child.get_meta("status_icon")
				var method_label = child.get_meta("method_label")
				var error_container = child.get_meta("error_container")

				match status:
					"running":
						status_icon.text = "⏳"
						method_label.add_theme_color_override("font_color", COLOR_RUNNING)
						error_container.visible = false
					"passed":
						status_icon.text = "✅"
						method_label.add_theme_color_override("font_color", COLOR_PASS)
						error_container.visible = false
					"failed":
						status_icon.text = "❌"
						method_label.add_theme_color_override("font_color", COLOR_FAIL)
						_display_method_errors(error_container, errors)
				break

	# Also update the suite button status if this is the last test or if it failed
	_update_suite_button_from_results(test_class)

func _update_suite_button_from_results(test_class: BaseTest) -> void:
	var suite_name = test_class.name
	if suite_name not in test_suites:
		return

	var test_results = test_class.get_test_results()
	if test_results.is_empty():
		return

	var suite_data = test_suites[suite_name]
	var button = suite_data.button
	var test_methods = suite_data.test_methods

	# Check if all tests are complete and determine status
	var total_methods = test_methods.size()
	var completed_methods = 0
	var passed_methods = 0
	var failed_methods = 0

	for method_name in test_methods:
		if method_name in test_results:
			completed_methods += 1
			if test_results[method_name].passed:
				passed_methods += 1
			else:
				failed_methods += 1

	var icon = "📁"
	var color = COLOR_PENDING
	var status = "pending"

	if completed_methods == total_methods:
		# All tests completed
		if failed_methods == 0:
			icon = "✅"
			color = COLOR_PASS
			status = "passed"
		else:
			icon = "❌"
			color = COLOR_FAIL
			status = "failed"
	elif completed_methods > 0:
		# Some tests completed
		icon = "⏳"
		color = COLOR_RUNNING
		status = "running"

	# Update button
	button.text = "%s %s (%d tests)" % [icon, suite_name, total_methods]
	button.add_theme_color_override("font_color", color)
	suite_data.status = status

func _display_method_errors(error_container: VBoxContainer, errors: Array) -> void:
	# Clear existing errors
	for child in error_container.get_children():
		child.queue_free()

	if errors.is_empty():
		error_container.visible = false
		return

	for error in errors:
		var error_label = RichTextLabel.new()
		error_label.bbcode_enabled = true
		error_label.fit_content = true
		error_label.custom_minimum_size.y = 20

		# Format error message
		if error.contains("Expected:") and error.contains("Actual:"):
			# Parse expected/actual format
			var lines = error.split("\n")
			var formatted_error = "[color=red]" + lines[0] + "[/color]\n"

			for i in range(1, lines.size()):
				var line = lines[i]
				if line.strip_edges().begins_with("Expected:"):
					formatted_error += "  [i]Expected:[/i] " + line.trim_prefix("  Expected: ") + "\n"
				elif line.strip_edges().begins_with("Actual:"):
					formatted_error += "  [i]Received:[/i] " + line.trim_prefix("  Actual: ") + "\n"
				else:
					formatted_error += line + "\n"

			error_label.text = formatted_error.strip_edges()
		else:
			error_label.text = "[color=red]" + str(error) + "[/color]"

		error_container.add_child(error_label)

	error_container.visible = true

func _store_suite_results(test_class: BaseTest) -> void:
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

	suite_results.append({
		"suite_name": test_class.name,
		"passed": passed_tests,
		"total": total_tests,
		"passed_expectations": passed_expectations,
		"total_expectations": total_expectations
	})

func _update_summary() -> void:
	summary_label.text = "[b]Ready to run tests[/b]"

func _update_final_summary() -> void:
	var total_passed := 0
	var total_tests := 0
	var total_passed_expectations := 0
	var total_expectations := 0

	for result in suite_results:
		total_passed += result.passed
		total_tests += result.total
		total_passed_expectations += result.passed_expectations
		total_expectations += result.total_expectations

	var status_text = ""
	var color = "green"

	if total_passed == total_tests:
		status_text = "[color=green][b]All tests passed![/b][/color]"
	else:
		var failed = total_tests - total_passed
		status_text = "[color=red][b]%d test(s) failed[/b][/color]" % failed
		color = "red"

	summary_label.text = "%s\n[color=%s]%d/%d tests passed (%d/%d expectations)[/color]" % [
		status_text, color, total_passed, total_tests, total_passed_expectations, total_expectations
	]

func _find_test_classes(parent_node: Node = get_parent()) -> Array[Node]:
	var classes: Array[Node] = []
	for child in parent_node.get_children():
		if child is BaseTest and child != self:
			classes.append(child)
		if child.get_child_count() > 0:
			classes.append_array(_find_test_classes(child))
	return classes

func _find_test_methods(test_class: BaseTest) -> PackedStringArray:
	var test_methods: PackedStringArray = []
	for method in test_class.get_method_list():
		if method.name.begins_with("test_"):
			# Check for "run only these" patterns
			if test_class.ONLY_PATTERNS.size():
				var found := false
				for pattern in test_class.ONLY_PATTERNS:
					if method.name.contains(pattern):
						found = true
						break
				# Skip to the next
				if !found:
					continue
			test_methods.append(method.name)
	return test_methods
