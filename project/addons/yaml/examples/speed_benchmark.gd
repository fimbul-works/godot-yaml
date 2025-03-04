extends Node2D

## Number of iterations for each benchmark
const ITERATIONS: int = 5

## Path to the YAML file to benchmark
const YAML_PATH: String = "res://addons/yaml/examples/comprehensive.yaml"

func _ready() -> void:
	if !visible:
		return
	print_rich("[b]⚡ YAML Speed Benchmark[/b]")
	print(YAML.version())

	# Load the YAML file
	var yaml_input: String = load_yaml_file()
	if yaml_input.is_empty():
		return

	# Run benchmarks
	print_rich("\n[b]Running benchmarks (%d iterations each):[/b]" % ITERATIONS)

	# Parse without style
	var parse_results := benchmark_operation(
		func(): return YAML.parse(yaml_input),
		"Parse (no style)"
	)

	# Parse with style
	var style_parse_results := benchmark_operation(
		func(): return YAML.parse(yaml_input, true),
		"Parse (with style)"
	)

	# Get data and style for stringify tests
	var data = parse_results[0][0].get_data() if parse_results else null
	var style = style_parse_results[0][0].get_style() if style_parse_results else null

	if !data:
		printerr("No data available for stringify tests")
		return

	# Stringify without style
	var stringify_results := benchmark_operation(
		func(): return YAML.stringify(data),
		"Stringify (no style)"
	)

	# Stringify with style
	if style:
		var style_stringify_results := benchmark_operation(
			func(): return YAML.stringify(data, style),
			"Stringify (with style)"
		)

		print_rich("\n[b]Average times:[/b]")
		print_stats("Parse (no style)", parse_results)
		print_stats("Parse (with style)", style_parse_results)
		print_stats("Stringify (no style)", stringify_results)
		print_stats("Stringify (with style)", style_stringify_results)

func load_yaml_file() -> String:
	var file := FileAccess.open(YAML_PATH, FileAccess.READ)
	if !file:
		printerr("Could not open file: ", YAML_PATH)
		return ""
	var content := file.get_as_text()
	# Replace tabs with spaces for consistent parsing
	return content.replace("\t", "    ")

## Benchmark a YAML operation
## Returns an array of YAMLResult objects and their execution times in microseconds
func benchmark_operation(operation: Callable, label: String) -> Array:
	var results: Array = []
	print_rich("\n[b]%s:[/b]" % label)

	for i in range(ITERATIONS):
		var start := Time.get_ticks_usec()
		var result = operation.call()
		var elapsed := Time.get_ticks_usec() - start

		if result.has_error():
			printerr("Iteration %d failed: %s" % [i + 1, result.get_error_message()])
			continue

		results.append([result, elapsed])
		print("Iteration %d: %d µs" % [i + 1, elapsed])

	return results

## Print statistics for a set of benchmark results
func print_stats(label: String, results: Array) -> void:
	if results.is_empty():
		print("%s: No valid results" % label)
		return

	var times := results.map(func(r): return r[1])
	var avg := float(times.reduce(func(a, b): return a + b)) / times.size()
	var min_time := times.min()
	var max_time := times.max()

	print("%s:" % label)
	print("  Average: %.2f µs" % avg)
	print("  Min: %d µs" % min_time)
	print("  Max: %d µs" % max_time)
