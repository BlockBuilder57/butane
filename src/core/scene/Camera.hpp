// Created by block on 8/20/23.

#pragma once

#include <core/gl/GLHeaders.hpp>
#include <core/scene/Transform.hpp>
#include <core/Types.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace engine::core::scene {

	class Camera {
	   public:
		Camera();

		glm::mat4 GetView() { return glm::inverse(transform.matrix); }
		glm::mat4 GetProjection() { return matProjection; }

		void SetAspect(float ratio);
		void SetFovNearFar(float fieldofview, float near, float far);

		std::string Name;
		Transform transform {};

	   private:
		glm::mat4 matProjection {};
		float fov { 70.f };
		float zNear { 0.1f };
		float zFar { 1000.f };
		float aspect { 1.f };
	};

} // namespace engine::core::scene
