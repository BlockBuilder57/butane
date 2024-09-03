// Created by block on 7/31/24.

#include <core/Logger.hpp>
#include "Light.hpp"
#include "Scene.hpp"
#include <format>

namespace butane::core::scene {

	LightManager& LightManager::The() {
		static LightManager the;
		return the;
	}

	const glm::vec3 pointLightPositions[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(-2.5f,  2.0f, -3.8f)
	};

	const glm::vec3 pointLightColors[] = {
		glm::vec3(0.2f, 0.2f, 0.6f),
		glm::vec3(0.3f, 0.3f, 0.7f),
		glm::vec3(1.0f, 0.0f, 0.1f),
		glm::vec3(0.4f, 0.4f, 0.4f)
	};

	void LightManager::Init() {
		environment.type = Light::Type::Environment;
		environment.direction = {-0.2f, -1.0f, -0.3f};
		environment.ambient = {0.05f, 0.05f, 0.05f};
		environment.diffuse = {0.2f, 0.2f, 0.7f};
		environment.specular = {0.7f, 0.7f, 0.7f};

		for (int i = 0; i < 4; i++) {
			points[i].type = Light::Type::Point;
			points[i].transform.SetPos(pointLightPositions[i]);
			points[i].ambient = pointLightColors[i] * 0.2f;
			points[i].diffuse = pointLightColors[i];
			points[i].specular = pointLightColors[i];
			points[i].constant = 1.0f;
			points[i].linear = 0.09f;
			points[i].quadratic = 0.032f;
		}

		spot.type = Light::Type::Spot;
		spot.transform.SetPos({});
		spot.direction = {};
		spot.ambient = {0.0f, 0.0f, 0.0f};
		spot.diffuse = {1.0f, 1.0f, 1.0f};
		spot.specular = {1.0f, 1.0f, 1.0f};
		spot.constant = 1.0f;
		spot.linear = 0.09f;
		spot.quadratic = 0.032f;
		spot.cutOff = glm::cos(glm::radians(12.5f));
		spot.outerCutOff = glm::cos(glm::radians(15.0f));
	}

	void LightManager::Shutdown() {}

	void LightManager::StartTick() {}
	void LightManager::EndTick() {}

	void LightManager::SetUniforms(gfx::ShaderProgram* program) {
		if (program == nullptr)
			return;

		program->SetUniform("m_dirLight.direction", environment.direction);
		program->SetUniform("m_dirLight.ambient", environment.ambient);
		program->SetUniform("m_dirLight.diffuse", environment.diffuse);
		program->SetUniform("m_dirLight.specular", environment.specular);

		for (int i = 0; i < 4; i++) {
			program->SetUniform(std::format("m_pointLights[{}].position", i), points[i].transform.metaPos);
			program->SetUniform(std::format("m_pointLights[{}].ambient", i), points[i].ambient);
			program->SetUniform(std::format("m_pointLights[{}].diffuse", i), points[i].diffuse);
			program->SetUniform(std::format("m_pointLights[{}].specular", i), points[i].specular);
			program->SetUniform(std::format("m_pointLights[{}].constant", i), points[i].constant);
			program->SetUniform(std::format("m_pointLights[{}].linear", i), points[i].linear);
			program->SetUniform(std::format("m_pointLights[{}].quadratic", i), points[i].quadratic);
		}

		program->SetUniform("m_spotLight.position", spot.transform.metaPos);
		program->SetUniform("m_spotLight.direction", spot.transform.metaForward);
		program->SetUniform("m_spotLight.ambient", spot.ambient);
		program->SetUniform("m_spotLight.diffuse", spot.diffuse);
		program->SetUniform("m_spotLight.specular", spot.specular);
		program->SetUniform("m_spotLight.constant", spot.constant);
		program->SetUniform("m_spotLight.linear", spot.linear);
		program->SetUniform("m_spotLight.quadratic", spot.quadratic);
		program->SetUniform("m_spotLight.cutOff", spot.cutOff);
		program->SetUniform("m_spotLight.outerCutOff", spot.outerCutOff);
	}

	void LightManager::ImGuiDebug() {}
}