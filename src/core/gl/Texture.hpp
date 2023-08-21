// Created by block on 8/18/23.

#pragma once

#include <core/gl/GLHeaders.hpp>
#include <core/filesystem/Filesystem.hpp>
#include <core/Types.hpp>

namespace engine::core::gl {

	class Texture {
	   public:
		void LoadTexture(const std::filesystem::path& path);
		void FreeTexture();

		void Bind();
		void Unbind();

		GLuint GetID() { return texID; }

		u16 WrapModeU = GL_CLAMP_TO_EDGE;
		u16 WrapModeV = GL_CLAMP_TO_EDGE;
		u16 TexFilterScaleMin = GL_NEAREST;
		u16 TexFilterScaleMax = GL_NEAREST;
	   private:

		GLuint texID {};
	};

}