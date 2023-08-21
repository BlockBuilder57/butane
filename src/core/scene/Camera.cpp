// Created by block on 8/20/23.

#include "Camera.hpp"

namespace engine::core::scene {

	void Camera::SetFovNearFar(float fieldofview, float near, float far) {
		fov = fieldofview;
		zNear = near;
		zFar = far;

		matProjection = glm::perspective(glm::radians(fov), 800.f / 600.f, zNear, zFar);
	}

} // namespace engine::core::scene