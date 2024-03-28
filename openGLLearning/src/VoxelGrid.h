#pragma once

#include "vector"
#include "glm/glm.hpp"
#include "Model.h"

class VoxelGrid
{
public:
	glm::vec3 size;
	float resolution;
	glm::mat4 model;
	unsigned voxelCount;
	std::vector<unsigned> status;

public:
	VoxelGrid();
	void Bake(const std::vector<Model*>& objects);
};