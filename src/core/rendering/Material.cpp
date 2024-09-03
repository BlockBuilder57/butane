// Created by block on 2/8/24.

#include <core/filesystem/TomlLoader.hpp>
#include <core/scene/Light.hpp>
#include "Material.hpp"

#include <utility>

namespace butane::core::gfx {

	Material::Material() {
	}

	Material::Material(std::string material_name, butane::core::gfx::ShaderProgram* shader) {
		name = material_name;
		shaderProgram = shader;
	}

	Material::~Material() {
		ClearUniforms();
	}

	void Material::ClearUniforms() {
		for (auto i : uniforms) {
			delete i;
		}
		uniforms.clear();
	}

	void Material::SetPath(const std::filesystem::path& path) {
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

		std::string material_name = table["name"].value_or("missing name");
		std::string program_name = table["program"].value_or("shaders/default.program");
		//LogDebug("{}, {}", material_name, program_name);
		name = material_name;
		shaderProgram = ShaderSystem::The().GetProgram(program_name);

		if (table["uniforms"].type() == toml::node_type::table) {
			SetUpUniforms(*table["uniforms"].as_table());
		}
	}

	void Material::SetUpUniforms(const toml::table& table) {
		uniforms.clear();

		for (auto& shader_uni : shaderProgram->uniforms) {
			// make a new uniform with the pointer
			auto data = new Material::UniformData(&shader_uni);
			std::string uni_name = shader_uni.name;

			switch (data->GetType()) {
				default:
				case OGLType::Unknown: { LogError("Attempting to set uniform data with unknown type"); break; };

				case OGLType::Int: {
					data->SetData(table[uni_name].value_or(MATERIAL_DEFAULT_INT));
					break;
				};
				case OGLType::Float: {
					data->SetData(table[uni_name].value_or(MATERIAL_DEFAULT_FLOAT));
					break;
				};
				case OGLType::Vec2: {
					glm::vec2 vec = {};
					if (table[uni_name].type() == toml::node_type::array)
						vec = {table[uni_name][0].value_or(MATERIAL_DEFAULT_FLOAT), table[uni_name][1].value_or(MATERIAL_DEFAULT_FLOAT)};
					data->SetData(vec);
					break;
				};
				case OGLType::Vec3: {
					glm::vec3 vec = {};
					if (table[uni_name].type() == toml::node_type::array)
						vec = {table[uni_name][0].value_or(MATERIAL_DEFAULT_FLOAT), table[uni_name][1].value_or(MATERIAL_DEFAULT_FLOAT), table[uni_name][2].value_or(MATERIAL_DEFAULT_FLOAT)};
					data->SetData(vec);
					break;
				};
				case OGLType::Vec4: {
					glm::vec4 vec = {};
					if (table[uni_name].type() == toml::node_type::array)
						vec = {table[uni_name][0].value_or(MATERIAL_DEFAULT_FLOAT), table[uni_name][1].value_or(MATERIAL_DEFAULT_FLOAT), table[uni_name][2].value_or(MATERIAL_DEFAULT_FLOAT), table[uni_name][3].value_or(MATERIAL_DEFAULT_FLOAT)};
					data->SetData(vec);
					break;
				};
				case OGLType::Color: {
					glm::vec4 vec = {}; // RGBA
					if (table[uni_name].type() == toml::node_type::array)
						vec = {table[uni_name][0].value_or(MATERIAL_DEFAULT_COLOR), table[uni_name][1].value_or(MATERIAL_DEFAULT_COLOR), table[uni_name][2].value_or(MATERIAL_DEFAULT_COLOR), table[uni_name][3].value_or(MATERIAL_DEFAULT_COLOR)};
					data->SetData(vec);
					break;
				};
				case OGLType::Texture2D: {
					data->SetData(TextureSystem::The().GetTexture(table[uni_name].value_or("defaultMissing")));
					break;
				}
			}

			uniforms.push_back(data);
		}
	}

	void Material::SetUpUniforms(std::vector<ShaderProgram::Uniform>& shader_uniforms) {
		// we're just reconnecting all of our uniforms here

		// searching our uniform data first
		for (UniformData* data : uniforms) {
			// set the uniform to null just in case a match isn't found
			std::string name = data->GetName();

		   for (ShaderProgram::Uniform& uni : shader_uniforms)
				if (name == uni.name)
					data->SetByUniform(&uni);
		}

		uniforms.erase(std::remove_if(uniforms.begin(), uniforms.end(), [](UniformData* i) { return i->GetType() == OGLType::Unknown; }), uniforms.end());
	}

	static Material* lastBoundMaterial = nullptr;
	void Material::BindAndSetUniforms() {
		if (shaderProgram == nullptr)
			return;

		// very lazy attempt at batching
		if (lastBoundMaterial == this)
			return;
		lastBoundMaterial = this;

		shaderProgram->Bind();
		auto texindex = 0;

		for (auto& data : uniforms) {
			std::string uniform_name = data->GetName();

			switch (data->GetType()) {
				default:
				case OGLType::Unknown: { LogError("Attempting to bind uniform with unknown type"); break; };

				case OGLType::Int: {
					shaderProgram->SetUniform(uniform_name, data->GetData<int>(MATERIAL_DEFAULT_INT));
					break;
				};
				case OGLType::Float: {
					shaderProgram->SetUniform(uniform_name, data->GetData<float>(MATERIAL_DEFAULT_FLOAT));
					break;
				};
				case OGLType::Vec2: {
					shaderProgram->SetUniform(uniform_name, data->GetData<glm::vec2>({MATERIAL_DEFAULT_FLOAT, MATERIAL_DEFAULT_FLOAT}));
					break;
				};
				case OGLType::Vec3: {
					shaderProgram->SetUniform(uniform_name, data->GetData<glm::vec3>({MATERIAL_DEFAULT_FLOAT, MATERIAL_DEFAULT_FLOAT, MATERIAL_DEFAULT_FLOAT}));
					break;
				};
				case OGLType::Vec4: {
					shaderProgram->SetUniform(uniform_name, data->GetData<glm::vec4>({MATERIAL_DEFAULT_FLOAT, MATERIAL_DEFAULT_FLOAT, MATERIAL_DEFAULT_FLOAT, MATERIAL_DEFAULT_FLOAT}));
					break;
				};
				case OGLType::Color: {
					shaderProgram->SetUniform(uniform_name, data->GetData<glm::vec4>({MATERIAL_DEFAULT_COLOR, MATERIAL_DEFAULT_COLOR, MATERIAL_DEFAULT_COLOR, MATERIAL_DEFAULT_COLOR}));
					break;
				};
				case OGLType::Texture2D: {
					glActiveTexture(GL_TEXTURE0 + texindex);
					Texture* tex = data->GetData(MATERIAL_DEFAULT_TEXTURE_PTR);
					tex->Bind();
					shaderProgram->SetUniform(uniform_name, texindex++);
				};
			}
		}

		scene::LightManager::The().SetUniforms(shaderProgram);
	}

	Material::UniformData::UniformData(ShaderProgram::Uniform* ptr) {
		SetByUniform(ptr);
	}

	Material::UniformData::~UniformData() {
		ClearData();
	}

	void Material::UniformData::SetByUniform(ShaderProgram::Uniform* ptr) {
		if (ptr == nullptr)
			return;

		// clear our data just in case
		if (type != ptr->type)
			ClearData();

		name = ptr->name;
		type = ptr->type;
	}

	template <class T>
	T Material::UniformData::GetData(T fallback) {
		if (data == nullptr)
			return fallback;

		if constexpr (std::is_pointer_v<T>) {
			return reinterpret_cast<T>(data);
		}
		else {
			return *reinterpret_cast<T*>(data);
		}
	}

	template <class T>
	void Material::UniformData::SetData(T value) {
		if constexpr (std::is_pointer_v<T>) {
			data = reinterpret_cast<void*>(value);
		}
		else {
			data = new T;
			memcpy(data, &value, sizeof(T));
		}
	}

	void Material::UniformData::ClearData() {
		if (data == nullptr)
			return;

		switch(type) {
			default:
			case OGLType::Unknown: {
				break;
			};
			case OGLType::Int: {
				delete static_cast<int*>(data);
				break;
			};
			case OGLType::Float: {
				delete static_cast<float*>(data);
				break;
			};
			case OGLType::Vec2: {
				delete static_cast<glm::vec2*>(data);
				break;
			};
			case OGLType::Vec3: {
				delete static_cast<glm::vec3*>(data);
				break;
			};
			case OGLType::Vec4:
			case OGLType::Color: {
				delete static_cast<glm::vec4*>(data);
				break;
			};
			case OGLType::Texture2D: {
				// we don't delete texture data!!
				//delete static_cast<Texture*>(data);
				break;
			}
		}
	}

	void Material::UniformData::ImGuiDebug() {
		// we're in a table!
		ImGui::TableSetColumnIndex(0);

		ImGui::Text("%s", name.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%s", magic_enum::enum_name(type).data());

		ImGui::TableSetColumnIndex(2);

		if (data == nullptr) {
			ImGui::TextDisabled("nullptr");
		}
		else {
			ImGui::PushItemWidth(150.f);

			std::string control_name = "##" + this->name;

			switch(type) {
				default:
				case OGLType::Unknown: {
					ImGui::TextDisabled("???");
					break;
				};

				case OGLType::Int: {
					ImGui::DragInt(control_name.c_str(), static_cast<int*>(data));
					break;
				};
				case OGLType::Float: {
					ImGui::DragFloat(control_name.c_str(), static_cast<float*>(data));
					break;
				};
				case OGLType::Vec2: {
					ImGui::DragFloat2(control_name.c_str(), static_cast<float*>(data));
					break;
				};
				case OGLType::Vec3: {
					ImGui::DragFloat3(control_name.c_str(), static_cast<float*>(data));
					break;
				};
				case OGLType::Vec4: {
					ImGui::DragFloat4(control_name.c_str(), static_cast<float*>(data));
					break;
				};
				case OGLType::Color: {
					ImGui::ColorEdit4(control_name.c_str(), static_cast<float*>(data));
					break;
				};
				case OGLType::Texture2D: {
					Texture* tex = static_cast<Texture*>(data);
					ImGui::TextDisabled("%s", tex->GetName().c_str());
				}
			}

			ImGui::PopItemWidth();
		}
	}

}