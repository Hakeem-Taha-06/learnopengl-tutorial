#version 330 core
out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
uniform vec3 color;//unused

//light stuff
uniform vec3 cameraPos;

struct Light{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int shine;
};

uniform Light light;
uniform Material material;

void main(){
	vec3 ambient = material.ambient * light.ambient;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);

	//a dot product between the light direction and the normal of the surface
	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = diff*light.diffuse*material.diffuse;

	//need to review the math on this part
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	//a dot product between the reflected light direction around the normal and the vector of the view direction
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);
	vec3 specular = material.specular * spec * light.specular;

	vec3 finalLightColor = (specular + diffuse + ambient);

	fragColor = vec4(finalLightColor, 1.0);
}