extends Node2D

func _ready():
	var file = FileAccess.open("res://test.yaml", FileAccess.READ)
	var yaml_input = file.get_as_text().replace("\t", "    ") # TODO

	var yaml = YAML.new()
	# Getting version information
	print(yaml.version())

	# Parsing YAML
	var start = Time.get_ticks_usec()
	var dict = yaml.parse(yaml_input)
	var elapsed = Time.get_ticks_usec() - start
	print_rich("[b]Parsed data:[/b]\n", JSON.stringify(dict, "  ", false))
	print_rich("[i]Parsing took %d microseconds[/i]\n" % elapsed)

	# Stringifying to YAML
	start = Time.get_ticks_usec()
	var yaml_output = yaml.stringify(dict)
	elapsed = Time.get_ticks_usec() - start
	print_rich("[b]Stringified data:[/b]\n", yaml_output)
	print_rich("[i]Stringifying took %d microseconds[/i]\n" % elapsed)

	# Remember to free the object after using it
	yaml.free()
