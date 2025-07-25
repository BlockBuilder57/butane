// Created by block on 2025-07-23.

#include "SceneManager.hpp"

namespace butane::core::scene {

	SceneManager& SceneManager::The() {
		static SceneManager the;
		return the;
	}

	void SceneManager::Init() {

	}

	void SceneManager::Shutdown() {}

	void SceneManager::StartTick() {}
	void SceneManager::EndTick() {}

	void SceneManager::ImGuiDebug() {}

	int SceneManager::AddScene(Scene* scene) {
		scenes.push_back(scene);
		return scenes.size() - 1;
	}

	Scene* SceneManager::GetScene() {
		assert(!scenes.empty());
		return scenes.front();
	}

	glm::mat4 SceneManager::GetCameraView() {
		return GetScene()->GetCameraView();
	}
	glm::mat4 SceneManager::GetCameraProjection() {
		return GetScene()->GetCameraProjection();
	}
	Transform* SceneManager::GetCameraTransform() {
		return GetScene()->GetCameraTransform();
	}

}