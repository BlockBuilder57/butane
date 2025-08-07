// Created by block on 2025-07-26.

#pragma once

#include <core/System.hpp>

namespace butane::core::assets {

	struct AssetSystem : public core::System {
		static AssetSystem& The();
		const char* GetName() override { return STRINGIFY(AssetSystem); }

		// implementation of [core::System] interface
		void Init() override;
		void Shutdown() override;
		void ImGuiDebug() override;

	   private:

	};

} // namespace core
