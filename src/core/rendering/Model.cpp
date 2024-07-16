// Created by block on 7/15/24.

#include <core/rendering/GLHeaders.hpp>
#include <core/rendering/Model.hpp>
#include <core/rendering/TextureSystem.hpp>
#include <core/scene/Scene.hpp>
#include <core/TimeSystem.hpp>

namespace engine::core::gfx {

	Model::Model(const std::filesystem::path& path) {
		LoadModel(path);
	}

	void Model::Draw(glm::mat4 matModel) {
		for(Mesh mesh : meshes)
			// todo real materials
			mesh.Draw(MaterialSystem::The().GetMaterial("models/backpack/backpack.material"));
	}

	void Model::LoadModel(const std::filesystem::path& path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			LogError("ERROR::ASSIMP::{}", importer.GetErrorString());
			return;
		}

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene) {
		// process all the node's meshes (if any)
		for(unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}
		// then do the same for each of its children
		for(unsigned int i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		// std::vector<Texture> textures;

		// walk through each of the mesh's vertices
		for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			// normals
			if(mesh->HasNormals())
				vertex.normal = { mesh->mNormals[i].x mesh->mNormals[i].y, mesh->mNormals[i].z };
			// texture coordinates
			if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = vec;
			} else {
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for(unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// process materials (todo)
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices);
	}

} // namespace engine::core::gfx