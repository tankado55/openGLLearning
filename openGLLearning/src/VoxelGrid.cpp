#define NOMINMAX
#include <glm/gtc/matrix_transform.hpp>
#include <Eigen/Dense>
//#include <igl/read_triangle_mesh.h>
//#include <igl/copyleft/cgal/mesh_boolean.h>
//#include <igl/MeshBooleanType.h>
#include <iostream>
#include "VoxelGrid.h"

VoxelGrid::VoxelGrid() :
	resolution(60, 10, 60),
	voxelSize(0.5),
	modelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(-(resolution.x - 1.0), 1.0, -(resolution.z - 1.0)) / 2.0f)),
	voxelCount(resolution.x* resolution.y* resolution.z),
	status(voxelCount, 0),
	bakedStatus(voxelCount)
{
}

static Eigen::MatrixXd glmToEigen(const glm::mat4& glmMatrix) {
	Eigen::MatrixXd eigenMatrix(4, 4);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			eigenMatrix(i, j) = glmMatrix[i][j];
		}
	}
	return eigenMatrix;
}

static glm::mat3 eigenToGlm(const Eigen::MatrixXd& eigenMatrix) {
	glm::mat3 glmMatrix;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			glmMatrix[i][j] = eigenMatrix(i, j);
		}
	}
	return glmMatrix;
}

static void transformToWorldSpace(Eigen::MatrixXd& V, const Eigen::Matrix4d& M) {
	// Add homogeneous coordinate to vertices
	Eigen::MatrixXd V_homogeneous(V.rows(), 4);
	V_homogeneous << V, Eigen::MatrixXd::Ones(V.rows(), 1);

	// Transform vertices to world space
	V_homogeneous = (M * V_homogeneous.transpose()).transpose();

	// Update vertices
	V = V_homogeneous.leftCols<3>(); // Discard the last column (homogeneous coordinates)
}

bool CheckAABBCollision(const glm::vec3& minA, const glm::vec3& maxA, const glm::vec3& minB, const glm::vec3& maxB) {
	return glm::all(glm::lessThanEqual(minA, maxB)) && glm::all(glm::lessThanEqual(minB, maxA));
}

void VoxelGrid::Bake(const std::vector<Model*>& objects)
{
	// 1.0f obstacle
	// 2.0f smoke
	for (int i = 0; i < objects.size(); i++)
	{
		Model* model = objects[i];
		AABB aabbModel = model->GetAABB();

		for (int j = 0; j < voxelCount; j++)
		{
			// to world
			float xPos = j % (int)resolution.z;
			float yPos = (j / (int)resolution.x) % (int)resolution.y;
			float zPos = int(j / ((int)resolution.x * (int)resolution.y));
			glm::vec3 offset = glm::vec3(xPos, yPos, zPos);
			glm::mat4 toWorld = glm::mat4(1.0);
			toWorld = glm::scale(toWorld, glm::vec3(voxelSize));

			AABB aabbVoxel;
			aabbVoxel.min = toWorld * modelMatrix * (glm::vec4(-0.5, -0.5, -0.5, 1) + glm::vec4(offset, 0.0));
			aabbVoxel.max = toWorld * modelMatrix * (glm::vec4( 0.5,  0.5,  0.5, 1) + glm::vec4(offset, 0.0));
			// Check for collisions

			if (CheckAABBCollision(aabbModel.min, aabbModel.max, aabbVoxel.min, aabbVoxel.max))
			{
				bakedStatus[j] = -1.0f;	
			}
			if (j == 0) {
				bakedStatus[j] = 1.0f;
			}
			if (j == voxelCount-1) {
				bakedStatus[j] = 1.0f;
			}
		}
	}
	status = bakedStatus;

	// GLBuffers
	glGenBuffers(1, &tboID);
	glBindBuffer(GL_TEXTURE_BUFFER, tboID);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * voxelCount, &status[0], GL_STATIC_DRAW);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_BUFFER, textureID);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, tboID);
}

void VoxelGrid::Draw(Shader& shader)
{
	glBindBuffer(GL_TEXTURE_BUFFER, tboID);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * voxelCount, &status[0], GL_STATIC_DRAW);
	shader.Bind();
	shader.SetUniform1i("voxelBuffer", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, textureID);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, tboID);
}

void VoxelGrid::BindBufferToTexture(Shader& shader)
{
	glBindBuffer(GL_TEXTURE_BUFFER, tboID);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * voxelCount, &status[0], GL_STATIC_DRAW);
	shader.Bind();
	shader.SetUniform1i("voxelBuffer", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, textureID);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, tboID);
}

glm::vec3 VoxelGrid::IndexToWorld(int j)
{
	// to world
	float xPos = j % (int)resolution.z;
	float yPos = (j / (int)resolution.x) % (int)resolution.y;
	float zPos = int(j / ((int)resolution.x * (int)resolution.y));
	glm::vec3 offset = glm::vec3(xPos, yPos, zPos);
	glm::mat4 toWorld = glm::mat4(1.0);
	toWorld = glm::scale(toWorld, glm::vec3(voxelSize));

	glm::vec3 result;
	result = toWorld * modelMatrix * (glm::vec4(offset, 1.0));
	return result;
}

bool VoxelGrid::indexIsValid(int i) {
	return (i >= 0 && i < status.size());
}

std::vector<int> VoxelGrid::GetNeighbors(int i)
{
	std::vector<int> neighbors;

	float x = i % (int)resolution.z;
	float y = (i / (int)resolution.x) % (int)resolution.y;
	float z = int(i / ((int)resolution.x * (int)resolution.y));

	// Define 26 possible directions in 3D space
	int dx[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 }; //27
	int dy[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1, -1, -1, -1, 0, 0, 0, 1, 1, 1, -1, -1, -1, 0, 0, 0, 1, 1, 1 };
	int dz[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1 };

	for (int i = 0; i < 27; i++) {
		int newX = x + dx[i];
		int newY = y + dy[i];
		int newZ = z + dz[i];

		if (newX >= 0 && newX < resolution.x && newY >= 0 && newY < resolution.y && newZ >= 0 && newZ < resolution.z) {
			int newIndex = newX + (newY * resolution.x) + (newZ * resolution.x * resolution.y);
			neighbors.push_back(newIndex);
		}
	}

	return neighbors;
}

void VoxelGrid::Flood(int i)
{
	if (!indexIsValid(i)) return;
	if (status[i] == -1.0) return;

	float maxNeigh = 0.0;
	std::vector<int> neighbors = GetNeighbors(i);
	for (auto neigh : neighbors)
	{
		maxNeigh = std::max(maxNeigh, status[neigh]);
	}

	status[i] = maxNeigh - 1.0f;


	for (auto neigh : neighbors)
	{
		if (status[neigh] < status[i] - 1.0)
		{
			Flood(neigh);
		}
	}




}

void VoxelGrid::Flood(glm::vec3 origin, int gas)
{
	// I need the index vc 
	// from world to local
	glm::mat4 toLocal = glm::mat4(1.0);
	toLocal = glm::scale(toLocal, glm::vec3(1.0/voxelSize));
	glm::vec3 localOrigin = glm::inverse(modelMatrix) * toLocal * glm::vec4(origin, 1.0);
	int index1D = int(localOrigin.x) + (int(localOrigin.y) * resolution.x) + (int(localOrigin.z) * resolution.x * resolution.y);
	glm::vec3 inverseTest = IndexToWorld(index1D); // debug
	if (!indexIsValid(index1D))
	{
		std::cout << "Smoke origin index is out of the scene" << std::endl;
		return;
	}
	if (status[index1D] == -1.0f)
	{
		std::cout << "smoke inside wall" << std::endl;
		return;
	}

	status[index1D] = gas;

	std::vector<int> neighbors = GetNeighbors(index1D);
	for (auto neigh : neighbors)
	{
		Flood(neigh);
	}
}

void VoxelGrid::ClearStatus()
{
	status = bakedStatus;
}

glm::mat4 VoxelGrid::GetToVoxelLocal()
{
	glm::mat4 toLocal = glm::mat4(1.0);
	toLocal = glm::scale(toLocal, glm::vec3(1.0 / voxelSize));
	glm::mat4 result = glm::inverse(modelMatrix) * toLocal;
	return result;
}


