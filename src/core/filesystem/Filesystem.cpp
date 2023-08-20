#include <core/filesystem/Filesystem.hpp>
#include <core/Logger.hpp>

namespace engine::core::filesystem {

	Filesystem& Filesystem::The() {
		static Filesystem the;
		return the;
	}

	stdfs::path Filesystem::GetDataDir() {
		return stdfs::current_path() / "data";
	}

	stdfs::path Filesystem::GetAbsolutePathFor(const stdfs::path& path) {
		return GetDataDir() / path;
	}

	Unique<sdl::File> Filesystem::OpenAbsoluteFile(const stdfs::path& path) {
		LogInfo("Attempting to open resource file \"{}\"", path.string());
		return sdl::File::OpenFile(path, "rb");
	}

	Unique<sdl::File> Filesystem::OpenRelativeFile(const stdfs::path& path) {
		LogInfo("Attempting to open resource file \"{}\"", (GetDataDir() / path).string());
		return sdl::File::OpenFile(GetDataDir() / path, "rb");
	}

} // namespace engine::core::filesystem
