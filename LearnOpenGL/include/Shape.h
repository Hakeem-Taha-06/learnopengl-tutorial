#pragma once
#include "glad/glad.h"

#include <vector>

enum class VertexDataShape
{
	Pos3d, //holds only a 3d position
	PosCol3d, //holds a 3d position followed by three color
};

class Shape {
public:
	Shape(const std::vector<float>& vertices, 
		  const std::vector<unsigned int>& indices) 
		 :m_vertices(vertices), m_indices(indices) , m_VAO(0){}

	void create(VertexDataShape shape, GLenum usage) {
		unsigned int VBO, EBO;
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenVertexArrays(1, &m_VAO);

		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), usage);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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
		}

		std::cout << "VAO: " << m_VAO << " VBO: " << VBO << " EBO: " << EBO << std::endl;
		std::cout << "Is VAO valid ->" << glIsVertexArray(m_VAO) << std::endl;
		std::cout << "--------------------------" << std::endl;
	}

	void draw(GLenum mode) {
		glBindVertexArray(m_VAO);
		glDrawElements(mode, (int)(m_indices.size() * sizeof(unsigned int)), GL_UNSIGNED_INT, 0);
	}
	
private:
	unsigned int m_VAO;
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indices;
};