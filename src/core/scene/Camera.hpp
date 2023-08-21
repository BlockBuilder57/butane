// Created by block on 8/20/23.

#pragma once

#include <core/gl/GLHeaders.hpp>
#include <core/Types.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace engine::core::scene {

	class Camera {
	   public:
		std::string Name;

		glm::mat4 GetView() { return glm::inverse(matTransform); }
		glm::mat4 GetProjection() { return matProjection; }

		void SetPos(const glm::vec3& pos);
		void SetRot(const glm::quat& rot);
		void SetPosRot(const glm::vec3& pos, const glm::quat& rot);

		void LookAtTarget(const glm::vec3& target, float roll = 0.f);

		void SetFovNearFar(float fieldofview, float near, float far);

	   //private:
		struct CameraMeta {
			glm::vec3 pos;
			glm::quat rot;
			glm::vec3 euler;
			glm::vec3 forward;
			glm::vec3 up;
		};

		void UpdateMeta();

		glm::mat4 matTransform {};
		glm::mat4 matProjection {};
		float fov { 60.f };
		float zNear { 0.1f };
		float zFar { 1000.f };
		CameraMeta meta {};
	};

}
