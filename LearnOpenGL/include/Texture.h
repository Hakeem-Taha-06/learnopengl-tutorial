#pragma once
#include <glad/glad.h>
#include <stb/stb_image.h>

class Texture {
public:

	//for now all textures are configured the same way
	Texture(const char* imagePath);

	~Texture();

	void use();

	inline unsigned int getID() const { return m_ID; }

	void setUnit(unsigned int unit);

private:
	unsigned int m_ID;
};

Texture::Texture(const char* imagePath) {
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float borderColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
}

Texture::~Texture() {
	glDeleteTextures(1, &m_ID);
}

void Texture::use() {
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::setUnit(unsigned int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}