#pragma once

#include <filesystem>
#include <core/sdl/File.hpp>

namespace engine::core::filesystem {

	namespace stdfs = std::filesystem;

	struct Filesystem {
	   public:
		static Filesystem& The();

		stdfs::path GetDataDir();
		stdfs::path GetAbsolutePathFor(const stdfs::path& path);

		Unique<sdl::File> OpenAbsoluteFile(const stdfs::path& path);
		Unique<sdl::File> OpenRelativeFile(const stdfs::path& path);
	};

}