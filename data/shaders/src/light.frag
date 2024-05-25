#version 330 core

in vec3 NORMAL;
in vec2 TEXCOORD;
in vec3 FRAGPOS;

uniform vec2 time;
uniform vec3 lightColor;

void main() {
	gl_FragColor = vec4(lightColor, 1.0);
}
