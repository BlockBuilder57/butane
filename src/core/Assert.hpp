//! Engine Assert System
//!
//! The core library uses its own assertion system which is more flexible than the
//! standard C library's assertion macros.
//!
//! They are not intended to be directly compatible with some of the quirks
//! the Standard C library allows (like using assert() as an expression).
//!
//! They are:
//! - ENGINE_ASSERT()
//!		- active in debug builds and removed on release
//! - ENGINE_CHECK()
//! 	- always active, even in release builds

#pragma once

#include <format>

namespace engine::core {
	[[noreturn]] void ExitMsg(const char* message);
} // namespace engine::core

#ifndef NDEBUG
	#define ENGINE_ASSERT(expr, fmt, ...)                                                                                                           \
		if(!(expr)) [[unlikely]] {                                                                                                                  \
			auto msg = std::format("Assertion \"{}\" @ {}:{} failed with message: {}", #expr, __FILE__, __LINE__, std::format(fmt, ##__VA_ARGS__)); \
			::engine::core::ExitMsg(msg.c_str());                                                                                                         \
		}
#else
	#define ENGINE_ASSERT(expr, format, ...)
#endif

#define ENGINE_CHECK(expr, fmt, ...)                                                                                                        \
	if(!(expr)) [[unlikely]] {                                                                                                              \
		auto msg = std::format("Check \"{}\" @ {}:{} failed with message: {}", #expr, __FILE__, __LINE__, std::format(fmt, ##__VA_ARGS__)); \
		::engine::core::ExitMsg(msg.c_str());                                                                                                     \
	}

#define ENGINE_TODO() \
	ENGINE_CHECK(false, "TODO <3");
