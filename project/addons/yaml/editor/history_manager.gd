@tool
class_name YAMLHistoryManager extends Node

# TextState - Represents just the text content
class TextState extends RefCounted:
	var text: String

	func _init(p_text: String) -> void:
		text = p_text

	func _to_string() -> String:
		return "TextState(length=%d)" % [text.length()]

# CursorState - Represents cursor and scroll position
class CursorState extends RefCounted:
	var caret_line: int
	var caret_column: int
	var scroll_v: float
	var scroll_h: float

	func _init(p_caret_line: int, p_caret_column: int,
			p_scroll_v: float, p_scroll_h: float) -> void:
		caret_line = p_caret_line
		caret_column = p_caret_column
		scroll_v = p_scroll_v
		scroll_h = p_scroll_h

	func _to_string() -> String:
		return "CursorState(line=%d, column=%d)" % [caret_line, caret_column]

# HistoryAction - Represents a complete undoable action
class HistoryAction extends RefCounted:
	var text_before: TextState
	var text_after: TextState
	var cursor_position: CursorState  # Position when this action was created

	func _init(p_text_before: TextState, p_text_after: TextState, p_cursor: CursorState) -> void:
		text_before = p_text_before
		text_after = p_text_after
		cursor_position = p_cursor

	func _to_string() -> String:
		return "HistoryAction(cursor=%s)" % [cursor_position]

# FileHistory - Manages history for a single file
class FileHistory extends RefCounted:
	var actions: Array[HistoryAction] = []
	var current_index: int = -1  # -1 means no history yet
	var saved_index: int = -1    # -1 means never saved

	# The text state when we started editing (or last saved)
	var base_text_state: TextState = null

	func _init(p_initial_text: String) -> void:
		base_text_state = TextState.new(p_initial_text)

	func add_action(action: HistoryAction) -> void:
		# If we're not at the end of history, truncate future actions
		if current_index < actions.size() - 1:
			actions = actions.slice(0, current_index + 1)

		# Don't add if text didn't actually change
		if current_index >= 0 and actions[current_index].text_after.text == action.text_before.text:
			if action.text_before.text == action.text_after.text:
				return

		actions.append(action)
		current_index = actions.size() - 1

	func can_undo() -> bool:
		return current_index >= 0

	func can_redo() -> bool:
		return current_index < actions.size() - 1

	func undo() -> Dictionary:
		if can_undo():
			var action = actions[current_index]
			current_index -= 1

			# When undoing, we want to:
			# 1. Restore the text before the change
			# 2. Keep cursor where it was when the action was created
			return {
				"text": action.text_before.text,
				"cursor": action.cursor_position
			}
		return {}

	func redo() -> Dictionary:
		if can_redo():
			current_index += 1
			var action = actions[current_index]

			# When redoing, we want to:
			# 1. Restore the text after the change
			# 2. Keep cursor where it was when the action was created
			return {
				"text": action.text_after.text,
				"cursor": action.cursor_position
			}
		return {}

	func is_modified() -> bool:
		if current_index < 0:
			return false

		return current_index != saved_index

	func mark_saved() -> void:
		saved_index = current_index

		# Update the base text state
		if current_index >= 0:
			base_text_state = actions[current_index].text_after
		else:
			# No actions yet, so base text is just the initial state
			pass

signal undo_performed(path)
signal redo_performed(path)

const MAX_HISTORY_ACTIONS = 100

var file_manager: YAMLFileManager
var file_system: YAMLFileSystem
var histories: Dictionary = {}  # path: FileHistory
var is_processing_operation: bool = false
var previous_text: String = ""  # Track the previous state for snapshot comparison

func _ready() -> void:
	file_system = YAMLFileSystem.get_singleton()

func setup(p_file_manager: YAMLFileManager) -> void:
	file_manager = p_file_manager

	# Connect to file manager signals
	file_manager.current_file_changed.connect(_on_current_file_changed)

	# Connect to file system signals
	file_system.file_opened.connect(_on_file_opened)
	file_system.file_closed.connect(_on_file_closed)
	file_system.file_saved.connect(_on_file_saved)
	file_system.file_renamed.connect(_on_file_renamed)

# Take a snapshot of the current state
# This should be called when significant changes happen
func take_snapshot() -> void:
	if is_processing_operation:
		return

	var path = file_manager.get_current_file_path()
	if path.is_empty():
		return

	var code_edit = file_manager.code_editor
	var current_text = code_edit.text

	# Skip if the text hasn't changed
	if current_text == previous_text:
		return

	# Get cursor state
	var cursor_state = CursorState.new(
		code_edit.get_caret_line(),
		code_edit.get_caret_column(),
		code_edit.get_v_scroll_bar().value,
		code_edit.get_h_scroll_bar().value
	)

	print("Taking snapshot with caret at line: %d, column: %d" % [cursor_state.caret_line, cursor_state.caret_column])

	# Ensure we have a history object for this file
	if not histories.has(path):
		histories[path] = FileHistory.new(current_text)

	var history = histories[path]

	# Create text states
	var text_before = TextState.new(previous_text)
	var text_after = TextState.new(current_text)

	# If this is the first action, and we don't have a previous text,
	# use the base text from the file history
	if previous_text.is_empty() and history.base_text_state:
		text_before = history.base_text_state

	# Create and add the new action
	var action = HistoryAction.new(text_before, text_after, cursor_state)
	history.add_action(action)

	# Update our tracking state
	previous_text = current_text

	# Limit history size
	_limit_history_size(history)

	# Update UI to show unsaved changes
	_update_modified_flag()

func _limit_history_size(history: FileHistory) -> void:
	if history.actions.size() > MAX_HISTORY_ACTIONS:
		var excess = history.actions.size() - MAX_HISTORY_ACTIONS
		history.actions = history.actions.slice(excess)
		history.current_index -= excess

		# Adjust saved index if needed
		if history.saved_index >= 0:
			history.saved_index -= excess
			if history.saved_index < 0:
				history.saved_index = -1  # No longer saved

func perform_undo() -> void:
	var path = file_manager.get_current_file_path()
	if path.is_empty() or not histories.has(path):
		return

	var history = histories[path]
	if not history.can_undo():
		return

	is_processing_operation = true

	var code_edit = file_manager.code_editor

	# Begin complex operation to avoid creating undo history during our undo
	code_edit.begin_complex_operation()

	# Get the state to restore
	var state = history.undo()
	if state.is_empty():
		is_processing_operation = false
		code_edit.end_complex_operation()
		return

	# Apply text change
	previous_text = state.text
	code_edit.set_text_and_preserve_state(state.text, false)

	var cursor = state.cursor
	print("Undoing with cursor at line: %d, column: %d" % [cursor.caret_line, cursor.caret_column])

	# Restore cursor position
	code_edit.set_caret_line(cursor.caret_line)
	code_edit.set_caret_column(cursor.caret_column)

	# Restore scroll position
	code_edit.get_v_scroll_bar().value = cursor.scroll_v
	code_edit.get_h_scroll_bar().value = cursor.scroll_h

	# Make cursor visible
	code_edit.center_viewport_to_caret()

	# End complex operation
	code_edit.end_complex_operation()

	# Update UI
	_update_modified_flag()

	# Emit signal
	undo_performed.emit(path)

	is_processing_operation = false

func perform_redo() -> void:
	var path = file_manager.get_current_file_path()
	if path.is_empty() or not histories.has(path):
		return

	var history = histories[path]
	if not history.can_redo():
		return

	is_processing_operation = true

	var code_edit = file_manager.code_editor

	# Begin complex operation to avoid creating undo history during our redo
	code_edit.begin_complex_operation()

	# Get the state to restore
	var state = history.redo()
	if state.is_empty():
		is_processing_operation = false
		code_edit.end_complex_operation()
		return

	# Apply text change
	previous_text = state.text
	code_edit.set_text_and_preserve_state(state.text, false)

	var cursor = state.cursor
	print("Redoing with cursor at line: %d, column: %d" % [cursor.caret_line, cursor.caret_column])

	# Restore cursor position
	code_edit.set_caret_line(cursor.caret_line)
	code_edit.set_caret_column(cursor.caret_column)

	# Restore scroll position
	code_edit.get_v_scroll_bar().value = cursor.scroll_v
	code_edit.get_h_scroll_bar().value = cursor.scroll_h

	# Make cursor visible
	code_edit.center_viewport_to_caret()

	# End complex operation
	code_edit.end_complex_operation()

	# Update UI
	_update_modified_flag()

	# Emit signal
	redo_performed.emit(path)

	is_processing_operation = false

func _update_modified_flag() -> void:
	var path = file_manager.get_current_file_path()
	if path.is_empty() or not histories.has(path):
		return

	var history = histories[path]
	file_manager.mark_current_modified(history.is_modified())

func _on_file_opened(path: String) -> void:
	# Initialize history for the new file
	if not histories.has(path):
		# Get initial file content
		var content = ""
		if is_instance_valid(file_manager) and is_instance_valid(file_manager.code_editor):
			content = file_manager.code_editor.text
			previous_text = content

		# Create fresh history
		histories[path] = FileHistory.new(content)

func _on_file_closed(path: String) -> void:
	if histories.has(path):
		histories.erase(path)

func _on_file_saved(path: String) -> void:
	if histories.has(path):
		histories[path].mark_saved()
		_update_modified_flag()

func _on_file_renamed(old_path: String, new_path: String) -> void:
	if histories.has(old_path):
		histories[new_path] = histories[old_path]
		histories.erase(old_path)

func _on_current_file_changed(path: String) -> void:
	# Make sure we have history for the current file
	if not path.is_empty() and not histories.has(path):
		_on_file_opened(path)

	# Update the previous text for the new file
	if is_instance_valid(file_manager) and is_instance_valid(file_manager.code_editor):
		previous_text = file_manager.code_editor.text
