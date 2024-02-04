// Created by block on 8/18/23.

#pragma once

#include <magic_enum.hpp>

#include <core/gl/GLHeaders.hpp>
#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/Types.hpp>

namespace engine::core::gl {

	class Texture {
	   public:
		Texture();
		Texture(const std::filesystem::path& path);
		Texture(u16 wrapU, u16 wrapV, const std::filesystem::path& path);
		Texture(u16 wrapU, u16 wrapV, u16 filterMin, u16 filterMax, const std::filesystem::path& path);

		~Texture();

		bool SetPath(const std::filesystem::path& path);
		bool LoadConfig(const std::filesystem::path& path);

		bool LoadTexture(int width, int height, void* data);
		bool LoadTexture(int width, int height, GLenum format, void* data);

		void FreeTexture();

		void Bind();
		void Unbind();

		GLuint GetID() { return texID; }

		u16 GetWidth() { return texWidth; }
		u16 GetHeight() { return texHeight; }
		GLenum GetFormat() { return texFormat; }

		GLenum WrapModeU, WrapModeV = GL_CLAMP_TO_EDGE;
		GLenum TexFilterScaleMin, TexFilterScaleMax = GL_LINEAR_MIPMAP_LINEAR;
	   private:
		// dummy enums for loading config w/ magic_enum
		enum class OGLTextureWrap {
			CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
			MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
			REPEAT = GL_REPEAT,
			MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE
		};
		enum class OGLTextureFilter {
			NEAREST = GL_NEAREST,
			LINEAR = GL_LINEAR,
			NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
			LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
			NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
			LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
		};

		void InitializeOGLTexture(int width, int height, GLenum format, void* data);

		core::filesystem::Watch* fileWatch {};
		core::filesystem::Watch* configWatch {};

		GLuint texID {};
		GLenum texFormat {};
		u16 texWidth, texHeight {};
	};

}

// I WAS HERE FOR THREE HOURS
template <>
struct magic_enum::customize::enum_range<engine::core::gl::Texture::OGLTextureWrap> {
	static constexpr int min = 0x2900;
	static constexpr int max = 0x8750;
	// (max - min) must be less than UINT16_MAX.
};
template <>
struct magic_enum::customize::enum_range<engine::core::gl::Texture::OGLTextureFilter> {
	static constexpr int min = 0x2600;
	static constexpr int max = 0x2704;
	// (max - min) must be less than UINT16_MAX.
};
