#pragma once
#include <string>

class ComputeShader
{
private:
	unsigned int m_RendererID;
	std::string m_Filepath; // debug purpose

public:
	ComputeShader(const std::string& filepath);
};