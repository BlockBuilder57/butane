// Created by block on 8/20/23.

#pragma once

#include <core/rendering/GLHeaders.hpp>
#include <core/scene/Camera.hpp>
#include <core/scene/SGNode.hpp>
#include <core/Types.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace engine::core::scene {

	/// root class for all scene graph nodes
	struct Node : public SGNodeMixin<Node> {};

	class Scene {
	   public:
		static Scene& The();

		glm::mat4 GetCameraView();
		glm::mat4 GetCameraProjection();
		Transform* GetCameraTransform();

		void SetCamera(Camera* newCam) { cam = newCam; }

	   private:
		std::unique_ptr<Node> graphRoot;
		Camera* cam;
	};

} // namespace engine::core::scene
