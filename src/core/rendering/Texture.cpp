// Created by block on 8/18/23.

#include <SDL2/SDL_image.h>
#include <toml++/toml.h>

#include <core/rendering/Texture.hpp>
#include <core/filesystem/TomlLoader.hpp>
#include <core/Logger.hpp>

namespace butane::core::gfx {

	Texture::Texture() {

	}

	Texture::Texture(const std::string& name) {
		this->name = name;
	}

	Texture::Texture(u16 wrapU, u16 wrapV, u16 filterMin, u16 filterMax, const std::filesystem::path& path) {
		this->WrapModeU = wrapU;
		this->WrapModeV = wrapV;
		this->TexFilterScaleMin = filterMin;
		this->TexFilterScaleMax = filterMax;
		SetPath(path);
	}

	Texture::~Texture() {
		FreeTexture();

		if (fileWatch != nullptr)
			core::filesystem::watchSystem->RemoveWatch(fileWatch);
	}

	bool Texture::SetPath(const std::filesystem::path& path) {
		// attempt to make this an absolute path
		std::filesystem::path absPath = path;
		if (!path.has_root_directory())
			absPath = core::filesystem::Filesystem::GetDataDir() / path;

		// free the texture if it exists
		FreeTexture();

		LogDebug("Loading texture from \"{}\"", absPath.c_str());

		SDL_Surface* surface = IMG_Load(absPath.c_str());
		if (!surface) {
			LogError("Failed to load texture! {}", absPath.c_str());
			return false;
		}

		// set the path
		name = absPath.filename();

		// set up hotloading
		if(!fileWatch) {
			// set up filewatch for hotloads
			fileWatch = new core::filesystem::Watch(absPath);
			fileWatch->SetCallback([&](const core::filesystem::stdfs::path& path, core::filesystem::Watch::Event ev) {
				if (ev == core::filesystem::Watch::Event::Modify) {
					SetPath(path);
				}
			});
			core::filesystem::watchSystem->AddWatch(fileWatch);
		}

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

	bool Texture::LoadConfig(const std::filesystem::path& path) {
		toml::table table = filesystem::TomlLoader::LoadTable(path);
		if (table.empty())
			return false;

		// set up config hotloading
		if(!configWatch) {
			// set up filewatch for hotloads
			configWatch = new core::filesystem::Watch(path);
			configWatch->SetCallback([&](const core::filesystem::stdfs::path& path, core::filesystem::Watch::Event ev) {
				if (ev == core::filesystem::Watch::Event::Modify) {
					LoadConfig(path);
				}
			});
			core::filesystem::watchSystem->AddWatch(configWatch);
		}

		if (table.contains(STRINGIFY(WrapModeU)))
			WrapModeU = (GLenum)filesystem::TomlLoader::GetEnum<OGLTextureWrap>(table[STRINGIFY(WrapModeU)], OGLTextureWrap::REPEAT);
		if (table.contains(STRINGIFY(WrapModeV)))
			WrapModeV = (GLenum)filesystem::TomlLoader::GetEnum<OGLTextureWrap>(table[STRINGIFY(WrapModeV)], OGLTextureWrap::REPEAT);
		if (table.contains(STRINGIFY(TexFilterScaleMin)))
			TexFilterScaleMin = (GLenum)filesystem::TomlLoader::GetEnum<OGLTextureFilter>(table[STRINGIFY(TexFilterScaleMin)], OGLTextureFilter::NEAREST);
		if (table.contains(STRINGIFY(TexFilterScaleMax)))
			TexFilterScaleMax = (GLenum)filesystem::TomlLoader::GetEnum<OGLTextureFilter>(table[STRINGIFY(TexFilterScaleMax)], OGLTextureFilter::NEAREST);

		if (texID != 0) {
			glBindTexture(GL_TEXTURE_2D, texID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapModeU);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapModeV);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TexFilterScaleMin);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TexFilterScaleMax);
		}

		return true;
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
		texWidth = width;
		texHeight = height;
		texFormat = format;

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
		if (texID == 0)
			return;

		name = "";
		glDeleteTextures(1, &texID);
	}

	void Texture::Bind() {
		glBindTexture(GL_TEXTURE_2D, texID);
	}

	void Texture::Unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

} // namespace butane::core::rendering
