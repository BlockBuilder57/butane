// Created by block on 2/3/24.

#pragma once

#include <imgui.h>

#include <core/Util.hpp>
#include <core/filesystem/Filesystem.hpp>
#include <map>
#include <vector>
#include <filesystem>

namespace engine::core::debug {

	struct ImGuiExtensions {
		struct DirectoryTree {
			std::vector<std::filesystem::path> directories;
			std::vector<std::filesystem::path> files;
		};

		static int IdIndex;

		// Takes in a start path, and a directory tree of files and directories under that start path.
		static std::filesystem::path DrawDirectoryTree(const std::filesystem::path& startPath, std::map<std::filesystem::path, DirectoryTree>& directoryTree);
	};

}