#pragma once

#include <filesystem>
#include <core/sdl/File.hpp>
#include <vector>

namespace butane::core::filesystem {

	namespace stdfs = std::filesystem;

	struct Filesystem {
	   public:
		static Filesystem& The();

		static stdfs::path GetDataDir();
		static stdfs::path GetAbsolutePathFor(const stdfs::path& path);
		static stdfs::path GetRelativePathFor(const stdfs::path& path);

		static Unique<sdl::File> OpenAbsoluteFile(const stdfs::path& path);
		static Unique<sdl::File> OpenRelativeFile(const stdfs::path& path);

		static std::vector<stdfs::path> WalkDirectory(const stdfs::path& path);
	};

}