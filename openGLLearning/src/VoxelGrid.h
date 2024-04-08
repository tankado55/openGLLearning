#pragma once

#include "vector"
#include "glm/glm.hpp"
#include "Model.h"

class VoxelGrid
{
public:
	glm::vec3 size;
	float resolution;
	glm::mat4 modelMatrix;
	unsigned voxelCount;
	std::vector<float> status;
	GLuint tboID;
	GLuint textureID;

public:
	VoxelGrid();
	void Bake(const std::vector<Model*>& objects);
	void Draw(Shader& shader);
	void Flood(glm::vec3 origin);
};