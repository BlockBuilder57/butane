// Created by block on 2025-07-23.

#pragma once

#include <core/System.hpp>
#include "Scene.hpp"

namespace butane::core::scene {

	struct SceneManager : public core::PerTickSystem {
		static SceneManager& The();
		const char* GetName() override { return STRINGIFY(SceneManager); }

		// implementation of [core::PerTickSystem] interface
		void Init() override;
		void Shutdown() override;
		void ImGuiDebug() override;
		void StartTick() override;
		void EndTick() override;

		glm::mat4 GetCameraView();
		glm::mat4 GetCameraProjection();
		Transform* GetCameraTransform();

		int AddScene(Scene* scene);
		Scene* GetScene();

	   private:
		std::vector<Scene*> scenes;
	};

} // namespace butane
