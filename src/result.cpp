#include "result.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void YAMLResult::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_data"), &YAMLResult::get_data);
	ClassDB::bind_method(D_METHOD("get_document", "index"), &YAMLResult::get_document, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_document_count"), &YAMLResult::get_document_count);
	ClassDB::bind_method(D_METHOD("get_documents"), &YAMLResult::get_documents);
	ClassDB::bind_method(D_METHOD("has_multiple_documents"), &YAMLResult::has_multiple_documents);

	ClassDB::bind_method(D_METHOD("has_error"), &YAMLResult::has_error);
	ClassDB::bind_method(D_METHOD("get_error_message"), &YAMLResult::get_error_message);
	ClassDB::bind_method(D_METHOD("get_error_line"), &YAMLResult::get_error_line);
	ClassDB::bind_method(D_METHOD("get_error_column"), &YAMLResult::get_error_column);
	ClassDB::bind_method(D_METHOD("get_error"), &YAMLResult::get_error);

	ClassDB::bind_method(D_METHOD("has_style"), &YAMLResult::has_style);
	ClassDB::bind_method(D_METHOD("get_style"), &YAMLResult::get_style);

	ClassDB::bind_static_method("YAMLResult", D_METHOD("error", "msg"), &YAMLResult::user_error);

	BIND_VIRTUAL_METHOD(YAMLResult, _to_string);
}

Ref<YAMLResult> YAMLResult::success(const Variant &data, const Ref<YAMLStyle> &style) {
	return Ref<YAMLResult>(memnew(YAMLResult(data, false, style)));
}

Ref<YAMLResult> YAMLResult::multi_document_success(const Array &documents) {
	return Ref<YAMLResult>(memnew(YAMLResult(documents, true, nullptr)));
}

Ref<YAMLResult> YAMLResult::error(const String &msg, int line, int column) {
	return Ref<YAMLResult>(memnew(YAMLResult(Variant(), false, nullptr, msg, line, column)));
}

Ref<YAMLResult> YAMLResult::user_error(const String &msg) {
	return Ref<YAMLResult>(memnew(YAMLResult(Variant(), false, nullptr, msg)));
}

Variant YAMLResult::get_data() const {
	if (has_error()) {
		return Variant();
	}

	if (is_multi_document) {
		UtilityFunctions::push_warning("YAMLResult.get_data() called on multi-document YAML. Use get_document() instead.");

		Array documents = data.operator Array();
		return documents.size() > 0 ? documents[0] : Variant();
	}

	return data;
}

Variant YAMLResult::get_document(int index) const {
	if (has_error()) {
		return Variant();
	}

	// Single document: return data if index is 0, null otherwise
	if (!is_multi_document) {
		return index == 0 ? data : Variant();
	}

	// Multi-document: access from documents array
	Array documents = data.operator Array();
	if (index >= 0 && index < documents.size()) {
		return documents[index];
	}

	// Out of range
	return Variant();
}

int YAMLResult::get_document_count() const {
	if (has_error()) {
		return 0;
	}

	if (is_multi_document) {
		Array documents = data.operator Array();
		return documents.size();
	}

	return 1; // Single document
}

Array YAMLResult::get_documents() const {
	if (has_error()) {
		return Array();
	}

	// Multi-document: return the documents array directly
	if (is_multi_document) {
		return data.operator Array();
	}

	// Single document: wrap in array for consistency
	Array single_doc_array;
	single_doc_array.push_back(data);
	return single_doc_array;
}

bool YAMLResult::has_multiple_documents() const {
	return !has_error() && is_multi_document;
}

bool YAMLResult::has_error() const {
	return !error_message.is_empty();
}

String YAMLResult::get_error_message() const {
	return error_message;
}

int YAMLResult::get_error_line() const {
	return error_line;
}

int YAMLResult::get_error_column() const {
	return error_column;
}

String YAMLResult::get_error() const {
	if (error_line >= 0 && error_column >= 0) {
		return vformat("%s (line %d, column %d)", error_message, error_line, error_column);
	}
	return error_message;
}

bool YAMLResult::has_style() const {
	return style.is_valid();
}

Ref<YAMLStyle> YAMLResult::get_style() const {
	return style;
}

String YAMLResult::_to_string() const {
	if (has_error()) {
		return vformat("YAMLResult(Error: %s)", get_error_message());
	}

	return vformat("YAMLResult(%s, %d document%s)",
			"Success",
			get_document_count(),
			get_document_count() == 1 ? "" : "s");
}
