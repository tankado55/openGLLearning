#pragma once
#include <string>
#include <unordered_map>

class ComputeShader
{
private:
	unsigned int m_RendererID;
	std::string m_Filepath; // debug purpose
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	ComputeShader(const std::string& filepath);
	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);

private:
	std::string ParseShader(const std::string& filepath);
	unsigned int CreateShader(const std::string& source);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	int GetUniformLocation(const std::string& name) const;
};