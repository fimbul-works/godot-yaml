class_name YAMLSyntaxHighlighter extends SyntaxHighlighter

var cache: Dictionary = {}
var syntax_parser := YAMLSyntaxParser.new()
var color_provider := YAMLSyntaxParser.ColorProvider.new()

func clear_highlighting_cache() -> void:
	cache.clear()

func _get_line_syntax_highlighting(line: int) -> Dictionary:
	var text: String = get_text_edit().get_line(line)
	if text in cache:
		return cache[text]

	color_provider.update_theme()
	cache[text] = _highlight_line(text)
	return cache[text]

func _highlight_line(text: String) -> Dictionary:
	return syntax_parser.highlight_line(text, color_provider)
