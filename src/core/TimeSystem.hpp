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
		void UpdateTickNowTime();

		float UpdateRate() { return updateRate; }
		float NowTime() { return nowTime; }
		float DeltaTime() { return deltaTime; }

		int TicksElapsed() { return ticks; }
		float TickDeltaTime() { return tickDeltaTime; }

	   private:
		// This is essentially how many update ticks we run (under ideal conditions)
		// This should be made a configurable value later on
		double updateRate = 1. / 100.;

		float deltaTime = 0.f;
		float lastTime = 0.f;
		float nowTime = 0.f;

		std::uint64_t ticks;
		bool inTick = false;
		float tickDeltaTime = updateRate + 1.f;
		float tickLastTime = 0.f;

	};

} // namespace core
