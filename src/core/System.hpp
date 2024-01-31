#pragma once
#include <core/Types.hpp>
#include <core/Util.hpp>
#include <vector>
#include <imgui.h>

namespace engine::core {

	struct System {
		virtual ~System() = default;
		virtual char* GetName() = 0;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void ImGuiDebug() {};
		bool ImGuiDebugFlag = false;
	};

	/// A [System] which runs on every update tick
	struct PerTickSystem : System {
		virtual void StartTick() = 0;
		virtual void EndTick() = 0;
	};

	/// Manager for all [System]s.
	struct SystemManager {
		static SystemManager& The();

		void Add(System* system);
		void Add(PerTickSystem* system);

		void Init();
		void Shutdown();
		void ImGuiDebug();

		void StartTick();
		void EndTick();

	   private:
		std::vector<System*> systems;
		std::vector<PerTickSystem*> tickSystems;
	};

}