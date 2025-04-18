class_name RiskyResource extends Resource

@export var value := ""
@export var number := 0
@export var settings := {}

func do_something() -> void:
	print("Resource doing something with value: %s" % value)
