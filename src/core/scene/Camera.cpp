// Created by block on 8/20/23.

#include "Camera.hpp"

namespace engine::core::scene {

	Camera::Camera() {
		// init with defaults
		SetFovNearFar(fov, zNear, zFar);
	}

	void Camera::SetFovNearFar(float fieldofview, float near, float far) {
		fov = fieldofview;
		zNear = near;
		zFar = far;

		matProjection = glm::perspective(glm::radians(fov), 1280.f / 720.f, zNear, zFar);
	}

} // namespace engine::core::scene