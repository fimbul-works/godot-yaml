@tool
extends EditorPlugin

const HIDE_NATIVE_LIST = true

var _scripts_tab_container: TabContainer
var _scripts_tab_bar: TabBar
var _scripts_item_list: ItemList
var _prev_state := TabContainerState.new()
var _last_tab_selected = -1
var _last_tab_hovered = -1


func _enter_tree() -> void:
	var script_editor = get_editor_interface().get_script_editor()
	_scripts_tab_container = first_or_null(script_editor.find_children(
			"*", "TabContainer", true, false
		)
	)
	_scripts_item_list = first_or_null(script_editor.find_children(
		"*", "ItemList", true, false
	))
	if _scripts_tab_container:
		_scripts_tab_bar = get_tab_bar_of(_scripts_tab_container)
		_prev_state.save(_scripts_tab_container, _scripts_tab_bar)
		_scripts_tab_container.tabs_visible = true
		_scripts_tab_container.drag_to_rearrange_enabled = true
		_scripts_tab_container.sort_children.connect(_update_tabs)
	if _scripts_tab_bar:
		_scripts_tab_bar.tab_close_display_policy = TabBar.CLOSE_BUTTON_SHOW_ACTIVE_ONLY
		_scripts_tab_bar.select_with_rmb = true
		_scripts_tab_bar.drag_to_rearrange_enabled = true
		_scripts_tab_bar.tab_close_pressed.connect(_on_tab_close)
		_scripts_tab_bar.tab_rmb_clicked.connect(_on_tab_rmb)
		_scripts_tab_bar.tab_selected.connect(_on_tab_selected)
		_scripts_tab_bar.tab_hovered.connect(_on_tab_hovered)
		_scripts_tab_bar.mouse_exited.connect(_on_tab_bar_mouse_exited)
		_scripts_tab_bar.active_tab_rearranged.connect(_on_active_tab_rearranged)
		_scripts_tab_bar.gui_input.connect(_on_scripts_tab_bar_gui_input)
	if _scripts_item_list:
		if HIDE_NATIVE_LIST:
			_scripts_item_list.get_parent().visible = false
		_scripts_item_list.property_list_changed.connect(_on_item_list_property_list_changed)
	_update_tabs()


func _exit_tree() -> void:
	if _scripts_tab_container:
		_scripts_tab_bar = get_tab_bar_of(_scripts_tab_container)
		_prev_state.restore(_scripts_tab_container, _scripts_tab_bar)
		_scripts_tab_container.sort_children.disconnect(_update_tabs)
	if _scripts_item_list:
		if HIDE_NATIVE_LIST:
			_scripts_item_list.get_parent().visible = true
		_scripts_item_list.property_list_changed.disconnect(_on_item_list_property_list_changed)
	if _scripts_tab_bar:
		_scripts_tab_bar.mouse_exited.disconnect(_on_tab_bar_mouse_exited)
		_scripts_tab_bar.gui_input.disconnect(_on_scripts_tab_bar_gui_input)
		_scripts_tab_bar.tab_close_pressed.disconnect(_on_tab_close)
		_scripts_tab_bar.tab_rmb_clicked.disconnect(_on_tab_rmb)
		_scripts_tab_bar.tab_selected.disconnect(_on_tab_selected)
		_scripts_tab_bar.tab_hovered.disconnect(_on_tab_hovered)
		_scripts_tab_bar.active_tab_rearranged.disconnect(_on_active_tab_rearranged)


func _on_tab_bar_mouse_exited():
	_last_tab_hovered = -1
	

func _on_tab_hovered(idx):
	_last_tab_hovered = idx


func _on_scripts_tab_bar_gui_input(event: InputEvent):
	if event is InputEventMouseMotion:
		var tab_control = _scripts_tab_container.get_tab_control(_last_tab_hovered)
		var path = ''
		if tab_control:
			path = tab_control.get("metadata/_edit_res_path")
		_scripts_tab_bar.tooltip_text = '' if path == null else path
	if _last_tab_hovered == -1: return
	if event is InputEventMouseButton:
		if event.is_pressed() and event.button_index == MOUSE_BUTTON_MIDDLE:
			_simulate_item_clicked(_last_tab_hovered, MOUSE_BUTTON_MIDDLE)


func _on_active_tab_rearranged(_idx_to):
	var control = _scripts_tab_container.get_tab_control(_last_tab_selected)
	if not control:
		return
	_scripts_tab_container.move_child(control, _idx_to)
	_scripts_tab_container.current_tab = _scripts_tab_container.current_tab
	_trigger_script_editor_update_script_names()


func _on_tab_selected(tab_idx):
	_last_tab_selected = tab_idx
	var item_idx = _find_list_item_idx_by_tab_idx(tab_idx)
	if item_idx != -1:
		if not _scripts_item_list.is_selected(item_idx):
			var select_scripts_item = func():
				_scripts_item_list.select(item_idx)
				_scripts_item_list.item_selected.emit(item_idx)
			select_scripts_item.call_deferred()


func _on_tab_rmb(tab_idx):
	_simulate_item_clicked(tab_idx, MOUSE_BUTTON_RIGHT)


func _on_tab_close(tab_idx):
	_simulate_item_clicked(tab_idx, MOUSE_BUTTON_MIDDLE)


func _on_item_list_property_list_changed():
	_update_tabs.call_deferred()


func _simulate_item_clicked(tab_idx, mouse_idx):
	if _scripts_item_list:
		var item_idx = _find_list_item_idx_by_tab_idx(tab_idx)
		if item_idx != -1:
			_scripts_item_list.item_clicked.emit(
				item_idx,
				_scripts_item_list.get_local_mouse_position(),
				mouse_idx
			)


func _update_tabs():
	_update_tab_names()
	_update_tab_icons()


func _update_tab_names():
	if not _scripts_tab_container or not _scripts_item_list:
		return

	for item_idx in _scripts_item_list.item_count:
		var tab_idx = _get_item_list_tab_idx(item_idx)
		if tab_idx != -1:
			_scripts_tab_container.set_tab_title(
				tab_idx, _scripts_item_list.get_item_text(item_idx)
			)


func _update_tab_icons():
	if not _scripts_tab_container or not _scripts_item_list:
		return
	
	for item_idx in _scripts_item_list.item_count:
		var tab_idx = _get_item_list_tab_idx(item_idx)
		if tab_idx != -1:
			_scripts_tab_container.set_tab_icon(
				tab_idx, _scripts_item_list.get_item_icon(item_idx)
			)


func _get_item_list_tab_idx(item_idx) -> int:
	var metadata = _scripts_item_list.get_item_metadata(item_idx)
	if not metadata is int:
		return -1
	else:
		return metadata


func _find_list_item_idx_by_tab_idx(tab_idx) -> int:
	for i in _scripts_item_list.item_count:
		if _scripts_item_list.get_item_metadata(i) == tab_idx:
			return i
	return -1


func _trigger_script_editor_update_script_names():
	var script_editor = get_editor_interface().get_script_editor()
	# for now it is the only way to trigger script_edtior._update_script_names
	script_editor.notification(Control.NOTIFICATION_THEME_CHANGED)


static func first_or_null(arr):
	if len(arr) == 0:
		return null
	return arr[0]


static func get_tab_bar_of(src) -> TabBar:
	for c in src.get_children(true):
		if c is TabBar:
			return c
	return null


class TabContainerState:
	var _tabs_visible
	var _drag_to_rearrange_enabled
	var _tab_close_display_policy
	var _select_with_rmb
	
	func save(src: TabContainer, tab_bar: TabBar):
		if src:
			_tabs_visible = src.tabs_visible
		if tab_bar:
			_drag_to_rearrange_enabled = tab_bar.drag_to_rearrange_enabled
			_tab_close_display_policy = tab_bar.tab_close_display_policy
			_select_with_rmb = tab_bar.select_with_rmb
	
	func restore(src: TabContainer, tab_bar: TabBar):
		if src:
			src.tabs_visible = _tabs_visible
		if tab_bar:
			tab_bar.drag_to_rearrange_enabled = _drag_to_rearrange_enabled
			tab_bar.tab_close_display_policy = _tab_close_display_policy
			tab_bar.select_with_rmb = _select_with_rmb
