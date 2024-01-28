// Created by block on 8/18/23.

#pragma once

#include <core/gl/GLHeaders.hpp>
#include <core/filesystem/Filesystem.hpp>
#include <core/Types.hpp>

namespace engine::core::gl {

	class Texture {
	   public:
		Texture();
		Texture(const std::filesystem::path& path);
		Texture(u16 wrapU, u16 wrapV, const std::filesystem::path& path);
		Texture(u16 wrapU, u16 wrapV, u16 filterMin, u16 filterMax, const std::filesystem::path& path);

		bool LoadTexture(const std::filesystem::path& path);
		bool LoadTexture(int width, int height, void* data);
		bool LoadTexture(int width, int height, GLenum format, void* data);

		void FreeTexture();

		void Bind();
		void Unbind();

		GLuint GetID() { return texID; }

		u16 WrapModeU = GL_CLAMP_TO_EDGE;
		u16 WrapModeV = GL_CLAMP_TO_EDGE;
		u16 TexFilterScaleMin = GL_LINEAR_MIPMAP_LINEAR;
		u16 TexFilterScaleMax = GL_LINEAR_MIPMAP_LINEAR;
	   private:
		void InitializeOGLTexture(int width, int height, GLenum format, void* data);
		GLuint texID {};
	};

}