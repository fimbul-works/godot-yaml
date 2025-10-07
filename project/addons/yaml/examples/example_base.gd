class_name ExampleBase extends Node2D

# Whether to show detailed logs
var LOG_VERBOSE := false

# Extra emoji to make logs visually distinct
var icon := ""

# Hook that runs all examples in the class
func _ready() -> void:
	if !visible:
		return

	print_rich("\n[b][font_size=16]%s%s[/font_size][/b]" % [
		"%s " % icon if icon.length() > 0 else "",
		name
	])

	run_examples()

# Override this to run examples in your class
func run_examples() -> void:
	# Child classes should override this method
	pass

# Logging Helpers
func log_header(text: String) -> void:
	print_rich("\n[b][font_size=16]%s[/font_size][/b]" % text)

func log_subheader(text: String) -> void:
	print_rich("\n[b][font_size=14]%s[/font_size][/b]" % text)

func log_success(text: Variant) -> void:
	print_rich("[color=green]✅ %s[/color]" % str(text))

func log_error(text: Variant) -> void:
	print_rich("[color=red]❌ %s[/color]" % str(text))

func log_warning(text: Variant) -> void:
	print_rich("[color=yellow]⚠️ %s[/color]" % str(text))

func log_info(text: Variant) -> void:
	print_rich("%s" % str(text))

func log_code_block(code: String) -> void:
	print_rich("\n[b]Code:[/b]")
	print_rich("[color=#aaaaff]%s" % code)

func log_result(text: Variant) -> void:
	print_rich("\n[b]Result:[/b]\n%s" % str(text))

# Run a single example with timing
func run_example(title: String, method: Callable) -> void:
	print_rich("\n[b][font_size=14]%s[/font_size][/b]" % title)
	var start_time := Time.get_ticks_usec()

	method.call()

	var elapsed := Time.get_ticks_usec() - start_time
	var t: float = elapsed
	var tl = "µsec"
	if t > 1000:
		t /= 1000.0
		tl = "ms"

	print_rich("\n[color=#888888]Completed in %.2f %s[/color]" % [t, tl])
