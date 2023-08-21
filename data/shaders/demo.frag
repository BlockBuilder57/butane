#version 330 core

in vec2 texcoord;

uniform vec2 time;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
	vec2 texcoordnew = texcoord;

	texcoordnew.y += cos(texcoord.x * 3.1415926 * 2) * 0.3;
	texcoordnew.x += mod(time.x * 0.2, 1.0);

	//gl_FragColor = vec4(texcoordnew.xy, 1.0, 1.0);
	gl_FragColor = texture(texture1, texcoordnew);
	//gl_FragColor *= vec4(vec3((mod(time.x, 1.0f) * 0.5) + 0.5), 1.0f);
}
