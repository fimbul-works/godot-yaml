@tool
class_name YAMLHistoryManager extends Node

# Simple history action - just track text changes
class HistoryAction extends RefCounted:
	var before_text: String
	var after_text: String

	func _init(p_before: String, p_after: String) -> void:
		before_text = p_before
		after_text = p_after

	func _to_string() -> String:
		return "HistoryAction(before=%d chars, after=%d chars)" % [before_text.length(), after_text.length()]

# Per-file history data
class FileHistory extends RefCounted:
	var actions: Array[HistoryAction] = []
	var current_index: int = -1
	var saved_index: int = -1  # Position where file was last saved

	# The text state when we started editing (or last saved)
	var base_text: String = ""

	func _init(p_initial_text: String) -> void:
		base_text = p_initial_text

	func add_action(action: HistoryAction) -> void:
		# If we're not at the end of history, truncate future actions
		if current_index < actions.size() - 1:
			actions = actions.slice(0, current_index + 1)

		# Don't add if text didn't actually change
		if action.before_text == action.after_text:
			return

		actions.append(action)
		current_index = actions.size() - 1

	func can_undo() -> bool:
		return current_index >= 0

	func can_redo() -> bool:
		return current_index < actions.size() - 1

	func undo() -> String:
		if can_undo():
			var action = actions[current_index]
			current_index -= 1
			return action.before_text
		return ""

	func redo() -> String:
		if can_redo():
			current_index += 1
			var action = actions[current_index]
			return action.after_text
		return ""

	func is_modified() -> bool:
		if current_index < 0:
			return false
		return current_index != saved_index

	func mark_saved() -> void:
		saved_index = current_index

		# Update the base text state
		if current_index >= 0:
			base_text = actions[current_index].after_text

signal undo_performed(path)
signal redo_performed(path)
signal text_restored(text)

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

	var current_text = file_manager.get_current_file_content()

	# Skip if the text hasn't changed
	if current_text == previous_text:
		return

	# Ensure we have a history object for this file
	if not histories.has(path):
		histories[path] = FileHistory.new(current_text)

	var history = histories[path]

	# Create the new action
	var action = HistoryAction.new(previous_text, current_text)

	# If this is the first action, and we don't have a previous text,
	# use the base text from the file history
	if previous_text.is_empty() and not history.base_text.is_empty():
		action.before_text = history.base_text

	# Add the new action
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

	# Get the state to restore
	var text = history.undo()
	if text.is_empty():
		is_processing_operation = false
		return

	# Update our tracking state
	previous_text = text

	# Emit signal to update text
	text_restored.emit(text)

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

	# Get the state to restore
	var text = history.redo()
	if text.is_empty():
		is_processing_operation = false
		return

	# Update our tracking state
	previous_text = text

	# Emit signal to update text
	text_restored.emit(text)

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
		if is_instance_valid(file_manager):
			content = file_manager.get_current_file_content()
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
	if is_instance_valid(file_manager):
		previous_text = file_manager.get_current_file_content()
