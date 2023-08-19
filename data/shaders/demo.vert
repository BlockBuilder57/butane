#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 texcoord;

uniform vec2 time;

void main() {
	gl_Position = vec4(aPos.x, aPos.y + (tan(time.x) * 0.2f) + cos((aPos.x+0.5)+time.x)*0.2f, aPos.z, 1.0);
	//gl_Position = vec4(aPos, 1.0);
	vertexColor = vec4(aColor, 1.0);

	// hack until we can flip images
	texcoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}