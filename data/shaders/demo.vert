#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec2 time;

void main() {
	//gl_Position = vec4(aPos.x, aPos.y + (tan(time.x) * 0.2f) + cos((aPos.x+0.5)+time.x)*0.2f, aPos.z, 1.0);
	gl_Position = vec4(aPos, 1.0);
}