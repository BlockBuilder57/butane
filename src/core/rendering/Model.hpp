// Created by block on 7/15/24.

#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <magic_enum.hpp>
#include <glm/glm.hpp>

#include <core/rendering/GLHeaders.hpp>
#include <core/rendering/Mesh.hpp>
#include <core/Types.hpp>

namespace butane::core::gfx {

	class Model {
	   public:
		Model(const std::filesystem::path& path);

		void Draw(glm::mat4 matModel);
		void Draw(Material* material, glm::mat4 matModel);

	   private:
		std::vector<Mesh> meshes;

		void LoadModel(const std::filesystem::path& path);
		void ProcessNode(aiNode *node, const aiScene *scene);
		Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	};

}