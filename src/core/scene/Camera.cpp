// Created by block on 8/20/23.

#include "Camera.hpp"

#include "core/sdl/Window.hpp"

namespace engine::core::scene {

	Camera::Camera() {
		// init with defaults
		if (sdl::Window::CurrentWindow != nullptr)
			aspect = sdl::Window::CurrentWindow->GetAspectRatio();

		SetAspect(aspect);
		SetFovNearFar(fov, zNear, zFar);
	}

	void Camera::SetAspect(float ratio) {
		aspect = ratio;
	}

	void Camera::SetFovNearFar(float fieldofview, float near, float far) {
		fov = fieldofview;
		zNear = near;
		zFar = far;

		matProjection = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
	}

} // namespace engine::core::scene