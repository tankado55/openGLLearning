#pragma once

#include "Shader.h"

class Quad
{
private:
	unsigned int quadVAO;
	unsigned int quadVBO;

public:
	Quad();
	void Draw(Shader& shader);
};