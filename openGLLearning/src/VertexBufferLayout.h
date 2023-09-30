#pragma once

#include <vector>
#include <GL/glew.h>

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	bool normalized; //TODO: guardare in documentazione a cosa serve
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
public:
	VertexBufferLayout();

	template<typename T>
	void Push(int count)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, false });
	}
	
	template<>
	void Push<unsigned int>(int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, false });
	}
	
	template<>
	void Push<unsigned char>(int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, true });
	}
};