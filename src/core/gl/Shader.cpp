#include <core/gl/Shader.hpp>

namespace engine::core::gl {

	void Program::AttachShader(engine::core::gl::Shader& shader) {
		shaderObjects.push_back(&shader);
		glAttachShader(glProgramObject, shader.Get());
	}

	void Program::Link() {
		glLinkProgram(glProgramObject);
		for(auto* shader : shaderObjects)
			shader->Done();

		shaderObjects.resize(0);
	}

	void Program::Bind() {
		glUseProgram(glProgramObject);
	}

	void Program::Unbind() {
		glUseProgram(0);
	}

	void Program::SetUniform(const std::string& uniform, float value) {
		glUniform1f(glGetUniformLocation(glProgramObject, uniform.c_str()), value);
	}
	void Program::SetUniform(const std::string& uniform, float x, float y) {
		glUniform2f(glGetUniformLocation(glProgramObject, uniform.c_str()), x, y);
	}

	bool Shader::Compile() {
		glCompileShader(glShaderObject);
		i32 success {};
		glGetShaderiv(glShaderObject, GL_COMPILE_STATUS, &success);
		return success;
	}

	void Shader::SetSource(const std::string& source) {
		auto ptr = reinterpret_cast<const GLchar*>(source.data());
		glShaderSource(glShaderObject, 1, &ptr, nullptr);
	}

} // namespace engine::core::gl