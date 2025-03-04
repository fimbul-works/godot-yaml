extends Node2D
## Example showing YAML style handling and customization with rich text output

func _ready():
	if !visible:
		return
	print_rich("\n[b]🎨 YAML Style Handling Examples[/b]")
	test_style_detection()
	test_style_modification()
	test_number_formats()
	test_scalar_styles()

func test_style_detection():
	print_rich("\n[b]Testing Style Detection:[/b]")

	# Parse the comprehensive example with style detection enabled
	var yaml := load_yaml_file("res://addons/yaml/examples/comprehensive.yaml")
	var result := YAML.parse(yaml, true) # Enable style detection

	if result.has_error():
		print_rich("[color=red]Parse failed: %s[/color]" % result.get_error_message())
		return

	if !result.has_style():
		print_rich("[color=yellow]Warning: No style was detected[/color]")
		return

	var style := result.get_style()
	var data = result.get_data()

	print_rich("[i]Original data:[/i]")
	print(data)

	# Test re-emission with detected style
	print_rich("\n[i]Re-emitting with detected style...[/i]")
	var styled_result := YAML.stringify(data, style)
	if styled_result.has_error():
		print_rich("[color=red]Styled stringify failed: %s[/color]" % styled_result.get_error_message())
		return

	print_rich("[color=green]✓ Re-emission successful[/color]")
	print_rich("\n[i]Styled output:[/i]\n%s" % styled_result.get_data())

func test_style_modification():
	print_rich("[b]Testing Style Modification:[/b]")

	# Create and modify a custom style
	var style := YAML.create_style()
	style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)  # Use | for multiline strings
	style.set_quote_style(YAMLStyle.QUOTE_DOUBLE)     # Use " for strings
	style.set_flow_style(YAMLStyle.FLOW_SINGLE)       # Use [] and {} style

	print_rich("[i]Created root style with:[/i]")
	print_rich("  • Literal scalar style (|)")
	print_rich("  • Double quote style (\")")
	print_rich("  • Single flow style ([]/{}")

	# Create a different style for nested nodes
	var nested_style := YAML.create_style()
	nested_style.set_flow_style(YAMLStyle.FLOW_NONE)  # Use block style
	style.set_child("nested", nested_style)

	print_rich("\n[i]Created nested style with:[/i]")
	print_rich("  • Block flow style")

	var data := {
		"string": "Hello\nWorld",
		"nested": {
			"list": [1, 2, 3],
			"mapping": {"a": 1, "b": 2}
		}
	}

	print_rich("\n[i]Applying styles to test data...[/i]")
	var result := YAML.stringify(data, style)
	if result.has_error():
		print_rich("[color=red]Stringify failed: %s[/color]" % result.get_error_message())
		return

	print_rich("[color=green]✓ Style application successful[/color]")
	print_rich("\n[i]Result:[/i]\n%s" % result.get_data())

func test_number_formats():
	print_rich("[b]Testing Number Formats:[/b]")

	# Test integer formats
	var integers := {
		"decimal": 255,
		"byte": 15,
		"large": 1048576  # 2^20
	}

	print_rich("\n[i]Integer Formats:[/i]\n")
	for format in [YAMLStyle.NUM_DECIMAL, YAMLStyle.NUM_HEX,
				  YAMLStyle.NUM_OCTAL, YAMLStyle.NUM_BINARY]:
		var style := YAMLStyle.new()
		style.number_format = format

		var result := YAML.stringify(integers, style)
		if result.has_error():
			print_rich("[color=red]Format %s failed: %s[/color]" %
				[YAMLStyle.get_number_format_string(format), result.get_error_message()])
			continue

		print_rich("[b]%s[/b] (%d):" %
			[YAMLStyle.get_number_format_string(format), format])
		print(result.get_data())

	# Test float formats
	var floats := {
		"pi": 3.14159,
		"small": 0.000314159,
		"large": 31415.9
	}

	print_rich("[i]Float Formats:[/i]\n")
	for format in [YAMLStyle.NUM_DECIMAL, YAMLStyle.NUM_SCIENTIFIC]:
		var style := YAMLStyle.new()
		style.number_format = format

		var result := YAML.stringify(floats, style)
		if result.has_error():
			print_rich("[color=red]Format %s failed: %s[/color]" %
				[YAMLStyle.get_number_format_string(format), result.get_error_message()])
			continue

		print_rich("[b]%s[/b] (%d):" %
			[YAMLStyle.get_number_format_string(format), format])
		print(result.get_data())

func test_scalar_styles():
	print_rich("\n[b]Testing Scalar Styles:[/b]")

	var text := "line 1\nline 2\nline 3\n"
	var style := YAML.create_style()

	# Literal style (|) - preserves newlines
	style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)
	var literal_result := YAML.stringify(text, style)
	if literal_result.has_error():
		print_rich("[color=red]Literal style failed: %s[/color]" % literal_result.get_error_message())
	else:
		print_rich("\n[i]Literal style (|):[/i]")
		print(literal_result.get_data())
		if !literal_result.get_data().contains("|"):
			print_rich("[color=yellow]Warning: Literal style not applied[/color]")

	# Folded style (>) - folds newlines to spaces
	style.set_scalar_style(YAMLStyle.SCALAR_FOLDED)
	var folded_result := YAML.stringify(text, style)
	if folded_result.has_error():
		print_rich("[color=red]Folded style failed: %s[/color]" % folded_result.get_error_message())
	else:
		print_rich("\n[i]Folded style (>):[/i]")
		print(folded_result.get_data())
		if !folded_result.get_data().contains(">"):
			print_rich("[color=yellow]Warning: Folded style not applied[/color]")

func load_yaml_file(path: String) -> String:
	var file := FileAccess.open(path, FileAccess.READ)
	if not file:
		print_rich("[color=red]Could not open file: %s[/color]" % path)
		return ""
	return file.get_as_text()
