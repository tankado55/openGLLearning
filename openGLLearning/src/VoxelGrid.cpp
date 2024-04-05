#define NOMINMAX
#include <glm/gtc/matrix_transform.hpp>
#include <Eigen/Dense>
//#include <igl/read_triangle_mesh.h>
//#include <igl/copyleft/cgal/mesh_boolean.h>
//#include <igl/MeshBooleanType.h>
#include "VoxelGrid.h"

VoxelGrid::VoxelGrid() :
	size(30, 5, 30),
	resolution(0.5),
	modelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(-((size.x - 1.0) * resolution), 0.0, -((size.z - 1.0) * resolution)) / 2.0f)),
	voxelCount(size.x* size.y* size.z),
	status(voxelCount, 0)
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

	for (int i = 0; i < objects.size(); i++)
	{
		Model* model = objects[i];
		AABB aabbModel = model->GetAABB();

		for (int j = 0; j < voxelCount; j++)
		{
			// to world
			float xPos = j % (int)size.z;
			float yPos = (j / (int)size.x) % (int)size.y;
			float zPos = int(j / ((int)size.x * (int)size.y));
			glm::vec3 offset = glm::vec3(xPos, yPos, zPos);
			glm::mat4 toWorld = glm::mat4(1.0);
			toWorld = glm::translate(toWorld, offset);
			toWorld = glm::scale(toWorld, glm::vec3(resolution));
			toWorld = modelMatrix * toWorld;

			AABB aabbVoxel;
			aabbVoxel.min = toWorld * glm::vec4(-0.5, -0.5, -0.5, 1);
			aabbVoxel.max = toWorld * glm::vec4( 0.5,  0.5,  0.5, 1);
			// Check for collisions

			if (CheckAABBCollision(aabbModel.min, aabbModel.max, aabbVoxel.min, aabbVoxel.max))
			{
				status[j] = true;	
			}
		}
	}

	// GLBuffers
	glBindBuffer(GL_TEXTURE_BUFFER, tboID);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(unsigned) * voxelCount, &status[0], GL_STATIC_DRAW);
	glBindTexture(GL_TEXTURE_BUFFER, textureID);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, tboID);
}

void VoxelGrid::Draw(Shader& shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, textureID);

	shader.Bind();
	shader.SetUniform1i("voxelBuffer", 0);
}
