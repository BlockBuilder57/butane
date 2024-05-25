#pragma once

#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/gl/GLHeaders.hpp>
#include <core/Types.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <toml++/toml.hpp>
#include <vector>

namespace engine::core::gl {

	struct Shader;

	struct ShaderProgram {
		ShaderProgram() { glProgramObject = glCreateProgram(); }

		~ShaderProgram() { glDeleteProgram(glProgramObject); }

		void AttachShader(Shader& shader);

		/// Link the program.
		void Link();

		void Bind();
		void Unbind();

		GLuint GetID() { return glProgramObject; }

		struct Uniform {
			enum class Type {
				Unknown,
				Int,
				Float,
				Vec2,
				Vec3,
				Texture,
			};

			std::string name;
			Type type;
		};

		// TODO:
		// (proper) float2/float3 overloads
		// vec3/vec4 overloads
		// mat3/mat4 overloads

		void SetUniform(const std::string& uniform, int value);
		void SetUniform(const std::string& uniform, float value);
		void SetUniform(const std::string& uniform, const glm::vec2& vec);
		void SetUniform(const std::string& uniform, const glm::vec3& vec);
		void SetUniform(const std::string& uniform, const glm::vec4& vec);
		void SetUniform(const std::string& uniform, const glm::mat4& mat);
		// void SetUniform(const std::string& uniform, double value);

	   private:
		u32 glProgramObject {};
		std::vector<Shader*> shaderObjects;
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

} // namespace engine::core::gl