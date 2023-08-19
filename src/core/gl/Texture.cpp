// Created by block on 8/18/23.

#include <SDL2/SDL_image.h>

#include <core/gl/Texture.hpp>

namespace engine::core::gl {

	void Texture::LoadTexture(const std::filesystem::path& path) {
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapModeU);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapModeV);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TexFilterScaleMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TexFilterScaleMax);

		// auto img1filename = (core::filesystem::Filesystem::The().GetDataDir() / "textures" / "test.png");
		SDL_Surface* surface = IMG_Load(path.c_str());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		SDL_FreeSurface(surface);
	}

	void Texture::FreeTexture() {
		glDeleteTextures(1, &texID);
	}

	void Texture::Bind() {
		glBindTexture(GL_TEXTURE_2D, texID);
	}

	void Texture::Unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

} // namespace engine::core::gl
