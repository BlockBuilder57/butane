#pragma once

#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/rendering/GLHeaders.hpp>
#include <core/Types.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>

namespace butane::core::gfx {

	struct Shader;

	struct ShaderProgram {
		ShaderProgram();
		~ShaderProgram();

		void AttachShader(Shader& shader);

		/// Link the program.
		void Link();

		void SetPath(const std::filesystem::path& path);

		void Bind();
		void Unbind();

		GLuint GetID() { return glProgramObject; }

		struct Uniform {
			void ImGuiDebug();

			std::string name {};
			OGLType type {};
		};

		std::vector<Uniform> uniforms;

		// TODO:
		// (proper) float2/float3 overloads

		void SetUniform(const std::string& uniform, int value);
		void SetUniform(const std::string& uniform, float value);
		void SetUniform(const std::string& uniform, const glm::vec2& vec);
		void SetUniform(const std::string& uniform, const glm::vec3& vec);
		void SetUniform(const std::string& uniform, const glm::vec4& vec);
		void SetUniform(const std::string& uniform, const glm::quat& quat);
		void SetUniform(const std::string& uniform, const glm::mat3& mat);
		void SetUniform(const std::string& uniform, const glm::mat4& mat);
		// void SetUniform(const std::string& uniform, double value);

	   private:
		u32 glProgramObject {};
		std::string name {};
		std::vector<Shader*> shaderObjects;
		core::filesystem::Watch* fileWatch {};
	};

	struct Shader {
		enum class Kind : GLenum {
			Vertex = GL_VERTEX_SHADER,
			Fragment = GL_FRAGMENT_SHADER
			// probably compute @ some point
		};

		explicit Shader(Kind kind) { glShaderObject = glCreateShader(static_cast<GLenum>(kind)); }

		~Shader() { Done(); }

		void AddProgram(ShaderProgram* program) { programs.push_back(program); }

		void Done() {
			if(glShaderObject != INVALID) {
				glDeleteShader(glShaderObject);
				glShaderObject = INVALID;
				programs.clear();
			}
		}

		bool Compile();

		std::string GetInfoLog();

		void SetPath(const filesystem::stdfs::path& path);
		void SetSource(const std::string& source);

		[[nodiscard]] u32 Get() { return glShaderObject; }

	   private:
		std::vector<ShaderProgram*> programs {};
		core::filesystem::Watch* fileWatch {};

		constexpr static u32 INVALID = static_cast<u32>(-1);
		u32 glShaderObject { INVALID };
		i32 compileSuccess {};
	};

} // namespace butane::core::rendering