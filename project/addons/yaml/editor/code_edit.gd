class_name YAMLCodeEdit extends CodeEdit

var syntax_highlighter_script = preload("res://addons/yaml/editor/syntax_highlighting/syntax_highlighter.gd")

func _init() -> void:
	# YAML indentation
	set_indent_size(2)
	set_indent_using_spaces(true)
	indent_automatic = true
	indent_automatic_prefixes = [":"]

	# Syntax highlighting
	if not syntax_highlighter:
		syntax_highlighter = syntax_highlighter_script.new()
