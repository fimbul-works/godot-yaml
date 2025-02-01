extends Node

@export var is_enabled: bool = true

func _ready():
	if !is_enabled:
		return
	var file = FileAccess.open("res://yaml_data/comprehensive.yaml", FileAccess.READ)
	var yaml_input = file.get_as_text().replace("\t", "    ") # TODO

	var parse_res = YAML.parse(yaml_input, true)
	if parse_res.has_error():
		printerr(parse_res.get_error())
		return
	var data = parse_res.get_data()
	var style = parse_res.get_style()

	#print(style.get_debug_string())

	var emit_result = YAML.emit(data)
	if emit_result.has_error():
		printerr(emit_result.get_error())
		return
	var yaml_raw = emit_result.get_data()
	print("RAW EMIT\n\n" + yaml_raw + "\n")
	emit_result = YAML.emit(data, style)
	var yaml_style = emit_result.get_data()
	print("STYLED EMIT\n\n" + yaml_style + "\n")
	print("ORIGINAL YAML\n\n" + yaml_input + "\n")
