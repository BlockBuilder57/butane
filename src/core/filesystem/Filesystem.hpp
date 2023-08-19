#pragma once

#include <filesystem>
#include <core/sdl/File.hpp>

namespace engine::core::filesystem {

	namespace stdfs = std::filesystem;

	struct Filesystem {
	   public:
		static Filesystem& The();

		stdfs::path GetDataDir();

		Unique<sdl::File> OpenReadFile(const stdfs::path& path);
	};

}