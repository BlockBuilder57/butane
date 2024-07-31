#pragma once
#include <core/Types.hpp>
#include <SDL2/SDL.h>
#include <filesystem>

namespace butane::core::sdl {

	struct File {
		enum class SeekDir {
			Beg = RW_SEEK_SET,
			Cur = RW_SEEK_CUR,
			End = RW_SEEK_END
		};

		static Unique<File> OpenFile(const std::filesystem::path& path, std::string_view mode);

		File(const File&) = delete;
		File(File&&) = default;

		~File();

		i64 Tell();
		i64 Seek(i64 offset, SeekDir whence);

		usize Read(void* buffer, usize size);

		std::string ReadString();

	   private:
		explicit constexpr File(SDL_RWops* ops)
			: rwops(ops) {

		}

		SDL_RWops* rwops{};
	};

}