#version 330 core
out vec4 fragColor;

in vec3 color;
in vec3 normal;
in vec3 fragPos;

uniform float texInterp;

//light stuff
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ambientStrength;

void main(){
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(lightPos - fragPos);

	float diff = max(dot(norm, lightDirection),0.0);
	vec3 diffuse = diff*lightColor;

	fragColor = vec4((diff + ambient) * color, 1.0);
}