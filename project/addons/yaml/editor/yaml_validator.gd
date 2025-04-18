@tool
class_name YAMLValidator
extends Node

signal validation_completed(result)

var _thread: Thread
var _mutex: Mutex
var _is_validating: bool = false
var _pending_validation: bool = false
var _current_text: String = ""

var code_editor: YAMLCodeEditor
var status_label: Label
var validation_timer: Timer

func _ready() -> void:
	_mutex = Mutex.new()

	# Create validation timer
	validation_timer = Timer.new()
	validation_timer.one_shot = true
	validation_timer.wait_time = 0.5  # 500ms delay
	validation_timer.timeout.connect(_on_validation_timer_timeout)
	add_child(validation_timer)

func setup(p_code_editor: YAMLCodeEditor, p_status_label: Label) -> void:
	code_editor = p_code_editor
	status_label = p_status_label

	# Connect to code editor changes
	code_editor.validation_requested.connect(_on_validation_requested)

func _on_validation_requested() -> void:
	# Reset and start the validation timer
	validation_timer.stop()
	validation_timer.start()

func _on_validation_timer_timeout() -> void:
	validate_async(code_editor.text)

func validate_async(yaml_text: String) -> void:
	_mutex.lock()
	_current_text = yaml_text

	if _is_validating:
		_pending_validation = true
		_mutex.unlock()
		return

	_is_validating = true
	_mutex.unlock()

	if _thread and _thread.is_started():
		_thread.wait_to_finish()

	_thread = Thread.new()
	_thread.start(_validation_thread_function)

func _validation_thread_function() -> void:
	var result = YAML.validate(_current_text)

	_mutex.lock()
	var is_validating = _is_validating
	_is_validating = false

	var should_continue = _pending_validation
	_pending_validation = false
	_mutex.unlock()

	# Emit signal on main thread
	call_deferred("_emit_validation_completed", result)

	if should_continue:
		call_deferred("validate_async", _current_text)

func _emit_validation_completed(result) -> void:
	validation_completed.emit(result)
	_process_validation_result(result)

func _process_validation_result(result) -> void:
	# Clear previous error indicators
	code_editor.clear_error_indicators()

	if not code_editor.text.strip_edges() or not result:
		status_label.text = ""
		return

	if result.has_error():
		var line = result.get_error_line()
		var col = result.get_error_column()
		status_label.modulate = Color.html("#ff6f6f")
		status_label.text = "Error at (%d, %d): %s" % [
			line,
			col,
			result.get_error_message()
		]

		if line >= 0 and line < code_editor.get_line_count():
			code_editor.mark_error_line(line, result.get_error_message())
	else:
		status_label.modulate = Color.WHITE
		status_label.text = "YAML is valid"

		# Clear status after a delay
		var status_clear_timer = Timer.new()
		status_clear_timer.one_shot = true
		status_clear_timer.wait_time = 3.0  # Clear after 3 seconds
		status_clear_timer.timeout.connect(func(): status_label.text = "")
		add_child(status_clear_timer)
		status_clear_timer.start()
