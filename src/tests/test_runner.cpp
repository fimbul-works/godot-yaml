#include "test_runner.hpp"
#include "../yaml.hpp"
#include "SFT.hpp"

#ifdef TESTS_ENABLED

namespace YAMLTests {

// Master test runner that coordinates all test suites
void run_all_tests() {
	godot::UtilityFunctions::print("🧪 Running Complete YAML Extension Test Suite...\n");
	godot::UtilityFunctions::print("============================================================\n");

	// Basic functionality tests
	run_basic_yaml_tests();
	godot::UtilityFunctions::print("");

	// Variant round-trip tests (future)
	// run_variant_tests();
	// godot::UtilityFunctions::print("");

	// Error handling tests (future)
	// run_error_handling_tests();
	// godot::UtilityFunctions::print("");

	// Style system tests (future)
	// run_style_tests();
	// godot::UtilityFunctions::print("");

	// Security tests (future)
	// run_security_tests();
	// godot::UtilityFunctions::print("");

	// Multi-document tests (future)
	// run_multi_document_tests();
	// godot::UtilityFunctions::print("");

	// Custom class serialization tests (future)
	// run_custom_class_tests();
	// godot::UtilityFunctions::print("");

	// File operations tests (future)
	// run_file_operations_tests();
	// godot::UtilityFunctions::print("");

	// Performance/stress tests (future)
	// run_performance_tests();

	godot::UtilityFunctions::print("============================================================");
	godot::UtilityFunctions::print("✅ Complete YAML Extension Test Suite Finished\n");
}

} // namespace YAMLTests

#endif // TESTS_ENABLED
