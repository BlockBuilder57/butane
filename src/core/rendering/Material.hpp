// Created by block on 2/8/24.

#pragma once

#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/rendering/GLHeaders.hpp>
#include <core/Types.hpp>

#include "TextureSystem.hpp"
#include "ShaderSystem.hpp"

namespace butane::core::gfx {

	namespace MaterialDefaults {
		const int Int = 0;
		const float Float = 0.f;
		const glm::vec2 Vec2 = {};
		const glm::vec3 Vec3 = {};
		const glm::vec4 Vec4 = {};
		const glm::quat Quat = glm::identity<glm::quat>();
		const glm::vec4 Color = {1.f, 1.f, 1.f, 1.f};
		Texture** const Texture2D = TextureSystem::GetDefaultTexturePtrPtr(TextureSystem::DefaultTexture::White);
	}

	struct Material {
		friend struct MaterialSystem;

		Material();
		Material(std::string material_name, ShaderProgram* shader);
		~Material();

		void SetPath(const std::filesystem::path& path);

		std::string GetName() { return name; }

		struct UniformData {
			UniformData() = default;
			UniformData(ShaderProgram::Uniform* ptr);
			~UniformData();

			void SetByUniform(ShaderProgram::Uniform* ptr);

			std::string GetName() { return name; };
			OGLType GetType() { return type; };

			template <class T>
			T GetData(T fallback);
			template <class T>
			void SetData(T value);
			void ClearData();

			void ImGuiDebug();

		   private:
			std::string name {};
			OGLType type {};
			void* data {};
		};

		void ClearUniforms();
		void SetUpUniforms(const toml::table& table);
		void SetUpUniforms(std::vector<ShaderProgram::Uniform>& shader_uniforms);
		void BindAndSetUniforms();

		ShaderProgram* shaderProgram {};

	   private:
		std::string name {};
		std::vector<UniformData*> uniforms {};
		core::filesystem::Watch* fileWatch {};
	};

}

