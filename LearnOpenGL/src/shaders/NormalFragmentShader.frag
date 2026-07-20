#version 330 core
out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
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
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emmision;
	int shine;
};

uniform Light light;
uniform Material material;
uniform float time;
uniform vec3 emmisionColor;

void main(){
	vec3 ambient = vec3(texture(material.diffuse, texCoord)) * light.ambient;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);

	//a dot product between the light direction and the normal of the surface
	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = diff*light.diffuse*vec3(texture(material.diffuse, texCoord));

	//need to review the math on this part
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	//a dot product between the reflected light direction around the normal and the vector of the view direction
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);
	vec3 specular = texture(material.specular, texCoord).rgb * spec * light.specular;

	vec3 emmision = vec3(0.0);
	if(texture(material.specular, texCoord).rgb == vec3(0.0)){
		emmision = texture(material.emmision, texCoord).rgb*emmisionColor;
	}
	

	vec3 finalLightColor = (specular + diffuse + ambient + emmision);

	fragColor = vec4(finalLightColor, 1.0);
}