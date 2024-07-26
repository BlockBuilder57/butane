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

		ImGui::Text("Render delta time: %.5fms", deltaTime * 1000.f);
		ImGui::Text("Update Rate: %.5fs (%.1f/s)", updateRate, 1./updateRate);
		ImGui::Text("Ticks: %lu", ticks);
		ImGui::Text("Tick delta: %.5fs", TickDeltaTime());

		ImGui::End();
	}

	void TimeSystem::StartTick() {
		tickConsumingTime -= updateRate;
		inTick = true;
		ticks++;

		tickLastStartTime = tickStartTime;
		UpdateNowTime();
		tickStartTime = nowTime;
	}

	void TimeSystem::EndTick() {
		inTick = false;
	}

	void TimeSystem::UpdateNowTime() {
		lastTime = nowTime;
		nowTime = SDL_GetTicks64() / 1000.f;
	}

	bool TimeSystem::TickNextReady() {
		// check if we have another tick to consume later
		if ((tickConsumingTime - updateRate) >= updateRate) {
			LogWarning("Can't keep up! Tick {} took too long!", ticks);
		}

		return tickConsumingTime >= updateRate;
	}

	void TimeSystem::SystemsUpdate() {
		UpdateNowTime();
		deltaTime = (nowTime - lastTime);
		tickConsumingTime += deltaTime;

		//LogDebug("Render delta: {}", DeltaTime());
	}

} // namespace engine::core