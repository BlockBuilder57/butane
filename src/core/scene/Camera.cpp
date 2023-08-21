// Created by block on 8/20/23.

#include "Camera.hpp"

namespace engine::core::scene {

	void Camera::SetPos(const glm::vec3& pos) {
		SetPosRot(pos, meta.rot);
	}
	void Camera::SetRot(const glm::quat& rot) {
		SetPosRot(meta.pos, rot);
	}
	void Camera::SetPosRot(const glm::vec3& pos, const glm::quat& rot) {
		float angle = glm::angle(rot);
		glm::vec3 axis = glm::axis(rot);

		glm::mat4 newmat = glm::mat4(1.f);
		newmat = glm::translate(newmat, pos);
		newmat = glm::rotate(newmat, angle, axis);

		matTransform = newmat;
		UpdateMeta();
	}

	void Camera::LookAtTarget(const glm::vec3& target, float roll) {
		glm::vec3 direction = meta.pos - target;
		direction = glm::normalize(direction);

		const glm::vec3 up = {0, 1, 0};
		glm::quat rotation = glm::rotate(glm::radians(roll), direction);

		// lookAt gives a view matrix, so we need to invert it
		matTransform = glm::inverse(glm::lookAt(meta.pos, target, up * rotation));
		UpdateMeta();
	}

	void Camera::SetFovNearFar(float fieldofview, float near, float far) {
		fov = fieldofview;
		zNear = near;
		zFar = far;

		matProjection = glm::perspective(glm::radians(fov), 800.f / 600.f, zNear, zFar);
	}

	void Camera::UpdateMeta() {
		glm::vec3 pos {};
		glm::quat rot {};
		glm::vec3 scale {};
		glm::vec3 skew {};
		glm::vec4 perspective {};

		// decompose existing matrix
		glm::decompose(matTransform, scale, rot, pos, skew, perspective);

		meta.pos = pos;
		meta.rot = rot;
		meta.euler = glm::degrees(glm::eulerAngles(rot));

		glm::vec3 forward = { 0, 0, 1 };
		glm::vec3 up = { 0, 1, 0 };
		meta.forward = rot * forward;
		meta.up = rot * up;
	}

}