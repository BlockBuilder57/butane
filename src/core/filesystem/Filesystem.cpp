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
	stdfs::path Filesystem::GetRelativePathFor(const stdfs::path& path) {
		return stdfs::relative(path, GetDataDir());
	}

	Unique<sdl::File> Filesystem::OpenAbsoluteFile(const stdfs::path& path) {
		LogInfo("Attempting to open absolute file \"{}\"", path.string());
		return sdl::File::OpenFile(path, "rb");
	}
	Unique<sdl::File> Filesystem::OpenRelativeFile(const stdfs::path& path) {
		LogInfo("Attempting to open data file \"{}\"", path.string());
		return sdl::File::OpenFile(GetDataDir() / path, "rb");
	}

	std::vector<stdfs::path> Filesystem::WalkDirectory(const stdfs::path& path) {
		std::vector<stdfs::path> bepis;

		for (const auto &entry : stdfs::recursive_directory_iterator(path)) {
			bepis.push_back(entry.path());
		}

		return bepis;
	}

} // namespace engine::core::filesystem
