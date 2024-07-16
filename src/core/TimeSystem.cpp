// Created by block on 7/15/24.

#include <SDL2/SDL.h>

#include "Logger.hpp"
#include "TimeSystem.hpp"

namespace engine::core {

	TimeSystem& TimeSystem::The() {
		static TimeSystem the;
		return the;
	}

	void TimeSystem::Init() {
	}

	void TimeSystem::Shutdown() {
		LogInfo("Shutting down at {:.2f} seconds ({} ticks)", nowTime, ticks);
	}

	void TimeSystem::ImGuiDebug() {
		if (!ImGuiDebugFlag)
			return;

		ImGui::Begin(GetName(), &ImGuiDebugFlag);

		ImGui::Text("Update Rate: %.3f", updateRate);
		ImGui::Text("Delta time: %.3f", deltaTime);
		ImGui::Text("Ticks: %lu", ticks);
		ImGui::Text("Tick delta: %.3f", tickDeltaTime);

		ImGui::End();
	}

	void TimeSystem::StartTick() {
		inTick = true;
		ticks++;

		UpdateNowTime();
		tickDeltaTime = (nowTime - tickLastTime);

		//LogDebug("Tick delta: {}", tickDeltaTime);
	}

	void TimeSystem::EndTick() {
		inTick = false;
		tickLastTime = nowTime;
	}

	void TimeSystem::UpdateNowTime() {
		lastTime = nowTime;
		nowTime = SDL_GetTicks64() / 1000.f;
	}

	void TimeSystem::SystemsUpdate() {
		UpdateNowTime();
		deltaTime = (nowTime - lastTime);
		tickDeltaTime = (nowTime - tickLastTime);

		//LogDebug("Render delta: {}", DeltaTime());
	}

} // namespace engine::core