#pragma once
#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

constexpr auto MAX_INFO_SIZE = 512;

class Shader {
public:
	Shader(const char* vPath, const char* fPath);
	~Shader();

	void use() const;
	inline unsigned int getID() const { return m_ID; }

	//float uniform
	void setUniformf(const char* name, float v1) const ;
	void setUniformf(const char* name, float v1, float v2) const ;
	void setUniformf(const char* name, float v1, float v2, float v3) const ;
	void setUniformf(const char* name, float v1, float v2, float v3, float v4) const ;

	//int uniform
	void setUniformi(const char* name, int v1) const;
	void setUniformi(const char* name, int v1, int v2) const;
	void setUniformi(const char* name, int v1, int v2, int v3) const;
	void setUniformi(const char* name, int v1, int v2, int v3, int v4) const;

	//unsigned int uniform
	void setUniformui(const char* name, unsigned int v1) const;
	void setUniformui(const char* name, unsigned int v1, unsigned int v2) const;
	void setUniformui(const char* name, unsigned int v1, unsigned int v2, unsigned int v3) const;
	void setUniformui(const char* name, unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4) const;

	//matrix uniform
	void setUniformMat4(const char* name, const float* matrix);

	//specific vector uniforms (same as float but easier to input)
	void setUniformVec3(const char* name, glm::vec3 v);

private:
	std::string _readFile(const char* path);
	void _createShader(const char* sourcePath, GLenum shaderType, unsigned int* shader);

	unsigned int m_ID;
};

Shader::Shader(const char* vPath, const char* fPath) {
	//-----------------------------------------
	//shader creation and compilation
	//-----------------------------------------
	//---------- vertex shader ----------
	unsigned int vertexShader;
	_createShader(vPath, GL_VERTEX_SHADER, &vertexShader);

	//---------- fragment shader ----------
	unsigned int fragShader;
	_createShader(fPath, GL_FRAGMENT_SHADER, &fragShader);

	//-----------------------------------------
	//shader program creation and linking
	//-----------------------------------------
	//create the shader program object
	m_ID = glCreateProgram();

	//attach shaders and link them and check for linking errors
	glAttachShader(m_ID, vertexShader);
	glAttachShader(m_ID, fragShader);
	glLinkProgram(m_ID);

	int linkStatus;
	char pInfoLog[MAX_INFO_SIZE];
	glGetProgramiv(m_ID, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		glGetProgramInfoLog(m_ID, MAX_INFO_SIZE, nullptr, pInfoLog);
		std::cerr << "program linking error: " << pInfoLog << std::endl;
		exit(EXIT_FAILURE);
	}

	//activate the program
	glUseProgram(m_ID);

	//delete the shaders as they are no longer needed
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}

Shader::~Shader() {
	glDeleteProgram(m_ID);
}

void Shader::use() const{
	glUseProgram(m_ID);
}

void Shader::_createShader(const char* sourcePath, GLenum shaderType, unsigned int* shader) {
	*shader = glCreateShader(shaderType);
	
	std::string shaderSource = _readFile(sourcePath);
	const char* src = shaderSource.c_str();
	glShaderSource(*shader, 1, &src, nullptr);

	glCompileShader(*shader);

	int compileStatus;
	char infoLog[MAX_INFO_SIZE];
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		glGetShaderInfoLog(*shader, MAX_INFO_SIZE, NULL, infoLog);
		std::cout << "shader compilation error: " << infoLog << std::endl;
		exit(EXIT_FAILURE);
	}
}

//reads a file and returns a string containing the contents
std::string Shader::_readFile(const char* path) {
	std::ifstream file;
	std::stringstream ss;

	file.exceptions(std::fstream::badbit);
	try {

		file.open(path, std::ios_base::in);
		if (!file.is_open()) {
			std::cerr << "error opening file: " << path << std::endl;
			exit(EXIT_FAILURE);
		}

		ss << file.rdbuf();

		file.close();
	}
	catch (std::fstream::failure e) {
		std::cerr << "File \"" << path << "\" could not open: "
			<< "(" << e.what() << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
	return ss.str();
}

//float uniform
void Shader::setUniformf(const char* name, float v1) const {
	glUniform1f(glGetUniformLocation(m_ID, name), v1);
}
void Shader::setUniformf(const char* name, float v1, float v2) const {
	glUniform2f(glGetUniformLocation(m_ID, name), v1, v2);
}
void Shader::setUniformf(const char* name, float v1, float v2, float v3) const {
	glUniform3f(glGetUniformLocation(m_ID, name), v1, v2, v3);
}
void Shader::setUniformf(const char* name, float v1, float v2, float v3, float v4) const {
	glUniform4f(glGetUniformLocation(m_ID, name), v1, v2, v3, v4);
}

//int uniform
void Shader::setUniformi(const char* name, int v1) const {
	glUniform1i(glGetUniformLocation(m_ID, name), v1);
}
void Shader::setUniformi(const char* name, int v1, int v2) const {
	glUniform2i(glGetUniformLocation(m_ID, name), v1, v2);
}
void Shader::setUniformi(const char* name, int v1, int v2, int v3) const {
	glUniform3i(glGetUniformLocation(m_ID, name), v1, v2, v3);
}
void Shader::setUniformi(const char* name, int v1, int v2, int v3, int v4) const {
	glUniform4i(glGetUniformLocation(m_ID, name), v1, v2, v3, v4);
}

//unsigned int uniform
void Shader::setUniformui(const char* name, unsigned int v1) const {
	glUniform1ui(glGetUniformLocation(m_ID, name), v1);
}
void Shader::setUniformui(const char* name, unsigned int v1, unsigned int v2) const {
	glUniform2ui(glGetUniformLocation(m_ID, name), v1, v2);
}
void Shader::setUniformui(const char* name, unsigned int v1, unsigned int v2, unsigned int v3) const {
	glUniform3ui(glGetUniformLocation(m_ID, name), v1, v2, v3);
}
void Shader::setUniformui(const char* name, unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4) const {
	glUniform4ui(glGetUniformLocation(m_ID, name), v1, v2, v3, v4);
}

//matrix uniform
void Shader::setUniformMat4(const char* name, const float* matrix) {
	glUniformMatrix4fv(glGetUniformLocation(m_ID, name), 1, GL_FALSE, matrix);
}

//specific vector uniforms (same as float but easier to input)
void Shader::setUniformVec3(const char* name, glm::vec3 v) {
	glUniform3f(glGetUniformLocation(m_ID, name), v.x, v.y, v.z);
}