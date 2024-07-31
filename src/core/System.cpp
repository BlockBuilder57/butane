#include <core/Logger.hpp>
#include <core/System.hpp>
#include <vector>

#include "imgui_internal.h"

namespace butane::core {

	SystemManager& SystemManager::The() {
		static SystemManager the;
		return the;
	}

	void SystemManager::Add(System* system) {
		systems.push_back(system);
	}

	void SystemManager::Add(PerTickSystem* system) {
		tickSystems.push_back(system);
	}

	void SystemManager::Init() {
		for(auto system : systems)
			system->Init();
		for(auto system : tickSystems)
			system->Init();
	}

	void SystemManager::Shutdown() {
		for(auto system : systems)
			system->Shutdown();
		for(auto system : tickSystems)
			system->Shutdown();
	}

	void SystemManager::ImGuiDebug() {
		ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
		if (ImGui::BeginMenu("Systems")) {
			ImGui::TextDisabled("Systems");
			for(auto system : systems)
				ImGui::MenuItem(system->GetName(), nullptr, &system->ImGuiDebugFlag);
			ImGui::TextDisabled("Tick Systems");
			for(auto system : tickSystems)
				ImGui::MenuItem(system->GetName(), nullptr, &system->ImGuiDebugFlag);

			ImGui::EndMenu();
		}
		ImGui::PopItemFlag();

		for(auto system : systems) {
			if (system->ImGuiDebugFlag)
				system->ImGuiDebug();
		}
		for(auto system : tickSystems) {
			if (system->ImGuiDebugFlag)
				system->ImGuiDebug();
		}
	}

	void SystemManager::StartTick() {
		for(auto system : tickSystems) {
			system->StartTick();
		}
	}

	void SystemManager::EndTick() {
		for(auto system : tickSystems) {
			system->EndTick();
		}
	}

} // namespace butane::core