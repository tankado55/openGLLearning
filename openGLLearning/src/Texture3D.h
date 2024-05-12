#pragma once

#include "Renderer.h"

class Texture3D
{
private:
	unsigned int m_RendererID;
	int m_Width, m_Height, m_Depth;
public:
	Texture3D(unsigned int width, unsigned int height, unsigned int depth);
	~Texture3D();

	void Bind(unsigned int slot = 0) const;
	void Unbind();

	inline int GetWidth() const { return m_Width; };
	inline int GetHeight() const { return m_Height; };
	inline int GetDepth() const { return m_Depth; };
	inline int GetId() const { return m_RendererID; };
};