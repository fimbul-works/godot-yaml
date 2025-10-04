class_name TestRunnerUI extends Control

@onready var test_suites_list: VBoxContainer = %TestSuitesList
@onready var test_details_scroll: ScrollContainer = %TestDetailsScroll
@onready var test_details_content: VBoxContainer = %TestDetailsContent
@onready var summary_label: RichTextLabel = %SummaryLabel
@onready var progress_bar: ProgressBar = %ProgressBar
@onready var run_button: Button = %RunButton

@export var run_on_launch: bool = true

var test_runner: TestRunner
var test_suites := {}
var current_suite_button: Button = null

# Colors for different states
const COLOR_PASS = Color.GREEN
const COLOR_FAIL = Color.RED
const COLOR_PENDING = Color.GRAY
const COLOR_RUNNING = Color.YELLOW

func _ready() -> void:
	# Initialize test runner
	test_runner = TestRunner.new()
	_connect_test_runner_signals()

	# Setup UI
	run_button.pressed.connect(_on_run_tests_pressed)
	_setup_ui()

	if run_on_launch:
		_on_run_tests_pressed()

func _connect_test_runner_signals() -> void:
	test_runner.tests_started.connect(_on_tests_started)
	test_runner.test_method_started.connect(_on_test_method_started)
	test_runner.test_method_completed.connect(_on_test_method_completed)
	test_runner.test_suite_started.connect(_on_test_suite_started)
	test_runner.test_suite_completed.connect(_on_test_suite_completed)
	test_runner.all_tests_completed.connect(_on_all_tests_completed)
	test_runner.progress_updated.connect(_on_progress_updated)

func _setup_ui() -> void:
	# Find and prepare test classes
	var test_classes = test_runner.find_test_classes(get_parent())
	_populate_test_suites(test_classes)
	_update_summary()

func _populate_test_suites(test_classes: Array[TestSuite]) -> void:
	# Clear existing items
	for child in test_suites_list.get_children():
		child.queue_free()

	test_suites.clear()

	for test_class in test_classes:
		if !test_class.visible or (test_class.owner and !test_class.owner.visible):
			continue

		var test_methods = test_runner.find_test_methods(test_class)
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

func _create_suite_button(test_class: TestSuite, test_methods: Array) -> Button:
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

	var test_class: TestSuite = suite_data.test_class
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

func _create_test_method_item(test_class: TestSuite, method_name: String) -> Control:
	var container := VBoxContainer.new()
	container.custom_minimum_size.y = 30

	# Method header
	var method_header := HBoxContainer.new()

	# Method name
	var method_label := Label.new()
	method_label.text = "⏳ %s()" % method_name
	method_label.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	method_header.add_child(method_label)

	container.add_child(method_header)

	# Error container (initially hidden)
	var error_container := VBoxContainer.new()
	error_container.visible = false
	error_container.add_theme_color_override("font_color", COLOR_FAIL)
	container.add_child(error_container)

	# Store references for later updates
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

	# Reset all UI elements
	_reset_ui_state()

	# Get test classes and run tests
	var test_classes: Array[TestSuite] = []
	for suite_name in test_suites:
		test_classes.append(test_suites[suite_name].test_class)

	if test_classes.size() == 0:
		# Mark the button with error
		run_button.disabled = true
		run_button.text = "Error"

		# Remove the "select test suite"
		for child in test_details_content.get_children():
			child.queue_free()

		summary_label.text = "[color=red][b]No test suites found.[/b][/color]"
		return

	# Start test execution
	await test_runner.run_tests(test_classes)

func _reset_ui_state() -> void:
	progress_bar.value = 0

	for suite_name in test_suites:
		var suite_data = test_suites[suite_name]
		suite_data.button.add_theme_color_override("font_color", COLOR_PENDING)
		suite_data.status = "pending"

func _update_method_display_from_results(container: Control, test_class: TestSuite, method_name: String) -> void:
	# Check if this test has already been run
	var test_results = test_class.get_test_results()
	if method_name in test_results:
		var result = test_results[method_name]
		var method_label = container.get_meta("method_label")
		var error_container = container.get_meta("error_container")

		if result.passed:
			method_label.text = "✅ %s()" % method_name
			method_label.add_theme_color_override("font_color", COLOR_PASS)
			error_container.visible = false
		else:
			method_label.text = "❌ %s()" % method_name
			method_label.add_theme_color_override("font_color", COLOR_FAIL)
			_display_method_errors(error_container, result.errors)

func _update_method_status(test_class: TestSuite, method_name: String, status: String, errors: Array = []) -> void:
	# Find the method item in the current display (if it's currently shown)
	for child in test_details_content.get_children():
		if child.has_meta("method_name") and child.has_meta("test_class"):
			if child.get_meta("method_name") == method_name and child.get_meta("test_class") == test_class:
				var method_label = child.get_meta("method_label")
				var error_container = child.get_meta("error_container")

				match status:
					"running":
						method_label.text = "⏳ %s()" % method_name
						method_label.add_theme_color_override("font_color", COLOR_RUNNING)
						error_container.visible = false
					"passed":
						method_label.text = "✅ %s()" % method_name
						method_label.add_theme_color_override("font_color", COLOR_PASS)
						error_container.visible = false
					"failed":
						method_label.text = "❌ %s()" % method_name
						method_label.add_theme_color_override("font_color", COLOR_FAIL)
						_display_method_errors(error_container, errors)
				break

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

func _update_summary() -> void:
	summary_label.text = "[b]Ready to run tests[/b]"

func _update_final_summary(final_results: Array) -> void:
	var total_passed := 0
	var total_tests := 0
	var total_passed_expectations := 0
	var total_expectations := 0

	for result in final_results:
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

# Signal handlers for TestRunner events
func _on_tests_started(total_tests: int) -> void:
	progress_bar.max_value = total_tests
	progress_bar.value = 0

func _on_test_method_started(test_class: TestSuite, method_name: String) -> void:
	_update_method_status(test_class, method_name, "running")

func _on_test_method_completed(test_class: TestSuite, method_name: String, result: Dictionary) -> void:
	if result.passed:
		_update_method_status(test_class, method_name, "passed")
	else:
		_update_method_status(test_class, method_name, "failed", result.errors)

func _on_test_suite_started(test_class: TestSuite) -> void:
	_update_suite_status(test_class.name, "running")

func _on_test_suite_completed(test_class: TestSuite, results: Dictionary) -> void:
	var status = "passed" if results.all_passed else "failed"
	_update_suite_status(test_class.name, status)

func _on_all_tests_completed(final_results: Array) -> void:
	_update_final_summary(final_results)
	run_button.disabled = false
	run_button.text = "Run Tests ▶"

func _on_progress_updated(completed: int, total: int) -> void:
	progress_bar.value = completed
