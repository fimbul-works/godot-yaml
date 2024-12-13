extends Node2D

@export var local_resource: Resource

@export var is_enabled: bool = true

func _ready():
	if !is_enabled:
		return
	var file = FileAccess.open("res://yaml_data/comprehensive.yaml", FileAccess.READ)
	var yaml_input = file.get_as_text().replace("\t", "    ") # TODO

	# Show version information
	print(YAML.version())

	# Test parsing speed
	var dict = test_parsing(yaml_input)
	if !dict:
		printerr("Parsing failed. Bummer!")
		return

	## Test emitting speed
	var yaml = test_emitting(dict)
	if !yaml:
		printerr("Emitting failed. Bogus!")
		return

func test_parsing(input: String) -> Variant:
	return time_call(func():
		return YAML.parse(input)
	, "Parsing")

func test_emitting(input: Variant) -> Variant:
	return time_call(func():
		return YAML.emit(input)
	, "Emitting")

## Time a function call for performance
func time_call(fn: Callable, label: String = "Operation") -> Variant:
	var start = Time.get_ticks_usec()
	var result = fn.call()
	var elapsed = Time.get_ticks_usec() - start
	if result.has_error():
		printerr("%s failed: " % label, result.get_error())
		return
	var data = result.get_data()
	if typeof(data) == TYPE_STRING:
		print_rich("[b]%s Output:[/b]\n" % label, data)
	else:
		print_rich("[b]%s Output:[/b]\n" % label, JSON.stringify(data, "  ", false))
	print_rich("[i]Operation completed in %d microseconds[/i]\n" % elapsed)
	return data
