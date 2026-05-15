extends ExampleBase

## Number of iterations for each benchmark
const ITERATIONS: int = 10

## Thread counts for multithreaded benchmarks
const THREAD_COUNTS: Array = [1, 2, 4, 8, 16, 32, 64, 100]

## Path to the YAML file to benchmark
const YAML_PATH: String = "res://addons/yaml/examples/data/supported_syntax.yaml"

func _init() -> void:
	icon = "⚡"

func run_examples() -> void:
	log_header("YAML Speed Benchmark")
	print(YAML.version())

	run_example("Load YAML File", load_yaml_file_benchmark)
	run_example("Parse (No Style) Benchmark", parse_benchmark)
	run_example("Parse (With Style) Benchmark", parse_with_style_benchmark)
	run_example("Stringify (No Style) Benchmark", stringify_benchmark)
	run_example("Stringify (With Style) Benchmark", stringify_with_style_benchmark)
	run_example("Compare Results", compare_results)
	run_example("Parse (Threaded) Bechmark", parse_threaded)
	run_example("Stringify (Threaded) Benchmark", stringify_threaded)

var yaml_input: String
var parse_times := []
var style_parse_times := []
var stringify_times := []
var style_stringify_times := []
var data = null
var style = null

func load_yaml_file_benchmark() -> void:
	log_info("Loading YAML file: " + YAML_PATH)

	var file := FileAccess.open(YAML_PATH, FileAccess.READ)
	if !file:
		log_error("Could not open file: " + YAML_PATH)
		return

	yaml_input = file.get_as_text()
	log_success("File loaded, size: " + str(yaml_input.length()) + " characters")

	var result := YAML.parse(yaml_input)
	data = result.get_data()

	if LOG_VERBOSE:
		log_result("First 200 characters:\n" + yaml_input.substr(0, 200) + "...")

func parse_benchmark() -> void:
	if yaml_input.is_empty():
		log_error("No YAML input loaded")
		return

	log_info("Running parse benchmark (" + str(ITERATIONS) + " iterations)...")

	for i in range(ITERATIONS):
		var start := Time.get_ticks_usec()
		var result = YAML.parse(yaml_input)
		var elapsed := Time.get_ticks_usec() - start

		if result.has_error():
			log_error("Iteration " + str(i + 1) + " failed: " + result.get_error())
			continue

		parse_times.append(elapsed)
		log_info("Iteration " + str(i + 1) + ": " + str(elapsed) + " µs")

	if parse_times.size() > 0:
		var avg = float(parse_times.reduce(func(a, b): return a + b)) / parse_times.size()
		log_success("Average parse time: " + str(avg) + " µs")
	else:
		log_error("No successful parse iterations")

func parse_with_style_benchmark() -> void:
	if yaml_input.is_empty():
		log_error("No YAML input loaded")
		return

	log_info("Running parse with style benchmark (" + str(ITERATIONS) + " iterations)...")

	for i in range(ITERATIONS):
		var start := Time.get_ticks_usec()
		var result = YAML.parse(yaml_input, YAML.create_security(), true)
		var elapsed := Time.get_ticks_usec() - start

		if result.has_error():
			log_error("Iteration " + str(i + 1) + " failed: " + result.get_error())
			continue

		if i == 0:
			style = result.get_style()

		style_parse_times.append(elapsed)
		log_info("Iteration " + str(i + 1) + ": " + str(elapsed) + " µs")

	if style_parse_times.size() > 0:
		var avg = float(style_parse_times.reduce(func(a, b): return a + b)) / style_parse_times.size()
		log_success("Average parse with style time: " + str(avg) + " µs")
	else:
		log_error("No successful parse with style iterations")

func stringify_benchmark() -> void:
	if data == null:
		log_error("No data available for stringify tests")
		return

	log_info("Running stringify benchmark (" + str(ITERATIONS) + " iterations)...")

	for i in range(ITERATIONS):
		var start := Time.get_ticks_usec()
		var result = YAML.stringify(data)
		var elapsed := Time.get_ticks_usec() - start

		if result.has_error():
			log_error("Iteration " + str(i + 1) + " failed: " + result.get_error())
			continue

		stringify_times.append(elapsed)
		log_info("Iteration " + str(i + 1) + ": " + str(elapsed) + " µs")

	if stringify_times.size() > 0:
		var avg = float(stringify_times.reduce(func(a, b): return a + b)) / stringify_times.size()
		log_success("Average stringify time: " + str(avg) + " µs")
	else:
		log_error("No successful stringify iterations")

func stringify_with_style_benchmark() -> void:
	if data == null or style == null:
		log_error("No data or style available for stringify tests")
		return

	log_info("Running stringify with style benchmark (" + str(ITERATIONS) + " iterations)...")

	for i in range(ITERATIONS):
		var start := Time.get_ticks_usec()
		var result = YAML.stringify(data, style)
		var elapsed := Time.get_ticks_usec() - start

		if result.has_error():
			log_error("Iteration " + str(i + 1) + " failed: " + result.get_error())
			continue

		style_stringify_times.append(elapsed)
		log_info("Iteration " + str(i + 1) + ": " + str(elapsed) + " µs")

	if style_stringify_times.size() > 0:
		var avg = float(style_stringify_times.reduce(func(a, b): return a + b)) / style_stringify_times.size()
		log_success("Average stringify with style time: " + str(avg) + " µs")
	else:
		log_error("No successful stringify with style iterations")

func compare_results() -> void:
	log_subheader("Performance Comparison")

	# Collect all test results
	var all_tests = [
		{
			"name": "Parse (no style)",
			"times": parse_times
		},
		{
			"name": "Parse (with style)",
			"times": style_parse_times
		},
		{
			"name": "Stringify (no style)",
			"times": stringify_times
		},
		{
			"name": "Stringify (with style)",
			"times": style_stringify_times
		}
	]

	# Calculate and print stats
	for test in all_tests:
		var times = test.times
		if times.is_empty():
			log_warning(test.name + ": No valid results")
			continue

		var avg: float = float(times.reduce(func(a, b): return a + b)) / times.size()
		var min_time: int = times.min()
		var max_time: int = times.max()

		log_info(test.name + ":")
		log_info("  Average: %.2f µs" % avg)
		log_info("  Min: %d µs" % min_time)
		log_info("  Max: %d µs" % max_time)

	log_info("\nPerformance Insights:")

	# Compare parse with and without style
	if !parse_times.is_empty() and !style_parse_times.is_empty():
		var avg_parse = float(parse_times.reduce(func(a, b): return a + b)) / parse_times.size()
		var avg_style_parse = float(style_parse_times.reduce(func(a, b): return a + b)) / style_parse_times.size()

		var style_overhead = ((avg_style_parse / avg_parse) - 1.0) * 100.0
		log_info("Style detection adds approximately %.1f%% overhead to parsing" % style_overhead)

	# Compare stringify with and without style
	if !stringify_times.is_empty() and !style_stringify_times.is_empty():
		var avg_stringify = float(stringify_times.reduce(func(a, b): return a + b)) / stringify_times.size()
		var avg_style_stringify = float(style_stringify_times.reduce(func(a, b): return a + b)) / style_stringify_times.size()

		var style_overhead = ((avg_style_stringify / avg_stringify) - 1.0) * 100.0
		log_info("Using style adds approximately %.1f%% overhead to stringify" % style_overhead)

func parse_threaded() -> void:
	if yaml_input.is_empty():
		log_error("No YAML input loaded")
		return

	log_subheader("Threaded Parse Benchmark")

	for thread_count in THREAD_COUNTS:
		var threads: Array[Thread] = []

		var start := Time.get_ticks_usec()

		for i in range(thread_count):
			var thread := Thread.new()
			thread.start(_parse_thread_func.bind(yaml_input))
			threads.append(thread)

		var total_parse_time := 0
		for thread in threads:
			total_parse_time += thread.wait_to_finish()

		var wall_time := Time.get_ticks_usec() - start
		var avg_thread_time := float(total_parse_time / thread_count)

		log_info("%d thread(s): wall=%d µs | avg thread=%d µs | throughput=%.1f parses/ms" % [
			thread_count,
			wall_time,
			avg_thread_time,
			thread_count / (wall_time / 1000.0)
		])

func stringify_threaded() -> void:
	if data == null:
		log_error("No data available for stringify tests")
		return

	log_subheader("Threaded Stringify Benchmark")

	for thread_count in THREAD_COUNTS:
		var threads: Array[Thread] = []

		var start := Time.get_ticks_usec()

		for i in range(thread_count):
			var thread := Thread.new()
			thread.start(_stringify_thread_func.bind(data))
			threads.append(thread)

		var total_stringify_time := 0
		for thread in threads:
			total_stringify_time += thread.wait_to_finish()

		var wall_time := Time.get_ticks_usec() - start
		var avg_thread_time := float(total_stringify_time / thread_count)

		log_info("%d thread(s): wall=%d µs | avg thread=%d µs | throughput=%.1f stringifies/ms" % [
			thread_count,
			wall_time,
			avg_thread_time,
			thread_count / (wall_time / 1000.0)
		])

static func _parse_thread_func(input: String) -> int:
	var start := Time.get_ticks_usec()
	YAML.parse(input)
	return Time.get_ticks_usec() - start

static func _stringify_thread_func(input_data: Variant) -> int:
	var start := Time.get_ticks_usec()
	YAML.stringify(input_data)
	return Time.get_ticks_usec() - start
