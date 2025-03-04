extends Node2D
## Example showing YAMLLoader and YAMLWriter interfaces with rich text output

var yaml_text := """
string: string_value
number: 1234
list:
  - apples
  - oranges
"""

func _ready():
	if !visible:
		return
	print_rich("\n[b]💾 YAML Loader/Writer Example[/b]")

	# Load from string
	print_rich("\n[b]Loading from string:[/b]")
	var data := YAMLLoader.load_string(yaml_text)
	print_rich("[i]Result:[/i]\n%s" % data)

	# Save as string
	print_rich("\n[b]Saving to string:[/b]")
	var yaml := YAMLWriter.save_string(data)
	print_rich("[i]Result:[/i]\n%s" % yaml)

	# Test file operations
	print_rich("[b]File Operations:[/b]")

	# Save to file
	print_rich("\n[i]Saving to file...[/i]")
	var success := YAMLWriter.save_file(data, "user://test.yaml")
	if !success:
		print_rich("[color=red]Error saving file[/color]")
	else:
		print_rich("[color=green]✓ File saved successfully[/color]")

	# Load from file
	print_rich("\n[i]Loading from file...[/i]")
	var loaded = YAMLLoader.load_file("user://test.yaml")
	if loaded == null:
		print_rich("[color=red]Error loading file[/color]")
	else:
		print_rich("[color=green]✓ File loaded successfully[/color]")
		print_rich("\n[i]Loaded content:[/i]\n%s" % loaded)
