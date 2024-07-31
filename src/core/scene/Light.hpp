// Created by block on 7/31/24.

#pragma once

#include <core/rendering/Shader.hpp>
#include "SGNode.hpp"

namespace engine::core::scene {

	class LightManager;

	struct Light : public SGNodeMixin<Light> {
		friend class LightManager;

		enum class Type {
			Point,
			Spot,
			Environment
		};

		Light() {};
		Light(Type type) : type(type) {};

		glm::vec3 GetColor() { return diffuse; };

	   protected:
		Type type;
		glm::vec3 direction;

		float cutOff;
		float outerCutOff;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct LightManager : public core::PerTickSystem {
		static LightManager& The();
		const char* GetName() override { return STRINGIFY(LightManager); }

		// implementation of [core::PerTickSystem] interface
		void Init() override;
		void Shutdown() override;
		void ImGuiDebug() override;
		void StartTick() override;
		void EndTick() override;

		void SetUniforms(gfx::ShaderProgram* program);

		Light environment;
		Light points[4];
		Light spot;
	};

} // namespace engine::core::scene
