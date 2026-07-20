#version 330 core
out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
uniform vec3 color;

uniform vec3 cameraPos;

struct PointLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	
	float constant;
	float linear;
	float quadratic;
};

struct DirLight{
    vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cutoff;
	float outerCutoff;
	float constant;
	float linear;
	float quadratic;
};

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emmision;
	int shine;
};

uniform PointLight pLight;
uniform PointLight pLight2;
uniform DirLight dLight;
uniform SpotLight sLight;
uniform Material material;
uniform float time;
uniform vec3 emmisionColor;

vec3 calculatePointLight(PointLight l){

	vec3 ambient = vec3(texture(material.diffuse, texCoord)) * l.ambient;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(l.position - fragPos);

	//a dot product between the light direction and the normal of the surface
	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = diff*l.diffuse*vec3(texture(material.diffuse, texCoord));

	//need to review the math on this part
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	//a dot product between the reflected light direction around the normal and the vector of the view direction
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);
	vec3 specular = texture(material.specular, texCoord).rgb * spec * l.specular;

	float d = length(l.position - fragPos);
	float attenuation = 1.0/(l.constant + l.linear*d + l.quadratic*d*d);
	
	ambient*=attenuation;
	diffuse*=attenuation;
	specular*=attenuation;

	return (ambient + diffuse + specular);
}

vec3 calculateDirectionalLight(DirLight l){
	vec3 ambient = vec3(texture(material.diffuse, texCoord)) * l.ambient;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(-l.direction);

	//a dot product between the light direction and the normal of the surface
	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = diff*l.diffuse*vec3(texture(material.diffuse, texCoord));

	//need to review the math on this part
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	//a dot product between the reflected light direction around the normal and the vector of the view direction
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);
	vec3 specular = texture(material.specular, texCoord).rgb * spec * l.specular;
	
	return (ambient + diffuse + specular);
}

vec3 calculateSpotLight(SpotLight l){
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(l.position - fragPos);

	//the cosine of the angle of the light ray relative to the light facing direction
	float theta = dot(lightDir, normalize(-l.direction));

	vec3 ambient = vec3(texture(material.diffuse, texCoord)) * l.ambient;

	//a dot product between the light direction and the normal of the surface
	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = diff*l.diffuse*vec3(texture(material.diffuse, texCoord));

	//need to review the math on this part
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	//a dot product between the reflected light direction around the normal and the vector of the view direction
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);
	vec3 specular = texture(material.specular, texCoord).rgb * spec * l.specular;

	if(l.outerCutoff > l.cutoff) l.outerCutoff = l.cutoff;
	//light edge smoothing
	float epsilon = l.cutoff - l.outerCutoff;
	float intensity = clamp((theta - l.outerCutoff)/epsilon, 0.0, 1.0);

	diffuse*=intensity;
	specular*=intensity;

	//attenuation
	float d = length(l.position - fragPos);
	float attenuation = 1.0/(l.constant + l.linear*d + l.quadratic*d*d);
	
	ambient*=attenuation;
	diffuse*=attenuation;
	specular*=attenuation;

	return (ambient + diffuse + specular);
}

void main(){
	vec3 finalLightColor = vec3(0.0);

	finalLightColor += calculateDirectionalLight(dLight);
	finalLightColor += calculatePointLight(pLight);
	finalLightColor += calculatePointLight(pLight2);
	finalLightColor += calculateSpotLight(sLight);

	vec3 emmision = texture(material.emmision, texCoord).rgb*emmisionColor;

	finalLightColor += emmision;

	fragColor = vec4(finalLightColor, 1.0);
}