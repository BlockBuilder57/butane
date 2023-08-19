#version 330 core
//out vec4 FragColor;

in vec4 vertexColor;
in vec2 texcoord;

uniform vec2 time;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
	gl_FragColor = vec4(mod(time.x, 1.0f), 0.5f, 0.2f, 1.0f);
	//gl_FragColor = vertexColor;
	gl_FragColor *= mix(texture(texture1, texcoord), texture(texture2, texcoord), 0.5);
}
