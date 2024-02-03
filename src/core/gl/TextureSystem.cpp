// Created by block on 1/31/24.

#include "TextureSystem.hpp"
#include "core/Logger.hpp"

namespace engine::core::gl {

	TextureSystem& TextureSystem::The() {
		static TextureSystem the;
		return the;
	}

	void TextureSystem::Init() {
		// generate single pixel textures
		defaultWhite = new gl::Texture();
		const unsigned char colorWhite[4] = { 255, 255, 255, 255 };
		defaultWhite->LoadTexture(1, 1, (void*)&colorWhite[0]);
		textureDict["./" STRINGIFY(defaultWhite)] = defaultWhite;

		defaultBlack = new gl::Texture();
		const unsigned char colorBlack[4] = { 0, 0, 0, 255 };
		defaultBlack->LoadTexture(1, 1, (void*)&colorBlack[0]);
		textureDict["./" STRINGIFY(defaultBlack)] = defaultBlack;

		// generate missing texture
		defaultMissing = new gl::Texture();
		constexpr int missingSize = 256;
		constexpr int missingStripeSize = 16;
		auto* missingData = new unsigned int[missingSize * missingSize];

		for (int i = 0; i < missingSize * missingSize; i++) {
			//missingData[i] = (i % missingSize - 1) * i;
			int x = (i % missingSize);
			int y = (i / missingSize);

			bool stripe = ((x + y)/missingStripeSize) % 2 == 0;
			bool stripe2 = ((x + y)/missingStripeSize) % 8 == 0;

			// alternating yellow and black, with every 4th black strip being magenta
			missingData[i] = stripe ? (stripe2 ? 0xFFFF00FF : 0xFF000000) : 0xFF00FFFF; // ABGR
		}

		defaultMissing->TexFilterScaleMax = defaultMissing->TexFilterScaleMin = GL_NEAREST;
		defaultMissing->LoadTexture(missingSize, missingSize, (void*)&missingData[0]);
		textureDict["./" STRINGIFY(defaultMissing)] = defaultMissing;
		delete[] missingData;

		// load textures from filesystem
		auto files = filesystem::Filesystem::WalkDirectory(filesystem::Filesystem::GetDataDir());
		for (auto file : files) {
			if (filesystem::stdfs::is_directory(file)) {
				// add ourselves if we're not in the map
				if (!debugTree.contains(file)) {
					debugTree[file] = {};
				}

				// add our parent if it's not in the map
				if (!debugTree.contains(file.parent_path())) {
					debugTree[file.parent_path()] = {};
				}

				// add ourselves to the parent's children
				auto childVec = debugTree[file.parent_path()];
				if (std::find(childVec.begin(), childVec.end(), file) == childVec.end())
					debugTree[file.parent_path()].push_back(file);

				continue;
			}

			const std::vector<std::string> whitelistFilenames = { ".png", ".jpg", ".bmp", ".xcf" };

			if (std::find(whitelistFilenames.begin(), whitelistFilenames.end(), file.extension()) != whitelistFilenames.end()) {
				auto relativePath = filesystem::Filesystem::GetRelativePathFor(file);
				textureDict[relativePath] = new gl::Texture(file);
			}
		}
	}

	void TextureSystem::Shutdown() {
		for (auto tex : textureDict) {
			delete tex.second;
		}
		textureDict.clear();
	}

	static int imguiIdx = 0;

	void TextureSystem::RecurseDirectoryStuff(filesystem::stdfs::path path) {
		if (!debugTree.contains(path))
			return;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		auto relativePath = filesystem::Filesystem::GetRelativePathFor(path);
		auto children = debugTree[path];

		imguiIdx++;
		if (ImGui::TreeNodeEx((void*)(intptr_t)imguiIdx, flags, "%s/", path.filename().c_str())) {
			for (const auto& childPath : children) {
				RecurseDirectoryStuff(childPath);
			}

			for (const auto& pair : textureDict) {
				if (pair.first.parent_path() != relativePath)
					continue;

				imguiIdx++;
				// all files act as leaves
				flags |= ImGuiTreeNodeFlags_Leaf;
				if (ImGui::TreeNodeEx((void*)(intptr_t)imguiIdx, flags, "%s", pair.first.filename().c_str())) {
					ImGui::TreePop();
				}
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
					debugPath = pair.first;
				}
			}

			ImGui::TreePop();
		}
	}

	void TextureSystem::ImGuiDebug() {
		if (!ImGuiDebugFlag)
			return;

		ImGui::Begin(GetName(), &ImGuiDebugFlag);

		imguiIdx = 0;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 64));
		ImGui::BeginChild("TextureList", ImVec2(ImGui::GetContentRegionAvail().x, 200), false);
		RecurseDirectoryStuff(filesystem::Filesystem::GetDataDir());
		ImGui::EndChild();
		ImGui::PopStyleColor();

		// draw some extra info for the selected debug path
		if (!debugPath.empty()) {
			Texture* tex = textureDict.contains(debugPath) ? textureDict[debugPath] : defaultMissing;
			if (tex != nullptr) {
				GLuint imgId = tex->GetID();
				u16 dimX = std::min((u16)ImGui::GetContentRegionAvail().x, tex->GetWidth());
				u16 dimY = std::min((u16)ImGui::GetContentRegionAvail().x, tex->GetHeight());

				ImGui::Text("Path: %s", debugPath.c_str());
				ImGui::Text("OGL Texture ID: %d", imgId);
				ImGui::Text("Dimensions: %d x %d", tex->GetWidth(), tex->GetHeight());
				ImGui::Image(reinterpret_cast<ImTextureID>(imgId), ImVec2(dimX, dimY));
			}
		}

		ImGui::End();
	}
	Texture* TextureSystem::GetDefaultTexture(DefaultTexture type) {
		switch (type) {
			case DefaultTexture::Missing:
				return defaultMissing;
			case DefaultTexture::White:
				return defaultWhite;
			case DefaultTexture::Black:
				return defaultBlack;
			default:
				return defaultMissing;
		}
	}


	Texture* TextureSystem::GetTexture(const filesystem::stdfs::path &path) {
		if (textureDict.contains(path))
			return textureDict[path];

		// no relative, check for absolute...?
		auto absolute = filesystem::Filesystem::GetAbsolutePathFor(path);
		if (textureDict.contains(absolute))
			return textureDict[absolute];

		return defaultMissing;
	}

} // namespace engine::core::gl
