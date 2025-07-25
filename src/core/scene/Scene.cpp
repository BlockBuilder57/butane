// Created by block on 8/20/23.

#include "Scene.hpp"

namespace butane::core::scene {

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
	Transform* Scene::GetCameraTransform() {
		if(cam != nullptr)
			return &cam->transform;

		return nullptr;
	}

} // namespace butane::core::scene
