#version 330 core
out vec4 fragColor;

in vec3 color;
in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

uniform float texInterp;

//light stuff
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shine;

uniform sampler2D texture1; 

void main(){
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = diff*lightColor;

	//need to review the math on this part
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shine);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 finalColor = (specular + diff + ambient) * color;

	fragColor = vec4(finalColor, 1.0)*texture(texture1, texCoord);
}