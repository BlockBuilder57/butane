// Created by block on 2/8/24.

#pragma once

#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/gl/GLHeaders.hpp>
#include <core/Types.hpp>

#include "Texture.hpp"
#include "TextureSystem.hpp"
#include "Shader.hpp"

namespace engine::core::gl {

	struct Material {
		ShaderProgram* shader {};

		Texture* diffuse {};
		Texture* specular {};
		Texture* emission {};

		float shininess {};

		void BindAndSetUniforms();
	};

}

