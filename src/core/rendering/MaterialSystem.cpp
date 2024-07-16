// Created by block on 6/2/24.

#include "MaterialSystem.hpp"

namespace engine::core::gfx {

	MaterialSystem& MaterialSystem::The() {
		static MaterialSystem the;
		return the;
	}

	void MaterialSystem::Init() {
		const std::filesystem::path dataPath = filesystem::Filesystem::GetDataDir() / "materials";

		// load textures from filesystem
		auto files = filesystem::Filesystem::WalkDirectory(dataPath);
		debug::ImGuiExtensions::PrecacheDirectories(files, debugTree);

		for (auto file : files) {
			if(filesystem::stdfs::is_directory(file))
				continue;

			if (file.extension() == ".material") {
				gfx::Material* mat = new gfx::Material();
				mat->SetPath(file);

				materialDict[file] = mat;

				debugTree[file.parent_path()].files.push_back(file);
			}
		}
	}

	void MaterialSystem::Shutdown() {
		for (auto i : materialDict) {
			delete i.second;
		}
		materialDict.clear();
	}

	void MaterialSystem::ImGuiDebug() {
		if (!ImGuiDebugFlag)
			return;

		ImGui::Begin(GetName(), &ImGuiDebugFlag);

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 64));
		ImGui::BeginChild("MaterialList", ImVec2(ImGui::GetContentRegionAvail().x, 180), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY);

		std::filesystem::path treeSelection = debug::ImGuiExtensions::DrawDirectoryTree(filesystem::Filesystem::GetDataDir() / "materials", debugTree);
		if (!treeSelection.empty())
			debugPath = filesystem::Filesystem::GetRelativePathFor(treeSelection);

		ImGui::EndChild();
		ImGui::PopStyleColor();

		// draw some extra info for the selected debug path
		if (!debugPath.empty()) {
			Material* mat = GetMaterial(debugPath);
			if (mat != nullptr) {
				ImGui::Text("Path: %s", debugPath.c_str());
				ImGui::Text("Name: %s", mat->GetName().c_str());

				if (ImGui::TreeNode("Uniforms")) {
					if (ImGui::BeginTable("uniforms_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
					{
						ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 100.0f);
						ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
						ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableHeadersRow();

						for (auto& uni : mat->uniforms) {
							ImGui::TableNextRow();
							uni->ImGuiDebug();
						}
						ImGui::EndTable();
					}

					ImGui::TreePop();
				}
			}
		}

		ImGui::End();
	}

	Material* MaterialSystem::GetMaterial(const filesystem::stdfs::path& path) {
		if (materialDict.contains(path))
			return materialDict[path];

		// no relative, check for absolute...?
		auto absolute = filesystem::Filesystem::GetAbsolutePathFor(path);
		if (materialDict.contains(absolute))
			return materialDict[absolute];

		// make a default?
		return nullptr;
	}

	void MaterialSystem::GetMaterialsUsingProgram(const ShaderProgram* program, std::vector<Material*>& materials) {
		materials.clear();
		for (auto i : materialDict) {
			Material* mat = i.second;
			if (mat->shaderProgram == program)
				materials.push_back(mat);
		}
	}

} // namespace engine::core::rendering
