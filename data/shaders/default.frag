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
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
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
	// lighting
	vec3 norm = normalize(NORMAL);
	//vec3 lightDir = normalize(-light.direction);
	vec3 lightDir = normalize(light.position - FRAGPOS);

	float theta     = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TEXCOORD));

	// diffuse
	float diff = clamp(dot(norm, lightDir), 0.0, 1.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TEXCOORD));

	// specular
	vec3 viewDir = normalize(viewPos - FRAGPOS);
	// normal phong
	//vec3 reflectDir = reflect(-lightDir, norm);
	//float spec = pow(clamp(dot(viewDir, reflectDir), 0.0, 1.0), material.shininess);
	// blinn-phong
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(clamp(dot(halfVector, norm), 0.0, 1.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TEXCOORD));

	// emission
	vec3 emiss_mask = -clamp(vec3(texture(material.specular, TEXCOORD))*300, 0, 1) + 1;

	vec4 emission_full = texture(material.emission, TEXCOORD);
	vec3 emission = emission_full.rgb * (emission_full.a * emiss_mask);

	// light falloff
	float distance = length(light.position - FRAGPOS);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	//ambient *= intensity;
	diffuse *= intensity;
	specular *= intensity;

	vec3 result = ambient + diffuse + specular;
	result += emission;
	gl_FragColor = vec4(result, 1.0);
}
