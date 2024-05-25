// Created by block on 5/25/24.

#include "ShaderSystem.hpp"

namespace engine::core::gl {

	ShaderSystem& ShaderSystem::The() {
		static ShaderSystem the;
		return the;
	}

	void ShaderSystem::Init() {
		const std::filesystem::path dataPath = filesystem::Filesystem::GetDataDir() / "shaders";

		// load textures from filesystem
		auto files = filesystem::Filesystem::WalkDirectory(dataPath);
		debug::ImGuiExtensions::PrecacheDirectories(files, debugTree);

		for (auto file : files) {
			if(filesystem::stdfs::is_directory(file))
				continue;

			if (file.extension() == ".program") {
				toml::parse_result res = toml::parse_file(file.c_str());

				// how do I error handle this now...?

				toml::table table = *res.as_table();

				gl::ShaderProgram* prog = new gl::ShaderProgram();
				gl::Shader* frag = nullptr;
				gl::Shader* vert = nullptr;

				if (table.contains("frag"))
					frag = AddShader(table["frag"].value_or("shaders/src/default.frag"), Shader::Kind::Fragment);
				if (table.contains("vert"))
					vert = AddShader(table["vert"].value_or("shaders/src/default.vert"), Shader::Kind::Vertex);

				if (frag != nullptr)
					prog->AttachShader(reinterpret_cast<Shader&>(*frag));
				if (vert != nullptr)
					prog->AttachShader(reinterpret_cast<Shader&>(*vert));
				prog->Link();

				programDict[file] = prog;

				//auto childVec = debugTree[file.parent_path()].files;
				//if (std::find(childVec.begin(), childVec.end(), file) == childVec.end())
				debugTree[file.parent_path()].files.push_back(file);
			}
		}
	}

	void ShaderSystem::Shutdown() {
	}

	void ShaderSystem::ImGuiDebug() {
		if (!ImGuiDebugFlag)
			return;

		ImGui::Begin(GetName(), &ImGuiDebugFlag);

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 64));
		ImGui::BeginChild("ProgramList", ImVec2(ImGui::GetContentRegionAvail().x, 180), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY);

		std::filesystem::path treeSelection = debug::ImGuiExtensions::DrawDirectoryTree(filesystem::Filesystem::GetDataDir() / "shaders", debugTree);
		if (!treeSelection.empty())
			debugPath = filesystem::Filesystem::GetRelativePathFor(treeSelection);

		ImGui::EndChild();
		ImGui::PopStyleColor();

		// draw some extra info for the selected debug path
		if (!debugPath.empty()) {
			ImGui::Text("Path: %s", debugPath.c_str());
		}

		ImGui::End();
	}

	Shader* ShaderSystem::AddShader(const filesystem::stdfs::path& path, Shader::Kind kind) {
		// make sure path is absolute
		filesystem::stdfs::path absPath = filesystem::Filesystem::GetAbsolutePathFor(path);

		Shader* shader = GetShader(absPath);
		if (shader != nullptr){
			// we already have this shader! no need to recreate it
			core::LogDebug("We have {} already, returning it", absPath.c_str());
			return shader;
		}

		// we do need to get this shader now
		shader = new gl::Shader(kind);
		shader->SetPath(absPath);

		if(!shader->Compile()) {
			core::LogInfo("Shader initial load compilation failure: {}", shader->GetInfoLog());
			return nullptr;
		}

		shaderDict[absPath] = shader;

		return shader;
	}

	Shader* ShaderSystem::GetShader(const filesystem::stdfs::path& path) {
		if (shaderDict.contains(path))
			return shaderDict[path];

		// no relative, check for absolute...?
		auto absolute = filesystem::Filesystem::GetAbsolutePathFor(path);
		if (shaderDict.contains(absolute))
			return shaderDict[absolute];

		// make a default?
		return nullptr;
	}

	ShaderProgram* ShaderSystem::GetProgram(const filesystem::stdfs::path& path) {
		if (programDict.contains(path))
			return programDict[path];

		// no relative, check for absolute...?
		auto absolute = filesystem::Filesystem::GetAbsolutePathFor(path);
		if (programDict.contains(absolute))
			return programDict[absolute];

		// make a default
		core::LogError("Returning a null program!!!");
		return nullptr;
	}

} // namespace engine::core::gl