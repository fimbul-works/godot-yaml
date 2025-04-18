@tool
extends Node2D

@export var run_tests := false:
	set(value):
		run_tests = value
		if test_scene:
			test_scene.visible = value

@onready var test_scene := %Tests
