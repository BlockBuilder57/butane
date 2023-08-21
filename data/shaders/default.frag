#version 330 core

in vec3 NORMAL;
in vec2 TEXCOORD;
in vec3 FRAGPOS;

uniform vec2 time;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 viewPos;
uniform vec3 lightPos;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
	float ambientStrength = 0.1;
	float specularStrength = 0.5;

	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(NORMAL);
	vec3 lightDir = normalize(lightPos - FRAGPOS);

	float diff = clamp(dot(norm, lightDir), 0.0, 1.0);
	vec3 diffuse = diff * lightColor;

	vec3 viewDir = normalize(viewPos - FRAGPOS);
	// normal phong
	//vec3 reflectDir = reflect(-lightDir, norm);
	//float spec = pow(clamp(dot(viewDir, reflectDir), 0.0, 1.0), 32.0);
	// blinn-phong
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(clamp(dot(halfVector, norm), 0.0, 1.0), 32.0);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	gl_FragColor = vec4(result, 1.0);
}
