#include <cstdio>
#include <cstdlib>
#include <core/Logger.hpp>

namespace butane::core::debug {

	[[noreturn]] void ExitMsg(const char* message) {
		LogFatal("{}", message);
		std::quick_exit(0xAF);
	}

} // namespace butane::core::debug