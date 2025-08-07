// Created by block on 2025-07-26.

#pragma once

#include <cstdint>

namespace butane::core::assets {

	enum AssetType {
		Model,
		Texture,
		TOML // Shader, Material, Texture info?
	};

	class AssetBase {
		std::uint32_t hash;
		std::uint32_t crc;
		AssetType type;
	};

	template <class T>
	struct OfAssetType {
	   private:
		T* asset;
	};

} // namespace butane::core::assets
