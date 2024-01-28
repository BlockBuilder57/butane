// Created by block on 8/18/23.

#include <SDL2/SDL_image.h>

#include <core/gl/Texture.hpp>
#include <core/Logger.hpp>

namespace engine::core::gl {

	Texture::Texture() {

	}

	Texture::Texture(const std::filesystem::path& path) {
		LoadTexture(path);
	}

	Texture::Texture(u16 wrapU, u16 wrapV, const std::filesystem::path& path) {
		this->WrapModeU = wrapU;
		this->WrapModeV = wrapV;
		LoadTexture(path);
	}

	Texture::Texture(u16 wrapU, u16 wrapV, u16 filterMin, u16 filterMax, const std::filesystem::path& path) {
		this->WrapModeU = wrapU;
		this->WrapModeV = wrapV;
		this->TexFilterScaleMin = filterMin;
		this->TexFilterScaleMax = filterMax;
		LoadTexture(path);
	}

	bool Texture::LoadTexture(const std::filesystem::path& path) {
		LogDebug("Loading texture from \"{}\"", path.c_str());
		SDL_Surface* surface = IMG_Load(path.c_str());
		if (surface) {
			// we need to determine what texture format we just opened

			GLenum texture_format = GL_RGBA;
			GLint nOfColors = surface->format->BytesPerPixel;

			// get the number of channels in the SDL surface
			if (nOfColors == 4) {
				// has alpha channel
				if (surface->format->Rmask == 0x000000ff)
					texture_format = GL_RGBA;
				else
					texture_format = GL_BGRA;
			} else if (nOfColors == 3) {
				// no alpha channel
				if (surface->format->Rmask == 0x000000ff)
					texture_format = GL_RGB;
				else
					texture_format = GL_BGR;
			} else if (nOfColors == 1) {
				// greyscale or palletized
				if (surface->format->palette != nullptr || surface->format->palette->ncolors > 0)
					LogWarning("Texture \"{}\" has palette, will appear corrupted", path.c_str());

				texture_format = GL_RED;
			} else {
				// ...two channels?
				LogWarning("Texture \"{}\" has odd number of channels, will appear corrupted", path.c_str());
			}

			//LogDebug("No. colors: {}, Rmask: {:08x}", nOfColors, surface->format->Rmask);

			InitializeOGLTexture(surface->w, surface->h, texture_format, surface->pixels);
			SDL_FreeSurface(surface);
			return true;
		}
		else {
			LogError("Failed to load texture! {}", path.c_str());
		}

		return false;
	}

	bool Texture::LoadTexture(int width, int height, void* data) {
		InitializeOGLTexture(width, height, GL_RGBA, data);
		return true;
	}
	bool Texture::LoadTexture(int width, int height, GLenum format, void* data) {
		InitializeOGLTexture(width, height, format, data);
		return true;
	}

	void Texture::InitializeOGLTexture(int width, int height, GLenum format, void* data) {
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapModeU);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapModeV);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TexFilterScaleMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TexFilterScaleMax);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
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
