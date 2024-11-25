// Created by block on 2/3/24.

#include "ImGuiExtensions.hpp"

#include <core/Logger.hpp>

namespace butane::core::debug {

	int ImGuiExtensions::IdIndex = 0;

	void ImGuiExtensions::PrecacheDirectories(const std::vector<std::filesystem::path>& files, std::map<std::filesystem::path, DirectoryTree>& directoryTree) {
		for (auto file : files) {
			if(!filesystem::stdfs::is_directory(file))
				continue;

			// add ourselves if we're not in the map
			if(!directoryTree.contains(file)) {
				directoryTree[file] = {};
			}

			// add our parent if it's not in the map
			if(!directoryTree.contains(file.parent_path())) {
				directoryTree[file.parent_path()] = {};
			}

			// add ourselves to the parent's directories
			auto childVec = directoryTree[file.parent_path()].directories;
			if(std::find(childVec.begin(), childVec.end(), file) == childVec.end())
				directoryTree[file.parent_path()].directories.push_back(file);
		}
	}

	std::filesystem::path ImGuiExtensions::DrawDirectoryTree(const std::filesystem::path& startPath, std::map<std::filesystem::path, DirectoryTree>& directoryTree) {
		if (startPath.empty() || directoryTree.empty())
			return {};

		// we need somewhere to start the tree
		if (!directoryTree.contains(startPath))
			return {};

		DirectoryTree thisTree = directoryTree[startPath];

		if (thisTree.directories.empty() && thisTree.files.empty()) {
			// matches the padding normally used by the tree nodes
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.x * 2);
			ImGui::TextDisabled("%s/", startPath.filename().c_str());
			return {};
		}

		std::filesystem::path toReturn = {};
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		IdIndex++;
		if (ImGui::TreeNodeEx((void*)(intptr_t)IdIndex, flags, "%s/", startPath.filename().c_str())) {
			for (const auto& dirPath : thisTree.directories) {
				toReturn = DrawDirectoryTree(dirPath, directoryTree);
			}

			for (const auto& filePath : thisTree.files) {
				IdIndex++;
				// all files act as leaves
				flags |= ImGuiTreeNodeFlags_Leaf;
				if (ImGui::TreeNodeEx((void*)(intptr_t)IdIndex, flags, "%s", filePath.filename().c_str())) {
					ImGui::TreePop();
				}
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
					toReturn = filePath;
				}
			}

			ImGui::TreePop();
		}

		return toReturn;
	}

}