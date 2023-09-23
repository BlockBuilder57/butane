// Created by block on 8/20/23.

#include "Scene.hpp"

namespace engine::core::scene {

	Scene& Scene::The() {
		static Scene the;
		return the;
	}

	glm::mat4 Scene::GetCameraView() {
		if(cam != nullptr)
			return cam->GetView();

		return glm::identity<glm::mat4>();
	}
	glm::mat4 Scene::GetCameraProjection() {
		if(cam != nullptr)
			return cam->GetProjection();

		return glm::identity<glm::mat4>();
	}

} // namespace engine::core::scene