// Created by block on 7/15/24.

#pragma once

#include <magic_enum.hpp>
#include <glm/glm.hpp>

#include <core/rendering/GLHeaders.hpp>
#include <core/rendering/MaterialSystem.hpp>
#include <core/Types.hpp>

namespace engine::core::gfx {

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};

	class Mesh {
	   public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		// temp, eventually this will be a scene object thing
		glm::mat4 transform;

		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

		void Draw(Material* mat, const glm::mat4& matModel);
		void Draw(Material* mat) { Draw(mat, glm::identity<glm::mat4>()); }

	   private:
		unsigned int VAO, VBO, EBO;

		void SetupMesh();
	};

}