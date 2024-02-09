// Created by block on 2/8/24.

#include "Material.hpp"

namespace engine::core::gl {

	void Material::BindAndSetUniforms() {
		if (shader == nullptr)
			return;

		glActiveTexture(GL_TEXTURE0);
		(diffuse != nullptr ? diffuse : TextureSystem::GetDefaultTexture(TextureSystem::DefaultTexture::White))->Bind();
		glActiveTexture(GL_TEXTURE1);
		(specular != nullptr ? specular : TextureSystem::GetDefaultTexture(TextureSystem::DefaultTexture::White))->Bind();
		glActiveTexture(GL_TEXTURE2);
		(emission != nullptr ? emission : TextureSystem::GetDefaultTexture(TextureSystem::DefaultTexture::Black))->Bind();

		shader->Bind();

		shader->SetUniform("material.diffuse", 0);
		shader->SetUniform("material.specular", 1);
		shader->SetUniform("material.emission", 2);
		shader->SetUniform("material.shininess", shininess);
	}

}