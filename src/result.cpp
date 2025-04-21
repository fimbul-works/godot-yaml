#include "result.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void YAMLResult::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_data", "index"), &YAMLResult::get_data, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_document", "index"), &YAMLResult::get_document, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_document_count"), &YAMLResult::get_document_count);
	ClassDB::bind_method(D_METHOD("has_error"), &YAMLResult::has_error);
	ClassDB::bind_method(D_METHOD("get_error_message"), &YAMLResult::get_error_message);
	ClassDB::bind_method(D_METHOD("get_error_line"), &YAMLResult::get_error_line);
	ClassDB::bind_method(D_METHOD("get_error_column"), &YAMLResult::get_error_column);
	ClassDB::bind_method(D_METHOD("has_style"), &YAMLResult::has_style);
	ClassDB::bind_method(D_METHOD("get_style"), &YAMLResult::get_style);
	ClassDB::bind_static_method("YAMLResult", D_METHOD("error", "msg", "line", "column"), &YAMLResult::error, DEFVAL(-1), DEFVAL(-1));
}

Ref<YAMLResult> YAMLResult::success(const Variant &data, const Ref<YAMLStyle> &style) {
	return Ref<YAMLResult>(memnew(YAMLResult(data, style)));
}

Ref<YAMLResult> YAMLResult::error(const String &msg, int line, int column) {
	return Ref<YAMLResult>(memnew(YAMLResult(Variant(), nullptr, msg, line, column)));
}

Variant YAMLResult::get_data(int index) const {
	// If data is not an array or index is 0, return as before for backward compatibility
	if (data.get_type() != Variant::ARRAY || index == 0 && data.operator Array().size() == 0) {
		return data;
	}

	// Handle array of documents
	Array documents = data;
	if (index >= 0 && index < documents.size()) {
		return documents[index];
	}

	// Return null for out of range indices
	return Variant();
}

int YAMLResult::get_document_count() const {
	if (!error_message.is_empty()) {
		return 0;
	}

	// If data is an array of documents
	if (data.get_type() == Variant::ARRAY) {
		Array documents = data;
		return documents.size();
	}

	return 1;
}
