#pragma once
#include <glad/glad.h>

#include <vector>

enum VertexDataShape
{
	Pos3d,          //3d position
	PosCol3d,       //3d position + rgb value
	PosColTex3d,    //3d position + rgb value + 2 texture coordinates
	PosColNormTex3d,//3d position + rgb value + normal vector + 2 texture coordinates 
	PosNorm3d,
};

class Shape {
public:
	Shape(const std::vector<float>& vertices,
		const std::vector<unsigned int>& indices);

	~Shape();

	void create(VertexDataShape shape, GLenum usage);
	void draw(GLenum mode);

	//so that other shapes can reuse this shape's buffers
	inline unsigned int getVBO() const { return m_VBO; }
	inline unsigned int getEBO() const { return m_EBO; }

	//so that this shape can reuse other shapes' buffers
	inline void setVBO(unsigned int VBO) { 
		m_VBO = VBO;
		if (m_VBO != 0) {
			isCachedVBO = true;
		}
	}
	inline void setEBO(unsigned int EBO) { 
		m_EBO = EBO; 
		if (m_EBO != 0) {
			isCachedEBO = true;
		}
	}
	
private:
	unsigned int m_VAO, m_VBO, m_EBO;
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indices;
	bool isCachedVBO = false;
	bool isCachedEBO = false;
};

Shape::Shape(const std::vector<float>& vertices,
	const std::vector<unsigned int>& indices)
	:m_vertices(vertices), m_indices(indices), m_VAO(0), m_VBO(0), m_EBO(0) {
}


Shape::~Shape() {
	unsigned int buffers[] = { m_VBO, m_EBO};
	glDeleteBuffers(2, buffers);
	glDeleteVertexArrays(1, &m_VAO);
}

void Shape::create(VertexDataShape shape, GLenum usage) {
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//only generate new buffers and send their data to the gpu if we aren't reusing existing ones
	if (!isCachedVBO) {
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), usage);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	}
	
	if (!isCachedEBO) {
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), usage);
	}
	else {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	}

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

	//this is getting out of hand
	switch (shape) {
	case Pos3d:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		break;
	case PosCol3d:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		break;
	case PosColTex3d:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		break;
	case PosColNormTex3d:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
		glEnableVertexAttribArray(3);
		break;
	case PosNorm3d:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	std::cout << "VAO: " << m_VAO << " VBO: " << m_VBO << " EBO: " << m_EBO << std::endl;
	std::cout << "Is VAO valid ->" << glIsVertexArray(m_VAO) << std::endl;
	std::cout << "--------------------------" << std::endl;
}

void Shape::draw(GLenum mode) {
	glBindVertexArray(m_VAO);
	glDrawElements(mode, (int)m_indices.size(), GL_UNSIGNED_INT, 0);
}