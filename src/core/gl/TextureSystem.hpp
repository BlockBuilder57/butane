// Created by block on 1/31/24.

#pragma once

#include <core/System.hpp>
#include "Texture.hpp"
#include <map>

namespace engine::core::gl {

	struct TextureSystem : public core::System {
		static TextureSystem &The();

		char *GetName() override { return STRINGIFY(TextureSystem); }

		// implementation of [core::System] interface
		void Init() override;
		void Shutdown() override;
		void ImGuiDebug() override;

		enum class DefaultTexture {
			Missing,
			White,
			Black
		};

		Texture* GetDefaultTexture(DefaultTexture type);

		Texture* GetTexture(const filesystem::stdfs::path& path);

	   private:
		Texture* defaultMissing {};
		Texture* defaultWhite {};
		Texture* defaultBlack {};

		// keys are relative to the data directory
		std::map<filesystem::stdfs::path, Texture*> textureDict {};

		filesystem::stdfs::path debugPath {};
		std::map<filesystem::stdfs::path, std::vector<filesystem::stdfs::path>> debugTree {};
		void RecurseDirectoryStuff(filesystem::stdfs::path path);
	};

} // namespace engine::core::gl
