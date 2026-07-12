#version 330 core
layout (location = 0) in vec3 aPos;

void main(){
	vec3 transformedPos; 
	
	if(gl_VertexID == 2){
		vec3 transformedPos = vec3(aPos.x, -aPos.y, aPos.z);
	}else{
		transformedPos = aPos;
	}
	gl_Position = vec4(transformedPos, 1.0);
}