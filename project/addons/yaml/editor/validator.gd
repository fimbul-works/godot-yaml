@tool
class_name YAMLEditorValidator extends Node

signal validation_completed(document)

var _thread: Thread
var _is_validating: bool = false
var _pending_validation: bool = false
var _validation_queue: Array = []

var code_editor: YAMLCodeEditor
var validation_timer: Timer
var file_system: YAMLFileSystem
var file_manager: YAMLEditorDocumentManager

func _ready() -> void:
	file_system = YAMLFileSystem.get_singleton()

	# Create validation timer
	validation_timer = Timer.new()
	add_child(validation_timer)
	validation_timer.one_shot = true
	validation_timer.wait_time = 0.5  # 500ms delay
	validation_timer.timeout.connect(_on_validation_timer_timeout)

func _exit_tree() -> void:
	if _thread and _thread.is_started():
		_thread.wait_to_finish()

func setup(p_code_editor: YAMLCodeEditor, p_file_manager: YAMLEditorDocumentManager) -> void:
	code_editor = p_code_editor
	file_manager = p_file_manager

	# Connect to code editor changes
	code_editor.validation_requested.connect(_on_validation_requested)

	# Connect to document changes
	file_manager.document_changed.connect(_on_document_changed)
	file_manager.document_created.connect(_on_document_created)

func _on_validation_requested() -> void:
	# Reset and start the validation timer
	validation_timer.stop()
	validation_timer.start()

func _on_validation_timer_timeout() -> void:
	var document = file_manager.get_current_document()
	if document:
		validate_document(document)

func _on_document_changed(document: YAMLEditorDocument) -> void:
	# Show any existing validation results
	if document.validation_result:
		validation_completed.emit(document)

	# Run validation if no results exist or document has errors
	if document.validation_result == null or document.has_error():
		validate_document(document)

func _on_document_created(document: YAMLEditorDocument) -> void:
	# Validate new document
	validate_document(document)

func validate_document(document: YAMLEditorDocument) -> void:
	if document == null:
		return

	if _is_validating:
		# Add to validation queue
		if not _validation_queue.has(document):
			_validation_queue.append(document)
		return

	_is_validating = true

	if _thread and _thread.is_started():
		_thread.wait_to_finish()

	_thread = Thread.new()
	_thread.start(_validation_thread_function.bind(document))

func _validation_thread_function(document: YAMLEditorDocument) -> void:
	# YAML validation is thread-safe
	var result = YAML.validate_syntax(document.content)

	# Update document on main thread
	call_deferred("_finish_validation", document, result)

func _finish_validation(document: YAMLEditorDocument, result: YAMLResult) -> void:
	# Update document with validation result
	document.set_validation_result(result)

	# Emit signal
	validation_completed.emit(document)

	# Process any pending validations
	_is_validating = false

	if not _validation_queue.is_empty():
		var next_document = _validation_queue.pop_front()
		validate_document(next_document)

func mark_error_in_editor(line: int, message: String) -> void:
	if is_instance_valid(code_editor):
		code_editor.mark_error_line(line, message)

func clear_errors_in_editor() -> void:
	if is_instance_valid(code_editor):
		code_editor.clear_error_indicators()
