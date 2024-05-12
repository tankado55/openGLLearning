#include "ComputeShader.h"
#include <sstream>
#include <fstream>
#include <iostream>

#include "Renderer.h"

ComputeShader::ComputeShader(const std::string& filepath):
	m_Filepath(filepath),
	m_RendererID(0)
{
	std::string source = ParseShader(filepath);
    m_RendererID = CreateShader(source);
}

void ComputeShader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void ComputeShader::Unbind() const
{
    GLCall(glDeleteProgram(m_RendererID));
}

void ComputeShader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void ComputeShader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

std::string ComputeShader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss;
    while (getline(stream, line))
    {
        ss << line << '\n';
    }

    return ss.str();
}

unsigned int ComputeShader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); //iv = integer, vector
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); // because I want to allocate on the stack and lenght is not a costant
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

int ComputeShader::GetUniformLocation(const std::string& name) const
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

    m_UniformLocationCache[name] = location;
    return location;
}

unsigned int ComputeShader::CreateShader(const std::string& source)
{
    unsigned int program = glCreateProgram();
    unsigned int shader = CompileShader(GL_COMPUTE_SHADER, source);

    glAttachShader(program, shader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(shader);
    return program;
}
