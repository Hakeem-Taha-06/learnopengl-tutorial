#version 330 core
out vec4 FragColor;

in vec3 color;

uniform float texInterp;
uniform vec3 lightColor;

uniform float ambientStrength;

void main(){
	vec3 ambient = ambientStrength * lightColor;

	FragColor = vec4(ambient * color, 1.0);
}