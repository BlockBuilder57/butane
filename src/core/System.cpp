#include <core/Logger.hpp>
#include <core/System.hpp>
#include <vector>

namespace engine::core {

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

} // namespace engine::core