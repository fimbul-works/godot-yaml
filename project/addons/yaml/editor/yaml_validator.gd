@tool
class_name YAMLValidator

signal validation_completed(result)

var _thread: Thread
var _mutex: Mutex
var _is_validating: bool = false
var _pending_validation: bool = false
var _current_text: String = ""
var _result = null

func _init() -> void:
	_mutex = Mutex.new()

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
	_result = result
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
