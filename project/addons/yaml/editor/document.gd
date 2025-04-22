@tool
class_name YAMLEditorDocument extends RefCounted

# Primary document data
var path: String
var content: String
var is_modified: bool = false
var validation_result: YAMLResult

# History management
class HistoryState extends RefCounted:
	var text: String
	var caret_line: int = 0
	var caret_column: int = 0

	func _init(p_text: String, p_line: int = 0, p_column: int = 0) -> void:
		text = p_text
		caret_line = p_line
		caret_column = p_column

	func _to_string() -> String:
		return "HistoryState(text_length=%d, line=%d, column=%d)" % [text.length(), caret_line, caret_column]

# Limit history size to prevent excessive memory use
const MAX_HISTORY := 100

var history_states: Array[HistoryState] = []
var current_history_index: int = -1
var saved_history_index: int = -1

# Signals
signal content_changed(document)
signal validation_changed(document)
signal modified_changed(document)

# Constructor
func _init(p_path: String, p_content: String = "") -> void:
	path = p_path
	content = p_content
	validation_result = YAMLResult.new()  # Empty result

	# Take initial snapshot if content isn't empty
	if not p_content.is_empty():
		_add_history_state(HistoryState.new(p_content))

# File path utilities
func get_file_name() -> String:
	return path.get_file()

func is_untitled() -> bool:
	return path.begins_with("untitled")

# Content management
func set_content(new_content: String, caret_line: int = 0, caret_column: int = 0) -> void:
	if content == new_content:
		return

	content = new_content
	_add_history_state(HistoryState.new(new_content, caret_line, caret_column))
	set_modified(true)
	content_changed.emit(self)

# Modification state
func set_modified(modified: bool) -> void:
	if is_modified == modified:
		return

	is_modified = modified
	modified_changed.emit(self)

# Validation management
func set_validation_result(result: YAMLResult) -> void:
	validation_result = result
	validation_changed.emit(self)

func has_error() -> bool:
	return validation_result and validation_result.has_error()

# History management
func can_undo() -> bool:
	return current_history_index > 0  # Need at least one previous state

func can_redo() -> bool:
	return current_history_index < history_states.size() - 1

func undo() -> HistoryState:
	if not can_undo():
		return null

	current_history_index -= 1
	var state := history_states[current_history_index]
	content = state.text

	# Update modification state
	set_modified(current_history_index != saved_history_index)
	content_changed.emit(self)

	return state

func redo() -> HistoryState:
	if not can_redo():
		return null

	current_history_index += 1
	var state := history_states[current_history_index]
	content = state.text

	# Update modification state
	set_modified(current_history_index != saved_history_index)
	content_changed.emit(self)

	return state

func mark_saved() -> void:
	saved_history_index = current_history_index
	set_modified(false)

func _add_history_state(state: HistoryState) -> void:
	# If we're not at the end of history, truncate future states
	if current_history_index < history_states.size() - 1:
		history_states = history_states.slice(0, current_history_index + 1)

	# Add the new state
	history_states.append(state)
	current_history_index = history_states.size() - 1

	if history_states.size() > MAX_HISTORY:
		var excess := history_states.size() - MAX_HISTORY
		history_states = history_states.slice(excess)
		current_history_index -= excess

		# Adjust saved index if needed
		if saved_history_index >= 0:
			saved_history_index -= excess
			if saved_history_index < 0:
				saved_history_index = -1
