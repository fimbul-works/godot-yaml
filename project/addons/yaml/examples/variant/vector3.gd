extends BaseTest
## Test suite for Vector3 YAML serialization and styling

# Test values covering different vector configurations
var test_values = {
	"Zero": Vector3.ZERO,
	"One": Vector3.ONE,
	"Unit_x": Vector3.RIGHT,
	"Unit_y": Vector3.UP,
	"Unit_z": Vector3.BACK,
	"Simple": Vector3(10, 20, 30),
	"Negative": Vector3(-30, -40, -50),
	"Mixed": Vector3(50, -60, 70),
	"Decimal": Vector3(1.5, 2.75, 3.125),
	"Large": Vector3(1000, 2000, 3000),
	"Very_small": Vector3(0.0001, 0.0002, 0.0003),
	"Normalized": Vector3(3, 4, 5).normalized(),
}

func _init():
	icon = "3️⃣"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	for name in test_values:
		var vec = test_values[name]
		var result = YAML.stringify(vec)

		expect(!result.has_error(), "Stringify %s: %s" % [name, result.get_error_message()])
		if result.has_error():
			continue

		if LOG_VERBOSE:
			print_rich("  %s: %s" % [name, result.get_data()])

		# Parse back and check
		var parse_result = YAML.parse(result.get_data())
		expect(!parse_result.has_error(), "Parse %s: %s" % [name, parse_result.get_error_message()])

		if !parse_result.has_error():
			var parsed_vec = parse_result.get_data()
			expect(vec.is_equal_approx(parsed_vec),
				"Roundtrip conversion for %s failed: %s != %s" % [name, vec, parsed_vec])

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var vec = Vector3(10, 20, 30)

	# Test map form
	var map_style = YAML.create_style()
	map_style.set_container_form(YAMLStyle.FORM_DICTIONARY)

	var map_result = YAML.stringify(vec, map_style)
	expect(!map_result.has_error(), "Map form stringify error: " + map_result.get_error_message())

	if LOG_VERBOSE && !map_result.has_error():
		print_rich("\n• Dictionary form:")
		print_rich(map_result.get_data())

	# Test sequence form
	var array_style = YAML.create_style()
	array_style.set_container_form(YAMLStyle.FORM_ARRAY)

	var array_result = YAML.stringify(vec, array_style)
	expect(!array_result.has_error(), "Sequence form stringify error: " + array_result.get_error_message())

	if LOG_VERBOSE && !array_result.has_error():
		print_rich("\n• Array form:")
		print_rich(array_result.get_data())

	# Test roundtrip for both forms
	if !map_result.has_error():
		var map_parse = YAML.parse(map_result.get_data())
		expect(!map_parse.has_error(), "Map parse error: " + map_parse.get_error_message())
		if !map_parse.has_error():
			expect(vec.is_equal_approx(map_parse.get_data()),
				"Map roundtrip failed: %s != %s" % [vec, map_parse.get_data()])

	if !array_result.has_error():
		var array_parse = YAML.parse(array_result.get_data())
		expect(!array_parse.has_error(), "Sequence parse error: " + array_parse.get_error_message())
		if !array_parse.has_error():
			expect(vec.is_equal_approx(array_parse.get_data()),
				"Sequence roundtrip failed: %s != %s" % [vec, array_parse.get_data()])

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var vec = Vector3(10, 20, 30)

	# Test block style
	var block_style = YAML.create_style()
	block_style.set_flow_style(YAMLStyle.FLOW_NONE)

	var block_result = YAML.stringify(vec, block_style)
	expect(!block_result.has_error(), "Block style stringify error: " + block_result.get_error_message())

	if LOG_VERBOSE && !block_result.has_error():
		print_rich("\n• Block style:")
		print_rich(block_result.get_data())

	# Test flow style
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	var flow_result = YAML.stringify(vec, flow_style)
	expect(!flow_result.has_error(), "Flow style stringify error: " + flow_result.get_error_message())

	if LOG_VERBOSE && !flow_result.has_error():
		print_rich("\n• Flow style:")
		print_rich(flow_result.get_data())

	# Test roundtrip for both styles
	if !block_result.has_error():
		var block_parse = YAML.parse(block_result.get_data())
		expect(!block_parse.has_error(), "Block parse error: " + block_parse.get_error_message())
		if !block_parse.has_error():
			expect(vec.is_equal_approx(block_parse.get_data()),
				"Block roundtrip failed: %s != %s" % [vec, block_parse.get_data()])

	if !flow_result.has_error():
		var flow_parse = YAML.parse(flow_result.get_data())
		expect(!flow_parse.has_error(), "Flow parse error: " + flow_parse.get_error_message())
		if !flow_parse.has_error():
			expect(vec.is_equal_approx(flow_parse.get_data()),
				"Flow roundtrip failed: %s != %s" % [vec, flow_parse.get_data()])

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var vec = Vector3(10, 20, 30)

	# First serialize with a specific style
	var style = YAML.create_style()
	style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	style.create_child("x").set_float_format(YAMLStyle.FLOAT_SCIENTIFIC)

	var result = YAML.stringify(vec, style)
	expect(!result.has_error(), "Style stringify error: " + result.get_error_message())

	if result.has_error():
		return

	if LOG_VERBOSE:
		print_rich("\n• Styled Vector3:")
		print_rich(result.get_data())

	# Parse with style detection
	var parse_result = YAML.parse(result.get_data(), null, true) # true for style detection
	expect(!parse_result.has_error(), "Parse with style detection error: " + parse_result.get_error_message())

	if parse_result.has_error():
		return

	# Verify style was detected
	expect(parse_result.has_style(), "Style should be detected")

	# Modify the vector and re-stringify with detected style
	var parsed_vec = parse_result.get_data()
	var modified_vec = parsed_vec * 2

	var restringify_result = YAML.stringify(modified_vec, parse_result.get_style())
	expect(!restringify_result.has_error(), "Restringify error: " + restringify_result.get_error_message())

	if LOG_VERBOSE && !restringify_result.has_error():
		print_rich("\n• Restringified with detected style:")
		print_rich(restringify_result.get_data())

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test Vector3-specific parsing errors
	var invalid_vector3_yaml = """
!Vector3
x: 1.0
y: 2.0
# Missing z field
"""
	var missing_field_result = YAML.parse(invalid_vector3_yaml)
	expect(missing_field_result.has_error(), "Should error on missing required field")
	if LOG_VERBOSE && missing_field_result.has_error():
		print_rich("\n• Error on missing field: %s" % missing_field_result.get_error_message())

	var wrong_type_yaml = """
!Vector3 "not a vector3"
"""
	var wrong_type_result = YAML.parse(wrong_type_yaml)
	expect(wrong_type_result.has_error(), "Should error on wrong type/format")
	if LOG_VERBOSE && wrong_type_result.has_error():
		print_rich("\n• Error on wrong type: %s" % wrong_type_result.get_error_message())

	var invalid_sequence_length = """
!Vector3
- 1.0
- 2.0
"""
	var seq_length_result = YAML.parse(invalid_sequence_length)
	expect(seq_length_result.has_error(), "Should error on invalid sequence length")
	if LOG_VERBOSE && seq_length_result.has_error():
		print_rich("\n• Error on invalid sequence length: %s" % seq_length_result.get_error_message())

## Test special values (INF, NAN)
func test_special_values() -> void:
	var special_values = {
		"inf": Vector3(INF, INF, INF),
		"neg_inf": Vector3(-INF, -INF, -INF),
		"mixed_inf": Vector3(INF, -INF, INF)
	}

	for name in special_values:
		var vec = special_values[name]
		var result = YAML.stringify(vec)

		expect(!result.has_error(), "Special value stringify: " + name)
		if result.has_error():
			continue

		if LOG_VERBOSE:
			print_rich("\n• %s: %s" % [name, result.get_data()])

		# Parse back and check
		var parse_result = YAML.parse(result.get_data())
		expect(!parse_result.has_error(), "Special value parse: " + name)

		if !parse_result.has_error():
			var parsed_vec = parse_result.get_data()

			# Check INF values are preserved
			if name == "inf":
				expect(is_inf(parsed_vec.x) && parsed_vec.x > 0, "Positive infinity preserved in x")
				expect(is_inf(parsed_vec.y) && parsed_vec.y > 0, "Positive infinity preserved in y")
				expect(is_inf(parsed_vec.z) && parsed_vec.z > 0, "Positive infinity preserved in z")
			elif name == "neg_inf":
				expect(is_inf(parsed_vec.x) && parsed_vec.x < 0, "Negative infinity preserved in x")
				expect(is_inf(parsed_vec.y) && parsed_vec.y < 0, "Negative infinity preserved in y")
				expect(is_inf(parsed_vec.z) && parsed_vec.z < 0, "Negative infinity preserved in z")
			elif name == "mixed_inf":
				expect(is_inf(parsed_vec.x) && parsed_vec.x > 0, "Positive infinity preserved in x")
				expect(is_inf(parsed_vec.y) && parsed_vec.y < 0, "Negative infinity preserved in y")
				expect(is_inf(parsed_vec.z) && parsed_vec.z > 0, "Positive infinity preserved in z")
