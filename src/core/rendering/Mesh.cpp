// Created by block on 7/15/24.

#include <core/rendering/GLHeaders.hpp>
#include <core/rendering/Mesh.hpp>
#include <core/scene/SceneManager.hpp>
#include <core/scene/Light.hpp>
#include <core/TimeSystem.hpp>

namespace butane::core::gfx {

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
		this->vertices = vertices;
		this->indices = indices;

		SetupMesh();
	}

	void Mesh::SetupMesh() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		// free the bind
		glBindVertexArray(0);
	}

	void Mesh::Draw(Material* mat, const glm::mat4& matModel) {
		if(mat == nullptr)
			return;

		glm::mat4 submesh = matModel * transform;

		mat->BindAndSetUniforms();

		mat->shaderProgram->SetUniform("gTime", glm::vec2(core::TimeSystem::The().NowTime(), std::chrono::system_clock::now().time_since_epoch().count()));
		mat->shaderProgram->SetUniform("gMatProjection", core::scene::SceneManager::The().GetCameraProjection());
		mat->shaderProgram->SetUniform("gMatView", core::scene::SceneManager::The().GetCameraView());
		mat->shaderProgram->SetUniform("gMatModel", submesh);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

} // namespace butane::core::gfx