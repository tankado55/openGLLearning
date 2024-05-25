#pragma once

#include "vector"
#include "glm/glm.hpp"
#include "Model.h"

class VoxelGrid
{

public:
	glm::vec3 resolution;
	float voxelSize;
	glm::mat4 modelMatrix;
	unsigned voxelCount;
	std::vector<float> status;
	std::vector<float> bakedStatus;
	GLuint tboID;
	GLuint textureID;

public:
	VoxelGrid();
	void Bake(const std::vector<Model*>& objects);
	void Draw(Shader& shader);
	void BindBufferToTexture(Shader& shader);
	glm::vec3 IndexToWorld(int j);
	bool indexIsValid(int i);
	std::vector<int> GetNeighbors(int i);
	void Flood(int i, glm::vec3 origin, glm::vec3 radius);
	void Flood(glm::vec3 origin, glm::vec3 radius, int gas);
	void ClearStatus();
	glm::mat4 GetToVoxelLocal();
	glm::vec3 GetResolution() const { return resolution; }
	glm::vec3 GetBounds() const;
};