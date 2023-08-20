#include <core/gl/Shader.hpp>
#include <core/Logger.hpp>

namespace engine::core::gl {

	void ShaderProgram::AttachShader(engine::core::gl::Shader& shader) {
		shader.SetProgram(this);
		shaderObjects.push_back(&shader);
		glAttachShader(glProgramObject, shader.Get());
	}

	void ShaderProgram::Link() {
		glLinkProgram(glProgramObject);
		/*for(auto* shader : shaderObjects)
			shader->Done();

		shaderObjects.resize(0);*/
	}

	void ShaderProgram::Bind() {
		glUseProgram(glProgramObject);
	}

	void ShaderProgram::Unbind() {
		glUseProgram(0);
	}

	void ShaderProgram::SetUniform(const std::string& uniform, float value) {
		glUniform1f(glGetUniformLocation(glProgramObject, uniform.c_str()), value);
	}
	void ShaderProgram::SetUniform(const std::string& uniform, float x, float y) {
		glUniform2f(glGetUniformLocation(glProgramObject, uniform.c_str()), x, y);
	}

	bool Shader::Compile() {
		// delete the existing shader, if possible
		if(glShaderObject != INVALID && compileSuccess) {
			glDeleteShader(glShaderObject);
			compileSuccess = 0;
		}

		glCompileShader(glShaderObject);
		glGetShaderiv(glShaderObject, GL_COMPILE_STATUS, &compileSuccess);
		return compileSuccess;
	}

	std::string Shader::GetInfoLog() {
		char log[4096] {};
		GLsizei len;
		glGetShaderInfoLog(glShaderObject, sizeof(log), &len, &log[0]);

		return { log, static_cast<usize>(std::min<usize>(sizeof(log), len)) };
	}

	void Shader::SetPath(const filesystem::stdfs::path& path) {
		auto file = core::filesystem::Filesystem::The().OpenAbsoluteFile(path);

		if (!file) {
			LogError("Failed to open {} for shader", path.string());
			return;
		}

		if(!fileWatch) {
			// set up filewatch for hotloads
			fileWatch = new core::filesystem::Watch(path);
			fileWatch->SetCallback([&](const core::filesystem::stdfs::path& path, core::filesystem::Watch::Event ev) {
				if (ev == core::filesystem::Watch::Event::Modify) {
					SetPath(path);

					if(!Compile()) {
						core::LogInfo("Shader hotload compilation failure: {}", GetInfoLog());
						return;
					}

					ourProgram->Link();
				}
			});
			core::filesystem::watchSystem->AddWatch(fileWatch);
		}

		SetSource(file->ReadString());
	}

	void Shader::SetSource(const std::string& source) {
		auto ptr = reinterpret_cast<const GLchar*>(source.data());
		glShaderSource(glShaderObject, 1, &ptr, nullptr);
	}

} // namespace engine::core::gl