#include "../yaml.hpp"
#include "SFT.hpp"
#include "test_runner.hpp"

#ifdef TESTS_ENABLED

namespace YAMLTests {

void test_version_info() {
	// Test YAML.version() returns correct version string
	String version = godot::YAML::version();

	NAMED_TESTS(
			"YAML Version Tests",
			"Version string not empty", !version.is_empty(),
			"Version contains version number", version.contains("."),
			"Version format check", version.length() >= 5)
}

void test_basic_parsing() {
	// Test basic YAML parsing functionality
	String yaml_input = "key: value\nnumber: 42";
	auto result = godot::YAML::parse(yaml_input);

	NAMED_TESTS(
			"Basic YAML Parsing",
			"Parse successful", !result->has_error(),
			"Data is dictionary", result->get_data().get_type() == godot::Variant::DICTIONARY)

	if (!result->has_error()) {
		godot::Dictionary data = result->get_data();
		NAMED_TESTS(
				"Parsed Data Validation",
				"Key exists", data.has("key"),
				"Key value correct", VAR_CHECK(data["key"], "value"),
				"Number parsed correctly", VAR_CHECK(data["number"], 42))
	}
}

void test_basic_error_handling() {
	// Test error handling with malformed YAML
	String invalid_yaml = "key: value\n\t\tinvalid indentation";
	auto result = godot::YAML::parse(invalid_yaml);

	NAMED_TESTS(
			"Basic Error Handling",
			"Parse detects error", result->has_error(),
			"Error message not empty", !result->get_error().is_empty(),
			"Error line reported", result->get_error_line() > 0)
}

void test_basic_stringify() {
	// Test basic stringify functionality
	godot::Dictionary test_data;
	test_data["string"] = "value";
	test_data["number"] = 123;
	test_data["boolean"] = true;

	auto result = godot::YAML::stringify(test_data);

	NAMED_TESTS(
			"Basic Stringify",
			"Stringify successful", !result->has_error(),
			"Result is string", result->get_data().get_type() == godot::Variant::STRING,
			"Result contains key", godot::String(result->get_data()).contains("string"))
}

void test_basic_round_trip() {
	// Test basic round-trip: parse -> stringify -> parse
	String original_yaml = "name: test\nvalue: 42\nactive: true";

	// Parse original
	auto parse_result = godot::YAML::parse(original_yaml);
	NAMED_TESTS(
			"Round Trip Parse",
			"Original parse successful", !parse_result->has_error())

	if (parse_result->has_error()) {
		return;
	}

	// Stringify parsed data
	auto stringify_result = godot::YAML::stringify(parse_result->get_data());
	NAMED_TESTS(
			"Round Trip Stringify",
			"Stringify successful", !stringify_result->has_error())

	if (stringify_result->has_error()) {
		return;
	}

	// Parse stringified data
	auto reparse_result = godot::YAML::parse(stringify_result->get_data());
	NAMED_TESTS(
			"Round Trip Reparse",
			"Reparse successful", !reparse_result->has_error())

	if (!reparse_result->has_error()) {
		godot::Dictionary original_data = parse_result->get_data();
		godot::Dictionary reparsed_data = reparse_result->get_data();

		NAMED_TESTS(
				"Round Trip Data Integrity",
				"Name values match", VAR_CHECK(reparsed_data["name"], original_data["name"]),
				"Value numbers match", VAR_CHECK(reparsed_data["value"], original_data["value"]),
				"Boolean values match", VAR_CHECK(reparsed_data["active"], original_data["active"]))
	}
}

// Main runner for basic YAML tests
void run_basic_yaml_tests() {
	godot::UtilityFunctions::print("🔹 Running Basic YAML Tests...\n");

	test_version_info();
	test_basic_parsing();
	test_basic_error_handling();
	test_basic_stringify();
	test_basic_round_trip();

	godot::UtilityFunctions::print("✅ Basic YAML Tests Complete\n");
}

} // namespace YAMLTests

#endif // TESTS_ENABLED
