@tool
class_name YAMLEditorMenuBar extends MenuBar

signal new_file
signal open_file
signal save_requested
signal save_as_requested
signal close_requested

signal undo_requested
signal redo_requested

signal cut_requested
signal copy_requested
signal paste_requested
signal select_all_requested

signal find_requested
signal find_next_requested
signal find_previous_requested
signal replace_requested

@export var file_menu: PopupMenu
@export var edit_menu: PopupMenu
@export var search_menu: PopupMenu

func _ready() -> void:
	# Wait for UI to be ready
	await get_tree().process_frame

	# Set up the menu bar
	_setup_menus()

func _setup_menus() -> void:
	# File menu
	file_menu.clear()
	file_menu.add_item("New", 0, KEY_MASK_CTRL | KEY_N)
	file_menu.add_item("Open...", 1, KEY_MASK_CTRL | KEY_O)
	file_menu.add_separator()
	file_menu.add_item("Save", 2, KEY_MASK_CTRL | KEY_S)
	file_menu.add_item("Save As...", 3, KEY_MASK_CTRL | KEY_MASK_SHIFT | KEY_S)
	file_menu.add_separator()
	file_menu.add_item("Close", 4, KEY_MASK_CTRL | KEY_W)

	# Edit menu
	edit_menu.clear()
	edit_menu.add_item("Undo", 0, KEY_MASK_CTRL | KEY_Z)
	edit_menu.add_item("Redo", 1, KEY_MASK_CTRL | KEY_MASK_SHIFT | KEY_Z)
	edit_menu.add_separator()
	edit_menu.add_item("Cut", 2, KEY_MASK_CTRL | KEY_X)
	edit_menu.add_item("Copy", 3, KEY_MASK_CTRL | KEY_C)
	edit_menu.add_item("Paste", 4, KEY_MASK_CTRL | KEY_V)
	edit_menu.add_separator()
	edit_menu.add_item("Select All", 5, KEY_MASK_CTRL | KEY_A)

	# Search menu
	search_menu.clear()
	search_menu.add_item("Find...", 0, KEY_MASK_CTRL | KEY_F)
	search_menu.add_item("Find Next", 1, KEY_F3)
	search_menu.add_item("Find Previous", 2, KEY_MASK_SHIFT | KEY_F3)
	search_menu.add_separator()
	search_menu.add_item("Replace...", 3, KEY_MASK_CTRL | KEY_R)

	# Connect signals
	file_menu.id_pressed.connect(_on_file_menu_id_pressed)
	edit_menu.id_pressed.connect(_on_edit_menu_id_pressed)
	search_menu.id_pressed.connect(_on_search_menu_id_pressed)

func _on_file_menu_id_pressed(id: int) -> void:
	match id:
		0: new_file.emit()
		1: open_file.emit()
		2: save_requested.emit()
		3: save_as_requested.emit()
		4: close_requested.emit()

func _on_edit_menu_id_pressed(id: int) -> void:
	match id:
		0: undo_requested.emit()
		1: redo_requested.emit()
		2: cut_requested.emit()
		3: copy_requested.emit()
		4: paste_requested.emit()
		5: select_all_requested.emit()

func _on_search_menu_id_pressed(id: int) -> void:
	match id:
		0: find_requested.emit()
		1: find_next_requested.emit()
		2: find_previous_requested.emit()
		3: replace_requested.emit()
