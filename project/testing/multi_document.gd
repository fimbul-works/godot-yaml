extends BaseTest

func _init() -> void:
	icon = "📑"

var yaml_text := """
title: Document 1
---
title: Document 2
---
title: Document 3
"""

func test_multi_document_parsing() -> void:
	var parse_result := YAML.parse(yaml_text)
	expect(!parse_result.has_error(), parse_result.get_error())

	var count := parse_result.get_document_count()
	expect_equal(count, 3, "Result should have 3 documents")
	expect_equal(parse_result.get_document(1).title, "Document 2", "Document titles should match")
	expect_equal(parse_result.get_document(4), null, "4th document should be null")
