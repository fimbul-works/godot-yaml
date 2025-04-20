@tool
class_name YAMLValidator extends Node

signal validation_completed(result)

var _thread: Thread
var _mutex: Mutex
var _is_validating: bool = false
var _pending_validation: bool = false
var _current_text: String = ""

var code_editor: YAMLCodeEditor
var validation_timer: Timer
var file_system: YAMLFileSystem

func _ready() -> void:
	file_system = YAMLFileSystem.get_singleton()
	_mutex = Mutex.new()

	# Create validation timer
	validation_timer = Timer.new()
	validation_timer.one_shot = true
	validation_timer.wait_time = 0.5  # 500ms delay
	validation_timer.timeout.connect(_on_validation_timer_timeout)
	add_child(validation_timer)

	# Connect to file system signals
	file_system.file_opened.connect(_on_file_opened)
	file_system.file_updated.connect(_on_file_updated)

func setup(p_code_editor: YAMLCodeEditor) -> void:
	code_editor = p_code_editor

	# Connect to code editor changes
	code_editor.validation_requested.connect(_on_validation_requested)

func _on_validation_requested() -> void:
	# Reset and start the validation timer
	validation_timer.stop()
	validation_timer.start()

func _on_validation_timer_timeout() -> void:
	validate_async(code_editor.text)

func _on_file_opened(path: String) -> void:
	# Validate file when opened
	if is_instance_valid(code_editor) and file_system.is_yaml_file(path):
		validate_async(code_editor.text)

func _on_file_updated(path: String) -> void:
	# Validate file when updated
	if is_instance_valid(code_editor) and file_system.is_yaml_file(path):
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
