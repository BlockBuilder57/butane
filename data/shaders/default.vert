#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 matProjection;
uniform mat4 matView;
uniform mat4 matModel;

out vec3 NORMAL;
out vec2 TEXCOORD;
out vec3 FRAGPOS;

void main() {
	FRAGPOS = vec3(matModel * vec4(aPos, 1.0));
	NORMAL = mat3(transpose(inverse(matModel))) * aNormal;

	// hack until we can flip images
	TEXCOORD = vec2(aTexCoord.x, 1.0 - aTexCoord.y);

	gl_Position = matProjection * matView * vec4(FRAGPOS, 1.0);
}
