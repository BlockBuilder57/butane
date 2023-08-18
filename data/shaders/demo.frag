#version 330 core
out vec4 FragColor;

uniform vec2 time;

void main() {
	FragColor = vec4(mod(time.x, 1.0f), 0.5f, 0.2f, 1.0f);
}