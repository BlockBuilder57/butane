#version 330 core

in vec2 texcoord;

uniform vec2 time;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
	gl_FragColor = texture(texture1, texcoord);
	gl_FragColor *= vec4(vec3((mod(time.x, 1.0f) * 0.5) + 0.5), 1.0f);
}
