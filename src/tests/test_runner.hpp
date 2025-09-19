#pragma once

#ifdef TESTS_ENABLED
namespace YAMLTests {

// Master test coordinator
void run_all_tests();

// Individual test suite runners
void run_basic_yaml_tests(); // Basic parsing, stringify, version
void run_variant_tests(); // Variant round-trip tests
// void run_error_handling_tests(); // Error conditions and malformed YAML
// void run_style_tests(); // Style system and formatting
// void run_security_tests(); // Security restrictions and resource loading
// void run_multi_document_tests(); // Multi-document YAML parsing
// void run_custom_class_tests(); // Custom class serialization
// void run_file_operations_tests(); // File I/O operations
// void run_performance_tests(); // Performance and stress testing

} // namespace YAMLTests
#endif
