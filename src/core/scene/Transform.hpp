// Created by block on 8/20/23.

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace engine::core::scene {

	struct Transform {
	   public:
		static const glm::vec3 Up;
		static const glm::vec3 Down;
		static const glm::vec3 Left;
		static const glm::vec3 Right;
		static const glm::vec3 Forward;
		static const glm::vec3 Back;

		void SetPos(const glm::vec3& pos);
		void SetRot(const glm::quat& rot);
		void SetPosRot(const glm::vec3& pos, const glm::quat& rot);

		void LookAtTarget(const glm::vec3& target, float roll = 0.f);

		glm::mat4 matrix {};

		glm::vec3 metaPos {};
		glm::quat metaRot {};
		glm::vec3 metaEuler {};

		glm::vec3 metaForward {};
		glm::vec3 metaUp {};

	   private:
		void UpdateMeta();
	};

} // namespace engine::core::scene
