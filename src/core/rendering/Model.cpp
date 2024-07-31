// Created by block on 7/15/24.

#include <core/rendering/GLHeaders.hpp>
#include <core/rendering/Model.hpp>
#include <core/rendering/TextureSystem.hpp>
#include <core/scene/Scene.hpp>
#include <core/TimeSystem.hpp>

namespace butane::core::gfx {

	Model::Model(const std::filesystem::path& path) {
		LoadModel(path);
	}

	void Model::Draw(glm::mat4 matModel) {
		for(Mesh mesh : meshes)
			// todo real materials
			mesh.Draw(MaterialSystem::The().GetMaterial("models/backpack/backpack.material"), matModel);
	}

	void Model::LoadModel(const std::filesystem::path& path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			LogError("Assimp error: {}", importer.GetErrorString());
			return;
		}

		ProcessNode(scene->mRootNode, scene);
	}

	glm::mat4 ASSIMPToGLM(aiMatrix4x4 ai) {
		/*glm::mat4 ret(ai[0][0], ai[0][1], ai[0][2], ai[0][3],
					  ai[1][0], ai[1][1], ai[1][2], ai[1][3],
					  ai[2][0], ai[2][1], ai[2][2], ai[2][3],
					  ai[3][0], ai[3][1], ai[3][2], ai[3][3]);*/

		// I'm really not sure why this doesn't just work?

		aiVector3t<ai_real> ai_pos;
		aiVector3t<ai_real> ai_rot_axis;
		ai_real ai_rot_angle;
		aiVector3t<ai_real> ai_scale;
		ai.Decompose(ai_scale, ai_rot_axis, ai_rot_angle, ai_pos);

		glm::mat4 newmat = glm::mat4(1.f);
		newmat = glm::translate(newmat, {ai_pos.x, ai_pos.y, ai_pos.z});
		newmat = glm::scale(newmat, {ai_scale.x, ai_scale.y, ai_scale.z});
		if (ai_rot_axis.Length() == 1)
			newmat = glm::rotate(newmat, ai_rot_angle, {ai_rot_axis.x, ai_rot_axis.y, ai_rot_axis.z});

		return newmat;
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene) {
		// process all the node's meshes (if any)
		for(unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			Mesh ourMesh = ProcessMesh(mesh, scene);

			aiNode* walker = node->mParent;
			glm::mat4 meshTransform = ASSIMPToGLM(node->mTransformation), transformStep = {};
			while(walker != nullptr && walker != scene->mRootNode) {
				transformStep = ASSIMPToGLM(walker->mTransformation);
				meshTransform = transformStep * meshTransform;
				walker = walker->mParent;
			}

			ourMesh.transform = meshTransform;

			meshes.push_back(ourMesh);
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
				vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			// texture coordinates
			// a vertex can contain up to 8 different texture coordinates, we can only deal with one at the moment
			for (int texi = 0; texi < 1; texi++) {
				if(mesh->mTextureCoords[texi]) // does the mesh contain texture coordinates?
					vertex.texCoords = { mesh->mTextureCoords[texi][i].x, mesh->mTextureCoords[texi][i].y };
				else
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
		//aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices);
	}

} // namespace butane::core::gfx