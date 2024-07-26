// Created by block on 7/15/24.

#pragma once

#include "Logger.hpp"
#include "System.hpp"

namespace engine::core {

	struct TimeSystem : public core::System {
		static TimeSystem& The();
		const char* GetName() override { return STRINGIFY(TimeSystem); }

		// implementation of [core::System] interface
		void Init() override;
		void Shutdown() override;
		void ImGuiDebug() override;

		// these do exist in PerTickSystem, but we want to be independent of that
		void StartTick();
		void EndTick();

		void SystemsUpdate();
		void UpdateNowTime();

		float UpdateRate() { return updateRate; }
		float NowTime() { return nowTime; }
		float DeltaTime() { return deltaTime; }

		bool IsInTick() { return inTick; }
		bool TickNextReady();
		float TickConsumingTime() { return tickConsumingTime; }
		float TickElapsedTime() { return (nowTime - tickStartTime); }
		float TickDeltaTime() { return (tickStartTime - tickLastStartTime); }

	   private:
		// This is essentially how many update ticks we run (under ideal conditions)
		// This should be made a configurable value later on
		double updateRate = 1. / 50.;

		float deltaTime = 0.f;
		float lastTime = 0.f;
		float nowTime = 0.f;

		std::uint64_t ticks;
		bool inTick = false;
		float tickStartTime = 0.f;
		float tickLastStartTime = 0.f;
		float tickConsumingTime = 0.f;

	};

} // namespace core
