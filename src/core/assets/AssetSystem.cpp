// Created by block on 2025-07-26.

#include "AssetSystem.hpp"

#include <physfs.h>
#include <SDL2/SDL.h>

#include <core/Logger.hpp>
#include <core/debug/Assert.hpp>
#include <core/filesystem/Filesystem.hpp>

// ew. why isn't the CMakeLists working?
#include "../../../third_party/murmur3/murmur3.h"

namespace butane::core::assets {

	AssetSystem& AssetSystem::The() {
		static AssetSystem the;
		return the;
	}

	void recurseEnumerate(const char* dir) {
		//core::LogDebug("recurse: {}", dir);

		char** files = PHYSFS_enumerateFiles(dir);

		for (char** i = files; *i != nullptr; i++) {
			std::string pathy(dir);
			if (!pathy.ends_with('/'))
				pathy += "/";
			pathy += *i;

			PHYSFS_Stat stat;
			int res = PHYSFS_stat(pathy.c_str(), &stat);

			/*core::LogDebug("    {}", pathy);
			if (res != 0) {
				if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY)
					core::LogDebug("        It's a directory!");
				else if (stat.filetype == PHYSFS_FILETYPE_REGULAR)
					core::LogDebug("        It's a file!");
			}*/

			if (res != 0) {
				if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY)
					recurseEnumerate(pathy.c_str());
				else if (stat.filetype == PHYSFS_FILETYPE_REGULAR) {
					std::uint32_t hash;
					MurmurHash3_x86_32(pathy.data(), pathy.size(), 0, &hash);

					core::LogDebug("{} -> {:04X}", pathy, hash);
				}
			}

		}

		PHYSFS_freeList(files);
	}

	void AssetSystem::Init() {
		assert(PHYSFS_isInit());

		PHYSFS_mount(filesystem::Filesystem::GetDataDir().c_str(), "/", 0);
		//PHYSFS_mount("package.zip", "/", 1);

		recurseEnumerate("/");
	}

	void AssetSystem::Shutdown() {
	}

	void AssetSystem::ImGuiDebug() {
		if (!ImGuiDebugFlag)
			return;

		ImGui::Begin(GetName(), &ImGuiDebugFlag);

		ImGui::Text("bepis");

		ImGui::End();
	}

} // namespace butane::core