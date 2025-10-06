#include "syntax_validator.hpp"

#include <ryml.hpp>

#include <memory>

using namespace godot;

YAML::SyntaxValidator::SyntaxValidator() {
	callbacks.m_error = error_callback;
	callbacks.m_user_data = this;
	evt_handler = std::make_unique<ryml::EventHandlerTree>(callbacks);
	ryml_parser = std::make_unique<ryml::Parser>(evt_handler.get(), ryml::ParserOptions().locations(true));
}

Ref<YAMLResult> YAML::SyntaxValidator::validate(const String &input) {
	try {
		current_result = YAMLResult::success(Variant());

		ryml::parse_in_arena(ryml_parser.get(), input.utf8().get_data(), &tree);

		return current_result;
	} catch (const YAMLException &e) {
		return YAMLResult::error(e.what(), e.get_line(), e.get_column());
	} catch (const std::exception &e) {
		return YAMLResult::error(e.what());
	} catch (...) {
		return YAMLResult::error("Unexpected error during validation");
	}
}

void YAML::SyntaxValidator::error_callback(const char *msg, size_t len, ryml::Location loc, void *user_data) {
	ryml::csubstr error_msg(msg, len);

	// Strip "ERROR: " prefix if present
	const ryml::csubstr strip_error_prefix = "ERROR: ";
	if (error_msg.begins_with(strip_error_prefix)) {
		error_msg = error_msg.sub(strip_error_prefix.len);
	}

	// Only return the first line, which has the relevent error message
	size_t newline_pos = error_msg.find('\n');
	if (newline_pos != ryml::substr::npos) {
		error_msg = error_msg.sub(0, newline_pos);
	}

	auto *validator = static_cast<SyntaxValidator *>(user_data);
	if (!validator) {
		throw YAMLException(from_ryml_str(error_msg));
	}

	validator->current_result = YAMLResult::error(from_ryml_str(error_msg), loc.line, loc.col);

	throw YAMLException(validator->current_result->get_error_message(), loc);
}
