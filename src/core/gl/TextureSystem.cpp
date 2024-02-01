// Created by block on 1/31/24.

#include "TextureSystem.hpp"

namespace engine::core::gl {

	TextureSystem& TextureSystem::The() {
		static TextureSystem the;
		return the;
	}

	void TextureSystem::Init() {
		const unsigned char colorWhite[4] = { 255, 255, 255, 255 };
		defaultWhite.LoadTexture(1, 1, (void*)&colorWhite[0]);

		// generate missing texture
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

		defaultMissing.TexFilterScaleMax = defaultMissing.TexFilterScaleMin = GL_NEAREST;
		defaultMissing.LoadTexture(missingSize, missingSize, (void*)&missingData[0]);
		delete[] missingData;

		// load textures from filesystem
		auto files = filesystem::Filesystem::The().WalkDirectory(filesystem::Filesystem::The().GetDataDir() / "textures");
		for (auto file : files) {
			if (filesystem::stdfs::is_directory(file))
				continue;

			if (file.extension() != ".json") {
				textureDict[file] = new gl::Texture(file);
			}
		}
	}

	void TextureSystem::Shutdown() {
		for (auto tex : textureDict) {
			delete tex.second;
		}
		textureDict.clear();
	}

	void TextureSystem::ImGuiDebug() {
		if (!ImGuiDebugFlag)
			return;

		ImGui::Begin(GetName());

		ImGui::BeginChild("TextureList", ImVec2(ImGui::GetContentRegionAvail().x, 150), false);
		for (const auto& pair : textureDict) {
			auto relPath = filesystem::Filesystem::GetRelativePathFor(pair.first);

			std::string imguiNonsense = std::string("Inspect##") + pair.first.string();
			if (ImGui::SmallButton(imguiNonsense.c_str()))
				debugPath = pair.first;
			ImGui::SameLine();
			ImGui::Text("%s", relPath.c_str());
		}
		ImGui::EndChild();

		// draw some extra info for the selected debug path
		if (!debugPath.empty()) {
			Texture* tex = textureDict.contains(debugPath) ? textureDict[debugPath] : &defaultMissing;
			if (tex != nullptr) {
				GLuint imgId = tex->GetID();
				u16 dimX = std::min((u16)ImGui::GetContentRegionAvail().x, tex->GetWidth());
				u16 dimY = std::min((u16)ImGui::GetContentRegionAvail().x, tex->GetHeight());

				ImGui::Text("Path: %s", filesystem::Filesystem::GetRelativePathFor(debugPath).c_str());
				ImGui::Text("OGL Texture ID: %d", imgId);
				ImGui::Text("Dimensions: %d x %d", tex->GetWidth(), tex->GetHeight());
				ImGui::Image(reinterpret_cast<ImTextureID>(imgId), ImVec2(dimX, dimY));
			}
		}

		ImGui::End();
	}
	Texture* TextureSystem::GetDefaultTexture(DefaultTexture type) {
		return &defaultMissing;
	}


	Texture* TextureSystem::GetTexture(const filesystem::stdfs::path &path) {
		auto absolute = filesystem::Filesystem::GetAbsolutePathFor(path);
		if (textureDict.contains(absolute))
			return textureDict[absolute];

		return &defaultMissing;
	}

} // namespace engine::core::gl
