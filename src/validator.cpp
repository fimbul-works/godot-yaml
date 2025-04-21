#include "validator.h"
#include "result.h"
#include <memory>
#include <ryml.hpp>

using namespace godot;

YAML::Validator::Validator() {
	// Setup ryml callbacks
	callbacks.m_error = error_callback;
	callbacks.m_user_data = this;

	// Initialize ryml components
	evt_handler = std::make_unique<ryml::EventHandlerTree>(callbacks);
	ryml_parser = std::make_unique<ryml::Parser>(evt_handler.get(), ryml::ParserOptions().locations(true));
}

Ref<YAMLResult> YAML::Validator::validate(const String &input) {
	try {
		current_result = YAMLResult::success(Variant());
		tree.clear();

		ryml::parse_in_arena(ryml_parser.get(), input.utf8().get_data(), &tree);

		return current_result;
	} catch (const YAMLException &e) {
		return current_result;
	} catch (const std::exception &e) {
		return YAMLResult::error(e.what());
	} catch (...) {
		return YAMLResult::error("Unknown error occurred during validation");
	}
}

void YAML::Validator::error_callback(const char *msg, size_t len, ryml::Location loc, void *user_data) {
	ryml::csubstr error_msg(msg, len);

	// Strip "ERROR: " prefix if present
	const ryml::csubstr strip_error_prefix = "ERROR: ";
	if (error_msg.begins_with(strip_error_prefix)) {
		error_msg = error_msg.sub(strip_error_prefix.len);
	}

	// RapidYAML does not like complex keys
	if (error_msg.begins_with("ryml trees cannot handle containers as keys")) {
		error_msg = ryml::to_csubstr("unsupported complex key");
	}

	// Only return the first line, which has the relevent error message
	size_t newline_pos = error_msg.find('\n');
	if (newline_pos != ryml::substr::npos) {
		error_msg = error_msg.sub(0, newline_pos);
	}

	auto *validator = static_cast<Validator *>(user_data);
	if (!validator) {
		throw YAMLException(from_ryml_str(error_msg));
	}

	validator->current_result = YAMLResult::error(
			from_ryml_str(error_msg),
			loc.line,
			loc.col);

	throw YAMLException(validator->current_result->get_error_message());
}
