#include "Texture3D.h"

Texture3D::Texture3D(unsigned int width, unsigned int height, unsigned int depth)
{
	glGenTextures(1, &m_RendererID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, m_RendererID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, width, height, depth, 0, GL_RGBA,
		GL_FLOAT, NULL);

	glBindImageTexture(0, m_RendererID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

Texture3D::~Texture3D()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture3D::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	glBindTexture(GL_TEXTURE_3D, m_RendererID);
}

void Texture3D::Unbind()
{
	GLCall(glBindTexture(GL_TEXTURE_3D, 0));
}
