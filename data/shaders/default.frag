#version 330 core

in vec3 NORMAL;
in vec2 TEXCOORD;
in vec3 FRAGPOS;

uniform vec2 time;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 viewPos;

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	sampler2D emission;
};

uniform Material material;

void main() {
	float ambientStrength = 1.0;
	float specularStrength = 0.5;
	float specularExponent = 32.0 * 5;

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TEXCOORD));

	vec3 norm = normalize(NORMAL);
	vec3 lightDir = normalize(light.position - FRAGPOS);

	float diff = clamp(dot(norm, lightDir), 0.0, 1.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TEXCOORD));

	vec3 viewDir = normalize(viewPos - FRAGPOS);
	// normal phong
	//vec3 reflectDir = reflect(-lightDir, norm);
	//float spec = pow(clamp(dot(viewDir, reflectDir), 0.0, 1.0), material.shininess);
	// blinn-phong
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(clamp(dot(halfVector, norm), 0.0, 1.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TEXCOORD));

	vec4 emission_full = texture(material.emission, TEXCOORD);
	vec3 emission = emission_full.rgb * emission_full.a;

	vec3 result = ambient + diffuse + specular + emission;
	gl_FragColor = vec4(result, 1.0);
}
