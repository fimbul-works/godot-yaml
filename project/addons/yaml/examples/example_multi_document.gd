extends ExampleBase

const MULTI_DOC_FILE = "res://addons/yaml/examples/data/multi_document.yaml"
const OUTPUT_FILE = "user://multi_document_copy.yaml"

var multi_doc_yaml := """
# Configuration Document
name: MyApplication
version: 1.2.3
environment: production
---
# Database Settings
database:
  host: localhost
  port: 5432
  name: myapp_db
  credentials:
	username: admin
	password: secret123
---
# Feature Flags
features:
  enable_new_ui: true
  enable_analytics: false
  enable_caching: true
  max_connections: 100
---
# Logging Configuration
logging:
  level: INFO
  handlers:
	- console
	- file
  file_path: /var/log/myapp.log
""".replace("	", "     ") # Handle Godot's tab indentation

var parsed_documents

func _init() -> void:
	icon = "📑"

func run_examples() -> void:
	run_example("Validate Multi-Document YAML", validate_multi_document)
	run_example("Parse Multi-Document YAML", parse_multi_document)
	run_example("Access Individual Documents", access_individual_documents)
	run_example("Work with Document Count", work_with_document_count)
	run_example("Process All Documents", process_all_documents)
	run_example("Create Multi-Document YAML", create_multi_document)
	run_example("Save Multi-Document File", save_multi_document_file)
	run_example("Load Multi-Document File", load_multi_document_file)

func validate_multi_document() -> void:
	log_info("Validating multi-document YAML string...")
	var result := YAML.validate_syntax(multi_doc_yaml)

	if result.has_error():
		log_error("Validation failed: " + result.get_error())
		return

	log_success("Multi-document YAML is valid!")

func parse_multi_document() -> void:
	log_info("Parsing multi-document YAML...")
	var result := YAML.parse(multi_doc_yaml)

	if result.has_error():
		log_error("Parse failed: " + result.get_error())
		return

	parsed_documents = result
	log_success("Multi-document YAML parsed successfully")

	if LOG_VERBOSE:
		log_info("Has multiple documents: %s" % result.has_multiple_documents())
		log_info("Found " + str(result.get_document_count()) + " documents")

func access_individual_documents() -> void:
	# Ensure we have parsed documents
	if parsed_documents == null:
		parsed_documents = YAML.parse(multi_doc_yaml)

	log_info("Accessing individual documents...")

	# Access first document (configuration)
	var config_doc = parsed_documents.get_document(0)
	if config_doc != null:
		log_success("Document 0 - Configuration:")
		log_info("• App name: " + str(config_doc.name))
		log_info("• Version: " + str(config_doc.version))
		log_info("• Environment: " + str(config_doc.environment))

	# Access second document (database settings)
	var db_doc = parsed_documents.get_document(1)
	if db_doc != null:
		log_success("Document 1 - Database Settings:")
		log_info("• Host: " + str(db_doc.database.host))
		log_info("• Port: " + str(db_doc.database.port))
		log_info("• Database: " + str(db_doc.database.name))

	# Access third document (feature flags)
	var features_doc = parsed_documents.get_document(2)
	if features_doc != null:
		log_success("Document 2 - Feature Flags:")
		log_info("• New UI enabled: " + str(features_doc.features.enable_new_ui))
		log_info("• Analytics enabled: " + str(features_doc.features.enable_analytics))
		log_info("• Max connections: " + str(features_doc.features.max_connections))

	# Try to access non-existent document
	var non_existent = parsed_documents.get_document(10)
	if non_existent == null:
		log_info("Document 10 (non-existent): null")

func work_with_document_count() -> void:
	if parsed_documents == null:
		parsed_documents = YAML.parse(multi_doc_yaml)

	var count = parsed_documents.get_document_count()
	log_info("Total document count: " + str(count))

	log_info("Iterating through all documents by index:")
	for i in range(count):
		var doc = parsed_documents.get_document(i)
		var doc_type = "Unknown"

		# Identify document type by its content
		if doc.has("name") and doc.has("version"):
			doc_type = "Configuration"
		elif doc.has("database"):
			doc_type = "Database Settings"
		elif doc.has("features"):
			doc_type = "Feature Flags"
		elif doc.has("logging"):
			doc_type = "Logging Configuration"

		log_info("• Document " + str(i) + ": " + doc_type)

func process_all_documents() -> void:
	if parsed_documents == null:
		parsed_documents = YAML.parse(multi_doc_yaml)

	log_info("Processing all documents at once...")
	var all_docs = parsed_documents.get_documents()

	log_success("Retrieved " + str(all_docs.size()) + " documents as array")

	if LOG_VERBOSE:
		for i in range(all_docs.size()):
			log_info("Document " + str(i) + " keys: " + str(all_docs[i].keys()))

func create_multi_document() -> void:
	log_info("Creating multi-document YAML from separate data structures...")

	# Create individual document data
	var user_doc = {
		"user": {
			"id": 12345,
			"name": "John Doe",
			"email": "john@example.com"
		}
	}

	var preferences_doc = {
		"preferences": {
			"theme": "dark",
			"language": "en",
			"notifications": true
		}
	}

	var session_doc = {
		"session": {
			"token": "abc123xyz",
			"expires": "2024-12-31T23:59:59Z",
			"permissions": ["read", "write"]
		}
	}

	# Convert each to YAML and combine
	var documents_yaml = []

	for doc_data in [user_doc, preferences_doc, session_doc]:
		var result = YAML.stringify(doc_data)
		if result.has_error():
			log_error("Failed to stringify document: " + result.get_error())
			return
		documents_yaml.append(result.get_data())

	# Combine with document separator
	var combined_yaml = "\n---\n".join(documents_yaml)

	log_success("Created multi-document YAML successfully")

	if LOG_VERBOSE:
		log_result(combined_yaml)

		# Verify by parsing it back
		var verify_result = YAML.parse(combined_yaml)
		if !verify_result.has_error():
			log_success("Verification: " + str(verify_result.get_document_count()) + " documents parsed")

func save_multi_document_file() -> void:
	log_info("Saving multi-document YAML to file: " + OUTPUT_FILE)

	# Create sample multi-document data
	var documents = [
		{"metadata": {"created": "2024-01-01", "version": 1}},
		{"data": {"items": [1, 2, 3], "total": 6}},
		{"summary": {"status": "complete", "processed": true}}
	]

	var yaml_parts = []
	for doc in documents:
		var result = YAML.stringify(doc)
		if result.has_error():
			log_error("Failed to stringify document: " + result.get_error())
			return
		yaml_parts.append(result.get_data())

	var multi_doc_content = "\n---\n".join(yaml_parts)

	# Save to file (note: this saves as plain text, not using YAML.save_file)
	var file = FileAccess.open(OUTPUT_FILE, FileAccess.WRITE)
	if file == null:
		log_error("Failed to open file for writing")
		return

	file.store_string(multi_doc_content)
	file.close()

	log_success("Multi-document YAML saved successfully")

	if LOG_VERBOSE:
		log_result("File content:\n" + multi_doc_content)

func load_multi_document_file() -> void:
	log_info("Loading multi-document YAML file: " + OUTPUT_FILE)

	# Check if file exists
	if !FileAccess.file_exists(OUTPUT_FILE):
		log_warning("File doesn't exist. Run 'Save Multi-Document File' first.")
		return

	var result := YAML.load_file(OUTPUT_FILE)

	if result.has_error():
		log_error("File loading failed: " + result.get_error())
		return

	log_success("Multi-document YAML file loaded successfully")

	if LOG_VERBOSE:
		var doc_count = result.get_document_count()
		log_info("Loaded " + str(doc_count) + " documents from file")

		for i in range(doc_count):
			var doc = result.get_document(i)
			log_info("Document " + str(i) + ": " + str(doc.keys()))
			log_result("  " + str(doc))
