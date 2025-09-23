@tool
class_name YAMLSyntaxParser extends RefCounted

## Token types
enum TokenType {
	TEXT,               # For keys only
	COMMENT,            # Comments
	SYMBOL,             # Structural elements like :, -, >, |, &, *, [, ], {, }
	STRING,             # String values (default for unmatched values)
	NUMBER,             # Numeric values
	KEYWORD,            # Booleans, null, merge keys, tags
	DOCUMENT_SEPARATOR, # New document separator
}

var re_patterns := {
	"merge_key": RegEx.create_from_string("^\\s*<<:\\s*\\*[^\\s]+"),
	"multiline_indicator": RegEx.create_from_string("(>|\\|-?)\\s*$"),
	"array_item": RegEx.create_from_string("^(\\s*-(?:\\s*-)*\\s*)(.*)$"),
	"key_value": RegEx.create_from_string("^\\s*([^:]+):(.*)$"),

	# Scalar patterns
	"quoted_string": RegEx.create_from_string("^(['\"])(?:\\\\.|[^\\\\])*\\1$"),
	"number": RegEx.create_from_string("^(?:0[xX][0-9a-fA-F]+|0[oO][0-7]+|0[bB][0-1]+|[-+]?(?:\\d+\\.?\\d*|\\.\\d+)(?:[eE][-+]?\\d+)?)$"),
	"boolean": RegEx.create_from_string("^(true|false)$"),
	"nullish": RegEx.create_from_string("^(null|~)$"),
	"special": RegEx.create_from_string("^(\\.inf|\\.nan)$"),

	# YAML functionality
	"anchor": RegEx.create_from_string("^\\s*&([^\\s]+)"),
	"alias": RegEx.create_from_string("^\\s*\\*([^\\s]+)"),
	"tag": RegEx.create_from_string("!(?:![\\w\\-\\.]+|[^\\s\\[\\]{}'\"`]+)"),
	"top_level_tag": RegEx.create_from_string("^\\s*(!(?:![\\w\\-\\.]+|[^\\s\\[\\]{}'\"`]+))\\s*(.*)$"),
	"document_separator": RegEx.create_from_string("^---$")
}

class ParserState:
	var in_string: bool = false
	var string_char: String = ""
	var stack: Array = []  # For nested flow collections
	var token_start: int = -1
	var colors: Dictionary = {}

	func push(char: String) -> void:
		stack.push_back(char)

	func pop() -> String:
		return stack.pop_back() if not stack.is_empty() else ""

	func peek() -> String:
		return stack.back() if not stack.is_empty() else ""

## Provides colors to the syntax highlighter
class ColorProvider:
	# Default theme colors
	var theme: Dictionary = {
		"text": Color(0.8025, 0.81, 0.8225, 1),
		"comment": Color(0.8025, 0.81, 0.8225, 0.5),
		"symbol": Color(0.67, 0.79, 1, 1),
		"string": Color(1, 0.93, 0.63, 1),
		"number": Color(0.63, 1, 0.88, 1),
		"keyword": Color(1, 0.44, 0.52, 1),
		"document_separator": Color(0.8025, 0.81, 0.8225, 0.5)
	}

	func _init():
		update_theme()

	## Updates the theme colors from Godot Editor settings
	func update_theme() -> void:
		# Only inside the Godot Editor
		if !Engine.is_editor_hint():
			return

		# Read theme from editor settings
		var settings = EditorInterface.get_editor_settings()
		theme = {
			"text": settings.get_setting("text_editor/theme/highlighting/text_color"),
			"comment": settings.get_setting("text_editor/theme/highlighting/comment_color"),
			"symbol": settings.get_setting("text_editor/theme/highlighting/symbol_color"),
			"string": settings.get_setting("text_editor/theme/highlighting/string_color"),
			"number": settings.get_setting("text_editor/theme/highlighting/number_color"),
			"keyword": settings.get_setting("text_editor/theme/highlighting/keyword_color"),
			"document_separator": settings.get_setting("text_editor/theme/highlighting/comment_color"),
		}

	## Get color for tokens
	func get_color_for_type(type: TokenType) -> Color:
		match type:
			YAMLSyntaxParser.TokenType.TEXT: return theme.text
			YAMLSyntaxParser.TokenType.COMMENT: return theme.comment
			YAMLSyntaxParser.TokenType.SYMBOL: return theme.symbol
			YAMLSyntaxParser.TokenType.STRING: return theme.string
			YAMLSyntaxParser.TokenType.NUMBER: return theme.number
			YAMLSyntaxParser.TokenType.KEYWORD: return theme.keyword
			YAMLSyntaxParser.TokenType.DOCUMENT_SEPARATOR: return theme.document_separator
			_: return theme.string # Default fallback is string color

## Highlight a line of YAML
func highlight_line(text: String, color_provider: ColorProvider = ColorProvider.new()) -> Dictionary:
	var comment_pos := _find_comment_start(text)
	var content := text if comment_pos == -1 else text.substr(0, comment_pos).rstrip(" \t")
	var colors := {}

	if content.strip_edges():
		colors = _highlight_line_content(content, color_provider)

	if comment_pos != -1:
		colors[comment_pos] = {"color": color_provider.get_color_for_type(TokenType.COMMENT)}

	return _sort_colors(colors)

# Find the beginning position of a comment
func _find_comment_start(text: String) -> int:
	var in_string := false
	var string_char := ""

	for i in range(text.length()):
		var char := text[i]

		if not in_string:
			if char in ['"', "'"] and (i == 0 or text[i - 1] != '\\'):
				in_string = true
				string_char = char
			elif char == '#' and (i == 0 or text[i - 1] == ' ' or text[i - 1] == '\t'):
				return i
		else:
			if char == string_char and (i == 0 or text[i - 1] != '\\'):
				in_string = false
				string_char = ""

	return -1

# Highlight line content
func _highlight_line_content(text: String, color_provider: ColorProvider) -> Dictionary:
	# Handle document separator
	var separator_match: RegExMatch = re_patterns.document_separator.search(text)
	if separator_match:
		return {0: {"color": color_provider.get_color_for_type(TokenType.DOCUMENT_SEPARATOR)}}

	# Handle merge keys
	var merge_match: RegExMatch = re_patterns.merge_key.search(text)
	if merge_match:
		return {
			merge_match.get_start(0): {"color": color_provider.get_color_for_type(TokenType.KEYWORD)}
		}

	# Check for top-level tags
	var tag_match: RegExMatch = re_patterns.top_level_tag.search(text)
	if tag_match:
		var colors := {}
		# Color just the tag part as keyword (red)
		_add_color(color_provider, colors, tag_match.get_start(1), tag_match.get_end(1), TokenType.KEYWORD)

		# Process any remaining content after the tag
		var remaining = tag_match.get_string(2).strip_edges()
		if remaining:
			var remaining_start = text.find(remaining, tag_match.get_end(1))
			if remaining_start != -1:
				if remaining.begins_with("{") or remaining.begins_with("["):
					colors.merge(_parse_flow_style(color_provider, remaining, remaining_start))
				else:
					_add_scalar_color(color_provider, colors, remaining, remaining_start)
		return colors

	# Handle array items
	var array_match: RegExMatch = re_patterns.array_item.search(text)
	if array_match:
		var colors := {}

		# Color the entire dash section as symbols
		_add_color(color_provider, colors, array_match.get_start(1), array_match.get_end(1), TokenType.SYMBOL)

		# Process the content after the dashes
		var content: String = array_match.get_string(2).strip_edges()
		if content:
			var content_start: int = array_match.get_start(2)
			if content.begins_with("[") or content.begins_with("{"):
				colors.merge(_parse_flow_style(color_provider, content, content_start))
			else:
				_add_scalar_color(color_provider, colors, content, content_start)
		return colors

	# Handle regular key-value pairs
	var key_value_match: RegExMatch = re_patterns.key_value.search(text)
	if key_value_match:
		return _parse_key_value(color_provider, text, key_value_match)

	# Handle flow-style collections at the root level
	if "[" in text or "{" in text:
		return _parse_flow_style(color_provider, text, 0)

	# Handle multi-line string indicators
	var multiline_match: RegExMatch = re_patterns.multiline_indicator.search(text)
	if multiline_match:
		var colors := {}
		# Color the indicator (> or |) as symbol
		_add_color(color_provider, colors, multiline_match.get_start(1), multiline_match.get_end(1), TokenType.SYMBOL)
		return colors

	# Default case: treat as string content (for multi-line string content)
	if text.strip_edges():
		return {0: {"color": color_provider.get_color_for_type(TokenType.STRING)}}

	return {}

# Parse flow collections
func _parse_flow_style(color_provider: ColorProvider, text: String, offset: int) -> Dictionary:
	var state := ParserState.new()
	var pos := 0

	while pos < text.length():
		var char := text[pos]

		# Handle string literals
		if char in ['"', "'"] and (pos == 0 or text[pos - 1] != '\\'):
			if not state.in_string:
				state.in_string = true
				state.string_char = char
				state.token_start = pos
			elif char == state.string_char:
				state.in_string = false
				_add_color(color_provider, state.colors, offset + state.token_start, offset + pos + 1, TokenType.STRING)
				state.token_start = -1

		# Handle flow collection brackets when not in string
		elif not state.in_string:
			if char in ['[', '{']:
				state.push(char)
				_add_color(color_provider, state.colors, offset + pos, offset + pos + 1, TokenType.SYMBOL)
				state.token_start = pos + 1

			elif char in [']', '}']:
				var matching := '[' if char == ']' else '{'
				if state.peek() == matching:
					state.pop()
					if state.token_start != -1:
						var token := text.substr(state.token_start, pos - state.token_start).strip_edges()
						if token:
							_add_scalar_color(color_provider, state.colors, token, offset + state.token_start)
					_add_color(color_provider, state.colors, offset + pos, offset + pos + 1, TokenType.SYMBOL)
					state.token_start = -1

			elif char in [':', ',']:
				if state.token_start != -1:
					var token := text.substr(state.token_start, pos - state.token_start).strip_edges()
					if token:
						if char == ':':
							# All map keys should be text colored, regardless of content
							_add_color(color_provider, state.colors, offset + state.token_start, offset + pos, TokenType.TEXT)
						else:
							_add_scalar_color(color_provider, state.colors, token, offset + state.token_start)
				_add_color(color_provider, state.colors, offset + pos, offset + pos + 1, TokenType.SYMBOL)
				state.token_start = pos + 1

			elif char != ' ' and state.token_start == -1:
				state.token_start = pos

		pos += 1

	# Handle any remaining token
	if state.token_start != -1 and state.token_start < pos:
		var token := text.substr(state.token_start, pos - state.token_start).strip_edges()
		if token:
			# Check if this is a key in a map context
			if not state.stack.is_empty() and state.stack.back() == '{' and ':' in text.substr(pos):
				_add_color(color_provider, state.colors, offset + state.token_start, offset + pos, TokenType.TEXT)
			else:
				_add_scalar_color(color_provider, state.colors, token, offset + state.token_start)

	return state.colors

# Parse dictionary key and value
func _parse_key_value(color_provider: ColorProvider, text: String, match: RegExMatch) -> Dictionary:
	var colors := {}

	# Color the key
	_add_color(color_provider, colors, match.get_start(1), match.get_end(1), TokenType.TEXT)

	# Color the colon
	_add_color(color_provider,colors, match.get_end(1), match.get_end(1) + 1, TokenType.SYMBOL)

	# Get and process the value if present
	var value := match.get_string(2).strip_edges()
	if value:
		var value_start := text.find(value, match.get_end(1))
		if value_start != -1:
			# First check for and handle any tags
			var tag_match: RegExMatch = re_patterns.tag.search(value)
			if tag_match:
				_add_color(color_provider, colors, value_start + tag_match.get_start(0),
						  value_start + tag_match.get_end(0), TokenType.KEYWORD)
				# Get remaining content after tag
				var after_tag := value.substr(tag_match.get_end(0)).strip_edges()
				if after_tag:
					var after_tag_start = text.find(after_tag, value_start + tag_match.get_end(0))
					if after_tag_start != -1:
						# Now check for multiline indicator in remaining content
						var indicator_match: RegExMatch = re_patterns.multiline_indicator.search(after_tag)
						if indicator_match:
							_add_color(color_provider, colors, after_tag_start + indicator_match.get_start(1), after_tag_start + indicator_match.get_end(1), TokenType.SYMBOL)
						elif after_tag.begins_with("{") or after_tag.begins_with("["):
							# Process flow style collections after the tag
							colors.merge(_parse_flow_style(color_provider, after_tag, after_tag_start))
						else:
							# Process normal scalar after the tag
							_add_scalar_color(color_provider, colors, after_tag, after_tag_start)
					return colors

			# If no tag, check for multiline indicator in full value
			var indicator_match: RegExMatch = re_patterns.multiline_indicator.search(value)
			if indicator_match:
				_add_color(color_provider, colors, value_start + indicator_match.get_start(1), value_start + indicator_match.get_end(1), TokenType.SYMBOL)
			elif value.begins_with("[") or value.begins_with("{"):
				colors.merge(_parse_flow_style(color_provider, value, value_start))
			else:
				_add_scalar_color(color_provider, colors, value, value_start)
	return colors

# Colors for scalar values
func _add_scalar_color(color_provider: ColorProvider, colors: Dictionary, token: String, start_index: int) -> void:
	# Handle empty or whitespace-only tokens
	token = token.strip_edges()
	if token.is_empty():
		return

	# Check for quoted strings first
	if re_patterns.quoted_string.search(token):
		_add_color(color_provider, colors, start_index, start_index + token.length(), TokenType.STRING)
		return  # Important: return early to prevent parsing tags inside strings

	# Check for tags
	var tag_match: RegExMatch = re_patterns.tag.search(token)
	if tag_match:
		var tag_start := tag_match.get_start(0)
		var tag_end := tag_match.get_end(0)

		# Only color the tag portion
		_add_color(color_provider, colors, start_index + tag_start, start_index + tag_end, TokenType.KEYWORD)

		# Process any remaining content after the tag
		if tag_end < token.length():
			var remaining := token.substr(tag_end).strip_edges()
			if remaining:
				var remaining_start = start_index + token.find(remaining, tag_end)
				if remaining_start != -1:
					if remaining.begins_with("{") or remaining.begins_with("["):
						colors.merge(_parse_flow_style(color_provider, remaining, remaining_start))
					else:
						# Apply appropriate coloring for the remaining content
						if re_patterns.number.search(remaining):
							_add_color(color_provider, colors, remaining_start, remaining_start + remaining.length(), TokenType.NUMBER)
						elif re_patterns.boolean.search(remaining) or re_patterns.nullish.search(remaining) or re_patterns.special.search(remaining):
							_add_color(color_provider, colors, remaining_start, remaining_start + remaining.length(), TokenType.KEYWORD)
						else:
							_add_color(color_provider, colors, remaining_start, remaining_start + remaining.length(), TokenType.STRING)
		return

	# Rest of the scalar checks for non-tag content
	elif re_patterns.number.search(token):
		_add_color(color_provider, colors, start_index, start_index + token.length(), TokenType.NUMBER)
	elif re_patterns.boolean.search(token) or re_patterns.nullish.search(token) or re_patterns.special.search(token):
		_add_color(color_provider, colors, start_index, start_index + token.length(), TokenType.KEYWORD)
	elif re_patterns.anchor.search(token) or re_patterns.alias.search(token):
		_add_color(color_provider, colors, start_index, start_index + token.length(), TokenType.SYMBOL)
	else:
		# Default fallback is string color
		_add_color(color_provider, colors, start_index, start_index + token.length(), TokenType.STRING)

# Add color for a type
func _add_color(color_provider: ColorProvider, colors: Dictionary, start: int, end: int, type: TokenType) -> void:
	colors[start] = {"color": color_provider.get_color_for_type(type)}

# Sort the colors dictionary by index
func _sort_colors(colors: Dictionary) -> Dictionary:
	# Get all indices as an array
	var indices := colors.keys()
	indices.sort()  # Sort indices in ascending order

	# Create new dictionary with sorted indices
	var sorted_colors := {}
	for idx in indices:
		sorted_colors[idx] = colors[idx]

	return sorted_colors
