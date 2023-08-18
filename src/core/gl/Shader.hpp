#pragma once
#include <core/gl/GLHeaders.hpp>
#include <core/Types.hpp>
#include <vector>

namespace engine::core::gl {

	struct Shader;

	struct ShaderProgram {
		ShaderProgram() { glProgramObject = glCreateProgram(); }

		~ShaderProgram() { glDeleteProgram(glProgramObject); }

		void AttachShader(Shader& shader);

		/// Link the program. Note that this finalizes all
		/// attached shader objects, since they are no longer needed
		void Link();

		void Bind();
		void Unbind();

		// TODO:
		// (proper) float2/float3 overloads
		// vec3/vec4 overloads
		// mat3/mat4 overloads

		void SetUniform(const std::string& uniform, float value);

		void SetUniform(const std::string& uniform, float x, float y); // for now

		void SetUniform(const std::string& uniform, double value);

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

		void Done() {
			if(glShaderObject != INVALID) {
				glDeleteShader(glShaderObject);
				glShaderObject = INVALID;
			}
		}

		bool Compile();

		std::string GetInfoLog();

		void SetSource(const std::string& source);

		[[nodiscard]] u32 Get() { return glShaderObject; }

	   private:
		constexpr static u32 INVALID = static_cast<u32>(-1);
		u32 glShaderObject { INVALID };
	};

} // namespace engine::core::gl