#pragma once

#include "vector"
#include "glm/glm.hpp"

class VoxelGrid
{
public:
	glm::vec3 size;
	float resolution;
	glm::mat4 model;
	std::vector<unsigned> status;

public:
	VoxelGrid();
};