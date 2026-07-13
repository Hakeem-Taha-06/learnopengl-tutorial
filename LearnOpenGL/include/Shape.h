#pragma once
#include "glad/glad.h"

#include <vector>

enum class VertexDataShape
{
	Pos3d,      //3d position
	PosCol3d,   //3d position + rgb value
	PosColTex3d,//3d position + rgb value + 2 texture coordinates
};

class Shape {
public:
	Shape(const std::vector<float>& vertices,
		const std::vector<unsigned int>& indices);

	~Shape();

	void create(VertexDataShape shape, GLenum usage);

	void draw(GLenum mode);
	
private:
	unsigned int m_VAO, m_VBO, m_EBO;
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indices;
};

Shape::Shape(const std::vector<float>& vertices,
	const std::vector<unsigned int>& indices)
	:m_vertices(vertices), m_indices(indices), m_VAO(0), m_VBO(0), m_EBO(0) {
}


Shape::~Shape() {
	unsigned int buffers[] = { m_VAO, m_VBO, m_VAO };
	glDeleteBuffers(3, buffers);
}

void Shape::create(VertexDataShape shape, GLenum usage) {
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), usage);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), usage);

	//what if the vertex data holds both position and color data vs only position data
	//this should be handled by another class that represents a 
	//type of vertex data that would handle its own initialization
	//or an enum that would be checked against
	/*Pos3D::Init() {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
	PosCol3d::Init() {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
		glEnableVertexAttribArray(1);
	}*/
	switch (shape) {
	case VertexDataShape::Pos3d:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		break;
	case VertexDataShape::PosCol3d:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		break;
	case VertexDataShape::PosColTex3d:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		break;
	}

	std::cout << "VAO: " << m_VAO << " VBO: " << m_VBO << " EBO: " << m_EBO << std::endl;
	std::cout << "Is VAO valid ->" << glIsVertexArray(m_VAO) << std::endl;
	std::cout << "--------------------------" << std::endl;
}

void Shape::draw(GLenum mode) {
	glBindVertexArray(m_VAO);
	glDrawElements(mode, (int)(m_indices.size() * sizeof(unsigned int)), GL_UNSIGNED_INT, 0);
}