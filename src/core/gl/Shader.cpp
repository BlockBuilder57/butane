#include <core/gl/Shader.hpp>
#include <core/Logger.hpp>

namespace engine::core::gl {

	void ShaderProgram::AttachShader(engine::core::gl::Shader& shader) {
		shader.AddProgram(this);
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

	void ShaderProgram::SetUniform(const std::string& uniform, int value) {
		glUniform1i(glGetUniformLocation(glProgramObject, uniform.c_str()), value);
	}
	void ShaderProgram::SetUniform(const std::string& uniform, float value) {
		glUniform1f(glGetUniformLocation(glProgramObject, uniform.c_str()), value);
	}
	void ShaderProgram::SetUniform(const std::string& uniform, const glm::vec2& vec) {
		glUniform2f(glGetUniformLocation(glProgramObject, uniform.c_str()), vec.x, vec.y);
	}
	void ShaderProgram::SetUniform(const std::string& uniform, const glm::vec3& vec) {
		glUniform3f(glGetUniformLocation(glProgramObject, uniform.c_str()), vec.x, vec.y, vec.z);
	}
	void ShaderProgram::SetUniform(const std::string& uniform, const glm::vec4& vec) {
		glUniform4f(glGetUniformLocation(glProgramObject, uniform.c_str()), vec.x, vec.y, vec.z, vec.w);
	}
	void ShaderProgram::SetUniform(const std::string& uniform, const glm::mat4& mat) {
		glUniformMatrix4fv(glGetUniformLocation(glProgramObject, uniform.c_str()), 1, false, &mat[0][0]);
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
		// attempt to make this an absolute path
		std::filesystem::path absPath = path;
		if (!path.has_root_directory())
			absPath = core::filesystem::Filesystem::GetDataDir() / path;

		auto file = core::filesystem::Filesystem::OpenAbsoluteFile(absPath);

		if (!file) {
			LogError("Failed to open {} for shader", absPath.string());
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

					for(auto prog : programs) {
						prog->Link();
					}
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