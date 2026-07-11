#version 330 core
out vec4 FragColor;

in vec4 vertexPosition;
uniform vec4 customColor;

void main(){
	FragColor = customColor + vertexPosition + vec4(0.5, 0.5, 0.5, 1.0);
}