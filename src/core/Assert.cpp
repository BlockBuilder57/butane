#include <cstdio>
#include <cstdlib>
#include <core/Logger.hpp>

namespace engine::core {

	[[noreturn]] void ExitMsg(const char* message) {
		LogFatal("{}", message);
		std::quick_exit(0xAF);
	}

} // namespace core