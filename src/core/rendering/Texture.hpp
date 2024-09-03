// Created by block on 8/18/23.

#pragma once

#include <magic_enum.hpp>

#include <core/rendering/GLHeaders.hpp>
#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/Types.hpp>

namespace butane::core::gfx {

	class Texture {
	   public:
		Texture();
		Texture(const std::string& name);
		Texture(u16 wrapU, u16 wrapV, u16 filterMin, u16 filterMax, const std::filesystem::path& path);

		~Texture();

		bool SetPath(const std::filesystem::path& path);
		bool LoadConfig(const std::filesystem::path& path);

		bool LoadTexture(int width, int height, void* data);
		bool LoadTexture(int width, int height, GLenum format, void* data);

		void FreeTexture();

		void Bind();
		void Unbind();

		std::string GetName() { return name; }
		GLuint GetID() { return texID; }

		u16 GetWidth() { return texWidth; }
		u16 GetHeight() { return texHeight; }
		GLenum GetFormat() { return texFormat; }

		GLenum WrapModeU, WrapModeV = GL_CLAMP_TO_EDGE;
		GLenum TexFilterScaleMin, TexFilterScaleMax = GL_LINEAR_MIPMAP_LINEAR;
	   private:
		void InitializeOGLTexture(int width, int height, GLenum format, void* data);

		std::string name {};
		core::filesystem::Watch* fileWatch {};
		core::filesystem::Watch* configWatch {};

		GLuint texID {};
		GLenum texFormat {};
		u16 texWidth, texHeight {};
	};

}
