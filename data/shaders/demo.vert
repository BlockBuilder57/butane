#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 texcoord;

uniform vec2 time;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main() {
	gl_Position = matProjection * matView * matModel * vec4(aPos, 1.0);
	//gl_Position = vec4(aPos, 1.0);
	vertexColor = vec4(aColor, 1.0);

	// hack until we can flip images
	texcoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}