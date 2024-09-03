// Created by block on 2/8/24.

#pragma once

#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/rendering/GLHeaders.hpp>
#include <core/Types.hpp>

#include "TextureSystem.hpp"
#include "ShaderSystem.hpp"

namespace butane::core::gfx {

#define MATERIAL_DEFAULT_INT 0
#define MATERIAL_DEFAULT_FLOAT 0.f
#define MATERIAL_DEFAULT_COLOR 1.f
#define MATERIAL_DEFAULT_TEXTURE_PTR TextureSystem::GetDefaultTexture(TextureSystem::DefaultTexture::White)

	class Material {
		friend struct MaterialSystem;

	   public:
		struct Defaults {
			static int Int;
			static float Float;
			static glm::vec4 Color;
			static Texture* Tex;
		};

		Material();
		Material(std::string material_name, ShaderProgram* shader);
		~Material();

		void SetPath(const std::filesystem::path& path);

		std::string GetName() { return name; }

		class UniformData {
		   public:
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

