// Created by block on 8/19/23.

#ifdef __linux__
#include "Watch.linux.cpp"
#else
#include "Watch.null.cpp"
#endif

#include <core/debug/Assert.hpp>

namespace butane::core::filesystem {

	Watch::Watch(const stdfs::path& watchPath) {
		CtorImpl<WatchImpl>(watchPath);
	}

	Watch::~Watch() {
		DtorImpl<WatchImpl>();
	}

	void Watch::SetCallback(const Watch::EventCallback& callback) {
		ToImpl<WatchImpl>()->SetCallback(callback);
	}

	void Watch::Update() {
		ToImpl<WatchImpl>()->Update();
	}

	WatchSystem* watchSystem = nullptr;

	void WatchSystem::Init() {

	}

	void WatchSystem::Shutdown() {
		ENGINE_TODO();
	}

	void WatchSystem::StartTick() {
		for(auto watch : watches) {
			watch->Update();
		}
	}

	void WatchSystem::AddWatch(butane::core::filesystem::Watch* watch) {
		watches.push_back(watch);
	}

	void WatchSystem::RemoveWatch(butane::core::filesystem::Watch* watch) {
		std::erase(watches, watch);
	}

}