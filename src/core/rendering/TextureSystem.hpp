// Created by block on 1/31/24.

#pragma once

#include <core/debug/ImGuiExtensions.hpp>
#include <core/rendering/Texture.hpp>
#include <core/System.hpp>
#include <map>

namespace butane::core::gfx {

	struct TextureSystem : public core::System {
		static TextureSystem& The();

		const char* GetName() override { return STRINGIFY(TextureSystem); }

		// implementation of [core::System] interface
		void Init() override;
		void Shutdown() override;
		void ImGuiDebug() override;

		enum class DefaultTexture {
			Missing,
			White,
			Black
		};

		// A dumb hack for early initializers...
		static Texture** GetDefaultTexturePtrPtr(DefaultTexture type);
		static Texture* GetDefaultTexturePtr(DefaultTexture type);

		Texture* GetTexture(const filesystem::stdfs::path& path);

	   private:
		static Texture* defaultMissing;
		static Texture* defaultWhite;
		static Texture* defaultBlack;

		// keys are relative to the data directory
		std::map<filesystem::stdfs::path, Texture*> textureDict {};

		filesystem::stdfs::path debugPath {};
		std::map<filesystem::stdfs::path, debug::ImGuiExtensions::DirectoryTree> debugTree {};
	};

} // namespace butane::core::rendering
