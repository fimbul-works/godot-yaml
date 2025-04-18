@tool
class_name YAMLUndoRedoManager
extends Node

signal undo_performed(path)
signal redo_performed(path)

# History structure: {path: {states: [], index: -1, saved_index: -1}}
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
	var current_text = file_manager.get_current_file_content()

	# If we're already processing an undo/redo, don't take a new snapshot
	if is_processing_operation:
		return

	# If the current state is different from the last state
	if current_history.states.is_empty() or current_text != current_history.states[current_history.index]:
		# If we're not at the end of the history, truncate future states
		if current_history.index < current_history.states.size() - 1:
			current_history.states = current_history.states.slice(0, current_history.index + 1)

		# Add the new state
		current_history.states.append(current_text)
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
		file_manager.set_current_file_content(previous_state)

		# Update the modified flag based on whether current state matches saved state
		_update_modified_flag()

		# Emit signal for completion
		undo_performed.emit(path)

		is_processing_operation = false

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
		file_manager.set_current_file_content(next_state)

		# Update the modified flag based on whether current state matches saved state
		_update_modified_flag()

		# Emit signal for completion
		redo_performed.emit(path)

		is_processing_operation = false

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
		var initial_content = file_manager.get_current_file_content()
		history[path] = {
			"states": [initial_content],
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
