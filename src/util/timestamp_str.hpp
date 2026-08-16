/**
 * @file timestamp_str.hpp
 * @brief Provides a function to get the current time as a formatted string.
 */
#pragma once

#define TICKS_MS 1000ULL
#define TICKS_SECOND (TICKS_MS * 1000ULL)
#define TICKS_MINUTE (TICKS_SECOND * 60ULL)
#define TICKS_HOUR (TICKS_MINUTE * 60ULL)

#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

/**
 * @brief Get current time as a formatted string.
 * @return String Formatted as hh:mm:ss:mmm
 */
String timestamp_str() {
	uint64_t ticks_usec = Time::get_singleton()->get_ticks_usec();

	uint64_t ticks_hour = UtilityFunctions::floori(ticks_usec / TICKS_HOUR);
	ticks_usec -= ticks_hour * TICKS_HOUR;

	uint64_t ticks_minute = UtilityFunctions::floori(ticks_usec / TICKS_MINUTE);
	ticks_usec -= ticks_minute * TICKS_MINUTE;

	uint64_t ticks_second = UtilityFunctions::floori(ticks_usec / TICKS_SECOND);
	ticks_usec -= ticks_second * TICKS_SECOND;

	uint64_t ticks_ms = UtilityFunctions::floori(ticks_usec / TICKS_MS);
	ticks_usec -= ticks_ms * TICKS_MS;

	String timestamp = vformat("%d:%02d:%02d:%03d", ticks_hour, ticks_minute, ticks_second, ticks_ms);
	return timestamp;
}

} //namespace godot
