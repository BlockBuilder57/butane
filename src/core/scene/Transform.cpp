// Created by block on 8/20/23.

#include "Transform.hpp"

namespace engine::core::scene {

	const glm::vec3 Transform::Up =      {  0,  1, 0 };
	const glm::vec3 Transform::Down =    {  0, -1, 0 };
	const glm::vec3 Transform::Left =    { -1,  0, 0 };
	const glm::vec3 Transform::Right =   {  1,  0, 0 };
	const glm::vec3 Transform::Forward = {  0,  0,-1 };
	const glm::vec3 Transform::Back =    {  0,  0, 1 };

	void Transform::SetPos(const glm::vec3& pos) {
		SetPosRot(pos, metaRot);
	}
	void Transform::SetRot(const glm::quat& rot) {
		SetPosRot(metaPos, rot);
	}
	void Transform::SetPosRot(const glm::vec3& pos, const glm::quat& rot) {
		float angle = glm::angle(rot);
		glm::vec3 axis = glm::axis(rot);

		glm::mat4 newmat = glm::mat4(1.f);
		newmat = glm::translate(newmat, pos);
		newmat = glm::rotate(newmat, angle, axis);

		matrix = newmat;
		UpdateMeta();
	}

	void Transform::LookAtTarget(const glm::vec3& target, float roll /*= 0.f*/) {
		glm::vec3 direction = metaPos - target;
		direction = glm::normalize(direction);

		const glm::vec3 up = { 0, 1, 0 };
		glm::quat rotation = glm::rotate(glm::radians(roll), direction);

		// lookAt gives a view matrix, so we need to invert it
		matrix = glm::inverse(glm::lookAt(metaPos, target, up * rotation));
		UpdateMeta();
	}

	void Transform::UpdateMeta() {
		glm::vec3 pos {};
		glm::quat rot {};
		glm::vec3 scale {};
		glm::vec3 skew {};
		glm::vec4 perspective {};

		// decompose existing matrix
		glm::decompose(matrix, scale, rot, pos, skew, perspective);

		metaPos = pos;
		metaRot = rot;
		metaEuler = glm::degrees(glm::eulerAngles(rot));

		glm::vec3 forward = { 0, 0, 1 };
		glm::vec3 up = { 0, 1, 0 };
		metaForward = rot * forward;
		metaUp = rot * up;
	}

} // namespace engine::core::scene