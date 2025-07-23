// Created by block on 2025-07-23.

#include "Wrap.hpp"

#include <core/filesystem/WatchSystem.hpp>
#include <core/InputSystem.hpp>
#include <core/rendering/GLHeaders.hpp>
#include <core/rendering/MaterialSystem.hpp>
#include <core/rendering/TextureSystem.hpp>
#include <core/rendering/ShaderSystem.hpp>
#include <core/scene/Light.hpp>
#include <core/TimeSystem.hpp>

namespace butane::wrap::systems {

	void RegisterSystemsAndInit() {
		// Create time system
		core::SystemManager::The().Add(static_cast<core::System*>(&core::TimeSystem::The()));

		// Create input system
		core::SystemManager::The().Add(static_cast<core::PerTickSystem*>(&core::InputSystem::The()));

		// Create file watch system
		core::filesystem::watchSystem = new core::filesystem::WatchSystem;
		core::SystemManager::The().Add(static_cast<core::PerTickSystem*>(core::filesystem::watchSystem));

		// Create shader, texture, and material systems
		core::SystemManager::The().Add(static_cast<core::System*>(&core::gfx::ShaderSystem::The()));
		core::SystemManager::The().Add(static_cast<core::System*>(&core::gfx::TextureSystem::The()));
		core::SystemManager::The().Add(static_cast<core::System*>(&core::gfx::MaterialSystem::The()));

		// Create systems for the scene
		core::SystemManager::The().Add(static_cast<core::PerTickSystem*>(&core::scene::LightManager::The()));

		// Init all registered systems
		core::SystemManager::The().Init();
	}

};
