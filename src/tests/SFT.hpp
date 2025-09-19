#pragma once

#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/node.hpp>

#include "ForEachMacro.hpp"
// Uncomment this so clangd can actually work in the ifdefs...make sure to redisable it before compiling!
// #define DEBUG_ENABLED

#ifdef DEBUG_ENABLED

// Defines that can be tweaked to change the output
#define TEST_PRINT_FUNCTION godot::UtilityFunctions::print_rich
#define PRINT_TEST(message, name, file, line, condition) TEST_PRINT_FUNCTION(godot::vformat(message, name, file, line, condition));
#define TEST_FAIL_MESSAGE "%-55s | [color=red]Failed[/color] [[color=yellow]%s:%s[/color]] - [code]%s[/code]"
#define TEST_PASS_MESSAGE "%-55s | [color=green]Passed[/color] %s%s%s"
#define TEST_CASE_NAME_MESSAGE "[color=cyan]%s[/color]"

#define SCENE_TEST_FAIL_MESSAGE(scene_path, message)                                                                                                                                   \
	TEST_PRINT_FUNCTION(godot::vformat("\n[color=cyan]Scene Test[/color]: %-43s | [color=red]Failed[/color] [[color=yellow]%s:%s[/color]] - %s %s.", scene_path.replace("res://", ""), \
			__FILE__, __LINE__, scene_path, message))
#define SCENE_TEST_PASS_MESSAGE(scene_path) TEST_PRINT_FUNCTION(godot::vformat("\n[color=cyan]Scene Test[/color]: %-43s | [color=green]Passed[/color]", scene_path.replace("res://", "")))

#define TEST_PASS(test_name) TEST_PRINT_FUNCTION(godot::vformat("%-55s | [color=green]Passed[/color]", test_name));
#define TEST_FAIL(test_name, message) TEST_PRINT_FUNCTION(godot::vformat("%-55s | [color=red]Failed[/color] [[color=yellow]%s:%s[/color]] - %s", test_name, __FILE__, __LINE__, message));

// Defines to use in your test conditions and checks
#define VAR_CHECK(first, second) first == godot::Variant(second)
#define STRING_CHECK(first, second) first == godot::StringName(second)

/*
Checks if a pointer is null, if it is report the check as failed and goto the null_first label with goto.

Why use goto here? It seems to be the only way to:
1. Perform the null check
2. Allow tests on that pointer after the check without crashing (so all the tests that are possible to run are actually run and are done in a safe way)
3. Prevent returning if it is a nullptr. I do not want to ever use a return statement in SFT so all tests can be put in a single function.
4. Do this without using exceptions (godot does not use exceptions)

This effectively makes it so:
1. All tests will always be run even if some tests that rely on a valid pointer fail.
2. The program won't crash because it is trying to run tests on a nullptr.

So I think this is a phenomenal use case for goto. Clangd will also let you know if the label doesn't exist so it's pretty hard to mess up and easy to debug.
*/
#define NULL_CHECK(object)                                                                                  \
	if (object == nullptr) {                                                                                \
		TEST_FAIL(godot::vformat("%s nullptr check", #object), godot::vformat("%s is a nullptr!", #object)) \
		goto null_##object;                                                                                 \
	}                                                                                                       \
	{ // Open goto scope, gets closed in TEST_OBJECT_END

/*
Make sure to use TEST_OBJECT_END on object_name after this at some point or it won't compile.
Note that because of the goto usage in NULL_CHECK if you declare any variables after TEST_OBJECT and before TEST_OBJECT_END they will have to be wrapped in a scoped block
SFT does this automatically but it means vars declared after TEST_OBJECT and before TEST_OBJECT_END will not be valid after TEST_OBJECT_END.
More info here: https://stackoverflow.com/a/14274292
*/
#define TEST_OBJECT(class_name, object_name)        \
	class_name *object_name = memnew(class_name()); \
	NULL_CHECK(object_name)

// Cleanup and close goto scopes.
#define TEST_OBJECT_END(object_name) \
	}                                \
	null_##object_name : if (object_name != nullptr) memdelete(object_name);
#define TEST_SCENE_END(object_name) \
	}                               \
	null_##object_name : if (object_name != nullptr) object_name->queue_free();

// Defines you or I will never need to change (hopefully)
inline bool SFT_check_result = false;
#define TEST_MESSAGE SFT_check_result ? TEST_PASS_MESSAGE : TEST_FAIL_MESSAGE
#define TEST_FILE SFT_check_result ? "" : __FILE__
#define TEST_LINE SFT_check_result ? "" : godot::vformat("%s", __LINE__)
#define TEST_CONDITION(condition) SFT_check_result ? "" : #condition
inline int SFT_check_number = 1;

// TEST_SCENE works pretty much exactly like TEST_OBJECT. Make sure to call TEST_SCENE_END at the end of the test.
// Note that the goto scope opens AFTER setting the object_name variable and is closed in TEST_SCENE_END.
// NOLINTBEGIN(cppcoreguidelines-avoid-goto)
#define TEST_SCENE(scene_path, root_node_class_name, object_name)                                                                                                  \
	root_node_class_name *object_name = nullptr;                                                                                                                   \
	{                                                                                                                                                              \
		if (!ResourceLoader::get_singleton()->exists(scene_path)) {                                                                                                \
			SCENE_TEST_FAIL_MESSAGE(String(scene_path), "does not exist");                                                                                         \
			goto null_##object_name;                                                                                                                               \
		}                                                                                                                                                          \
		const Ref<PackedScene> scene_ref##_object_name = ResourceLoader::get_singleton()->load(scene_path, "PackedScene", ResourceLoader::CACHE_MODE_IGNORE_DEEP); \
		if (!scene_ref##_object_name->can_instantiate()) {                                                                                                         \
			SCENE_TEST_FAIL_MESSAGE(String(scene_path), "could not be instantiated");                                                                              \
			goto null_##object_name;                                                                                                                               \
		}                                                                                                                                                          \
		Node *node##_object_name = scene_ref##_object_name->instantiate();                                                                                         \
		(object_name) = Object::cast_to<root_node_class_name>(node##_object_name);                                                                                 \
		if ((object_name) == nullptr) {                                                                                                                            \
			SCENE_TEST_FAIL_MESSAGE(String(#root_node_class_name), "node is a nullptr");                                                                           \
			goto null_##object_name;                                                                                                                               \
		}                                                                                                                                                          \
		SCENE_TEST_PASS_MESSAGE(String(scene_path));                                                                                                               \
// NOLINTEND(cppcoreguidelines-avoid-goto, cppcoreguidelines-avoid-do-while)

// The actual tests
#define NAMED_TESTS(test_case_name, ...)                                         \
	TEST_PRINT_FUNCTION(godot::vformat(TEST_CASE_NAME_MESSAGE, test_case_name)); \
	FOR_EACH_THREE(CHECK_NAMED, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define TESTS(test_case_name, ...)                                               \
	SFT_check_number = 1;                                                        \
	TEST_PRINT_FUNCTION(godot::vformat(TEST_CASE_NAME_MESSAGE, test_case_name)); \
	FOR_EACH_TWO(CHECK, test_case_name, __VA_OPT__(__VA_ARGS__, ))

// The condition checks that the tests perform
#define CHECK(test_case_name, condition)                                                        \
	SFT_check_result = condition;                                                               \
	PRINT_TEST(TEST_MESSAGE, SFT_check_number, TEST_FILE, TEST_LINE, TEST_CONDITION(condition)) \
	SFT_check_number++;

#define CHECK_NAMED(test_case_name, check_name, condition) \
	SFT_check_result = condition;                          \
	PRINT_TEST(TEST_MESSAGE, check_name, TEST_FILE, TEST_LINE, TEST_CONDITION(condition))

#endif // DEBUG_ENABLED
