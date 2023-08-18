#include <core/sdl/File.hpp>

namespace engine::core::sdl {
	Unique<File> File::OpenFile(const std::filesystem::path& path, std::string_view mode) {
		if(std::filesystem::exists(path)) {
			return Unique<File>(new File(SDL_RWFromFile(path.c_str(), mode.data())));
		} else {
			return nullptr;
		}
	}

	File::~File() {
		if(rwops)
			SDL_RWclose(rwops);
	}

	i64 File::Tell() {
		return SDL_RWtell(rwops);
	}

	i64 File::Seek(i64 offset, File::SeekDir whence) {
		return SDL_RWseek(rwops, offset, static_cast<int>(whence));
	}

	usize File::Read(void* buffer, usize size) {
		return SDL_RWread(rwops, buffer, size, 1);
	}

	std::string File::ReadString() {
		std::string c;

		Seek(0, SeekDir::End);
		auto size = Tell();
		c.resize(size);
		Seek(0, SeekDir::Beg);

		Read(c.data(), size);
		return c;
	}
}
