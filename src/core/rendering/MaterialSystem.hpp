// Created by block on 6/2/24.

#pragma once

#include <core/debug/ImGuiExtensions.hpp>
#include <core/rendering/Material.hpp>
#include <core/Logger.hpp>
#include <core/System.hpp>
#include <map>

namespace engine::core::gfx {

	struct MaterialSystem : public core::System {
		static MaterialSystem& The();

		const char* GetName() override { return STRINGIFY(MaterialSystem); }

		// implementation of [core::System] interface
		void Init() override;
		void Shutdown() override;
		void ImGuiDebug() override;

		Material* GetMaterial(const filesystem::stdfs::path& path);

		/// Rebuilds the uniforms for all materials that use this program
		void GetMaterialsUsingProgram(const ShaderProgram* program, std::vector<Material*>& materials);

	   private:
		// keys are relative to the data directory
		std::map<filesystem::stdfs::path, Material*> materialDict {};

		filesystem::stdfs::path debugPath {};
		std::map<filesystem::stdfs::path, debug::ImGuiExtensions::DirectoryTree> debugTree {};
	};

} // namespace engine::core::rendering
