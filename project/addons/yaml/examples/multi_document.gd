extends Node2D
## Example showing multi-document parsing

var yaml_text := """
title: Document 1
---
title: Document 2
---
title: Document 3
"""

func _ready():
	if !visible:
		return
	print_rich("[b]📑 Multiple Document YAML Example[/b]")

	# Parse YAML string
	var parse_result := YAML.parse(yaml_text)
	assert(!parse_result.get_error_message(), parse_result.get_error_message())

	# Get number of documents
	var count := parse_result.get_document_count()
	assert(count == 3, "Result should have 3 documents")

	for i in range(count):
		var doc := parse_result.get_document(i) # or parse_result.get_data(i)
		print("Title (index %d): %s" % [i, doc.title])
