// Created by block on 5/25/24.

#pragma once

#include <core/debug/ImGuiExtensions.hpp>
#include <core/rendering/Shader.hpp>
#include <core/Logger.hpp>
#include <core/System.hpp>
#include <map>

namespace butane::core::gfx {

	struct ShaderSystem : public core::System {
		static ShaderSystem& The();

		const char* GetName() override { return STRINGIFY(ShaderSystem); }

		// implementation of [core::System] interface
		void Init() override;
		void Shutdown() override;
		void ImGuiDebug() override;

		Shader* AddShader(const filesystem::stdfs::path& path, Shader::Kind kind);

		Shader* GetShader(const filesystem::stdfs::path& path);
		ShaderProgram* GetProgram(const filesystem::stdfs::path& path);

	   private:
		// keys are relative to the data directory
		std::map<filesystem::stdfs::path, Shader*> shaderDict {};
		std::map<filesystem::stdfs::path, ShaderProgram*> programDict {};

		filesystem::stdfs::path debugPath {};
		std::map<filesystem::stdfs::path, debug::ImGuiExtensions::DirectoryTree> debugTree {};
	};

} // namespace butane::core::rendering
