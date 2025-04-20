@tool
class_name YAMLUndoRedoManager
extends Node

signal undo_performed(path)
signal redo_performed(path)

# Enhanced history structure:
# {path: {states: [], index: -1, saved_index: -1}}
# Where each state is now a dictionary with:
# {text, caret_line, caret_column, scroll_v, scroll_h}
var history: Dictionary = {}
const MAX_UNDO_STATES = 100

var file_manager: YAMLFileManager
var snapshot_timestamp: int = 0
const SNAPSHOT_INTERVAL_MS = 300  # Minimum interval between snapshots
var is_processing_operation: bool = false

func _ready() -> void:
	pass

func setup(p_file_manager: YAMLFileManager) -> void:
	file_manager = p_file_manager

	# Connect to file manager signals
	file_manager.file_opened.connect(_on_file_opened)
	file_manager.file_closed.connect(_on_file_closed)
	file_manager.file_saved.connect(_on_file_saved)

func take_snapshot() -> void:
	var path = file_manager.get_current_file_path()
	if path.is_empty():
		return

	# Check if enough time has passed since last snapshot
	var current_time = Time.get_ticks_msec()
	if current_time - snapshot_timestamp < SNAPSHOT_INTERVAL_MS:
		return

	snapshot_timestamp = current_time

	# Initialize history for this file if it doesn't exist
	if not history.has(path):
		history[path] = {
			"states": [],
			"index": -1,
			"saved_index": -1
		}

	var current_history = history[path]
	var code_edit = file_manager.code_editor

	# Create a state object with text and cursor/scroll information
	var state = {
		"text": code_edit.text,
		"caret_line": code_edit.get_caret_line(),
		"caret_column": code_edit.get_caret_column(),
		"scroll_v": code_edit.get_v_scroll_bar().value,
		"scroll_h": code_edit.get_h_scroll_bar().value
	}

	# If we're already processing an undo/redo, don't take a new snapshot
	if is_processing_operation:
		return

	# If the current state is different from the last state
	if current_history.states.is_empty() or state.text != current_history.states[current_history.index].text:
		# If we're not at the end of the history, truncate future states
		if current_history.index < current_history.states.size() - 1:
			current_history.states = current_history.states.slice(0, current_history.index + 1)

		# Add the new state
		current_history.states.append(state)
		current_history.index = current_history.states.size() - 1

		# Limit the number of states to prevent memory issues
		if current_history.states.size() > MAX_UNDO_STATES:
			var excess = current_history.states.size() - MAX_UNDO_STATES
			current_history.states = current_history.states.slice(excess)
			current_history.index = current_history.states.size() - 1

			# Adjust saved index if needed
			if current_history.saved_index >= 0:
				current_history.saved_index -= excess
				if current_history.saved_index < 0:
					current_history.saved_index = -1

		# Update UI to show there are unsaved changes
		_update_modified_flag()

func perform_undo() -> void:
	var path = file_manager.get_current_file_path()
	if path.is_empty() or not history.has(path):
		return

	var current_history = history[path]

	# Check if we can undo
	if current_history.index > 0:
		is_processing_operation = true

		current_history.index -= 1
		var previous_state = current_history.states[current_history.index]

		# Apply the state to the editor
		var code_edit = file_manager.code_editor

		# Apply state using a deferred call to handle async correctly
		call_deferred("_process_state_change", previous_state, code_edit, path, false)

func perform_redo() -> void:
	var path = file_manager.get_current_file_path()
	if path.is_empty() or not history.has(path):
		return

	var current_history = history[path]

	# Check if we can redo
	if current_history.index < current_history.states.size() - 1:
		is_processing_operation = true

		current_history.index += 1
		var next_state = current_history.states[current_history.index]

		# Apply the state to the editor
		var code_edit = file_manager.code_editor

		# Apply state using a deferred call to handle async correctly
		call_deferred("_process_state_change", next_state, code_edit, path, true)

# Helper method to process state changes with async operations
func _process_state_change(state, code_edit, path: String, is_redo: bool) -> void:
	# Create a new function to handle the async operations
	_apply_state_async.call_deferred(state, code_edit, path, is_redo)

# Async method to apply state and handle completion
func _apply_state_async(state, code_edit, path: String, is_redo: bool) -> void:
	await _apply_state(state, code_edit)

	# Update the modified flag
	_update_modified_flag()

	# Emit appropriate signal
	if is_redo:
		redo_performed.emit(path)
	else:
		undo_performed.emit(path)

	# Reset processing flag
	is_processing_operation = false

# Helper method to apply a state to the code editor
func _apply_state(state, code_edit) -> void:
	# First, set the text content
	code_edit.text = state.text

	# Wait for the next frame to ensure text processing is complete
	await code_edit.get_tree().process_frame

	# Now restore cursor position after the text has been processed
	if state.caret_line < code_edit.get_line_count():
		code_edit.set_caret_line(state.caret_line)

		var line_length = code_edit.get_line(state.caret_line).length()
		if state.caret_column <= line_length:
			code_edit.set_caret_column(state.caret_column)

	# Restore scroll position (with a small delay to ensure the text is updated first)
	await code_edit.get_tree().process_frame
	code_edit.get_v_scroll_bar().value = state.scroll_v
	code_edit.get_h_scroll_bar().value = state.scroll_h

	# Make sure caret becomes visible
	code_edit.center_viewport_to_caret()

func clear_history(path: String) -> void:
	if history.has(path):
		history.erase(path)

func _update_modified_flag() -> void:
	var path = file_manager.get_current_file_path()
	if path.is_empty() or not history.has(path):
		return

	var current_history = history[path]
	var is_modified = (current_history.index != current_history.saved_index)

	# Only update if needed
	file_manager.mark_current_modified(is_modified)

func _on_file_opened(path: String) -> void:
	# Initialize history for the new file
	if not history.has(path):
		var code_edit = file_manager.code_editor

		var initial_state = {
			"text": code_edit.text,
			"caret_line": code_edit.get_caret_line(),
			"caret_column": code_edit.get_caret_column(),
			"scroll_v": code_edit.get_v_scroll_bar().value,
			"scroll_h": code_edit.get_h_scroll_bar().value
		}

		history[path] = {
			"states": [initial_state],
			"index": 0,
			"saved_index": 0  # Initially, the file is in a saved state
		}

func _on_file_closed(path: String) -> void:
	# Clean up history for the closed file
	clear_history(path)

func _on_file_saved(path: String) -> void:
	# Mark current state as saved
	if history.has(path):
		var current_history = history[path]
		current_history.saved_index = current_history.index

		# Update modified flag
		_update_modified_flag()
