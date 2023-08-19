#pragma once
#include <core/Types.hpp>
#include <vector>

namespace engine::core {

	struct System {
		virtual ~System() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
	};

	/// A [System] which runs on every update tick
	struct PerTickSystem : System {
		virtual void Tick() = 0;
	};

	/// Manager for all [System]s.
	struct SystemManager {
		static SystemManager& The();

		void Add(System* system);
		void Add(PerTickSystem* system);

		void Init();
		void Shutdown();

		void Tick();

	   private:
		std::vector<System*> systems;
		std::vector<PerTickSystem*> tickSystems;
	};

}