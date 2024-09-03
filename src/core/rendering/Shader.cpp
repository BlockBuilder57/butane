#include <core/filesystem/TomlLoader.hpp>
#include <core/rendering/ShaderSystem.hpp>
#include <core/rendering/MaterialSystem.hpp>
#include <core/Logger.hpp>

#include <magic_enum.hpp>

namespace butane::core::gfx {

	ShaderProgram::ShaderProgram() {
		glProgramObject = glCreateProgram();
		name = "Shader #" + std::to_string(glProgramObject);
	}

	ShaderProgram::~ShaderProgram() {
		glDeleteProgram(glProgramObject);
	}

	void ShaderProgram::AttachShader(butane::core::gfx::Shader& shader) {
		shader.AddProgram(this);
		shaderObjects.push_back(&shader);
		glAttachShader(glProgramObject, shader.Get());
	}

	void ShaderProgram::Link() {
		glLinkProgram(glProgramObject);
		/*for(auto* shader : shaderObjects)
			shader->Done();

		shaderObjects.resize(0);*/

		// let's collect our uniforms

		GLint uniformCount = 0;
		const GLsizei bufSize = 64; // maximum name length
		char name[bufSize]; // variable name in GLSL

		GLint size; // size of the variable
		OGLType type; // type of the variable (float, vec3 or mat4, etc)

		glGetProgramiv(glProgramObject, GL_ACTIVE_UNIFORMS, &uniformCount);
		//LogDebug("Shader {} has {} uniforms", glProgramObject, uniformCount);

		uniforms.clear();
		for (int i = 0; i < uniformCount; i++) {
			glGetActiveUniform(glProgramObject, i, bufSize, nullptr, &size, reinterpret_cast<GLenum*>(&type), &name[0]);

			// skip globals and privates
			if (name[0] == 'g' || (name[0] == 'm' && name[1] == '_'))
				continue;

			//LogDebug("Uniform {} ({}): a {} (size {})", name, i, magic_enum::enum_name<OGLType>(type), size);

			uniforms.emplace_back(name, type);
		}
	}

	void ShaderProgram::SetPath(const std::filesystem::path& path) {
		// attempt to make this an absolute path
		std::filesystem::path absPath = path;
		if (!path.has_root_directory())
			absPath = core::filesystem::Filesystem::GetDataDir() / path;

		if(!fileWatch) {
			// set up filewatch for hotloads
			fileWatch = new core::filesystem::Watch(absPath);
			fileWatch->SetCallback([&](const core::filesystem::stdfs::path& path, core::filesystem::Watch::Event ev) {
				if (ev == core::filesystem::Watch::Event::Modify) {
					SetPath(path);
				}
			});
			core::filesystem::watchSystem->AddWatch(fileWatch);
		}

		toml::table table = filesystem::TomlLoader::LoadTable(path);

		gfx::Shader* frag = nullptr;
		gfx::Shader* vert = nullptr;

		// build uniforms

		bool hotloading = false;
		std::vector<Material*> materials {};
		if (!uniforms.empty()) {
			hotloading = true;
			MaterialSystem::The().GetMaterialsUsingProgram(this, materials);
			uniforms.clear();
		}

		// hotloading will break everything!
		// we need to reconnect our materials
		if (hotloading) {
			for (Material* mat : materials) {
				mat->SetUpUniforms(uniforms);
			}
		}

		if (table.contains("frag"))
			frag = ShaderSystem::The().AddShader(table["frag"].value_or("shaders/src/default.frag"), Shader::Kind::Fragment);
		if (table.contains("vert"))
			vert = ShaderSystem::The().AddShader(table["vert"].value_or("shaders/src/default.vert"), Shader::Kind::Vertex);

		if (frag != nullptr)
			AttachShader(reinterpret_cast<Shader&>(*frag));
		if (vert != nullptr)
			AttachShader(reinterpret_cast<Shader&>(*vert));
		Link();
	}

	void ShaderProgram::Bind() {
		glUseProgram(glProgramObject);
	}
	void ShaderProgram::Unbind() {
		glUseProgram(0);
	}

	void ShaderProgram::Uniform::ImGuiDebug() {
		// we're in a table!
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", name.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%s", magic_enum::enum_name(type).data());
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
	void ShaderProgram::SetUniform(const std::string& uniform, const glm::quat& quat) {
		glUniform4f(glGetUniformLocation(glProgramObject, uniform.c_str()), quat.x, quat.y, quat.z, quat.w);
	}
	void ShaderProgram::SetUniform(const std::string& uniform, const glm::mat3& mat) {
		glUniformMatrix3fv(glGetUniformLocation(glProgramObject, uniform.c_str()), 1, false, &mat[0][0]);
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
			fileWatch = new core::filesystem::Watch(absPath);
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

} // namespace butane::core::rendering