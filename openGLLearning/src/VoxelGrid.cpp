#include <glm/gtc/matrix_transform.hpp>
#include <Eigen/Dense>
#include <igl/read_triangle_mesh.h>
#include <igl/embree/EmbreeIntersector.h>
#include <igl/copyleft/cgal/mesh_boolean.h>
#include <igl/MeshBooleanType.h>
#include "VoxelGrid.h"

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


VoxelGrid::VoxelGrid():
	size(30, 5, 30),
	resolution(0.5),
	model(glm::translate(glm::mat4(1.0f), glm::vec3(-((size.x - 1.0) * resolution), 0.0, -((size.z - 1.0) * resolution)) / 2.0f)),
	voxelCount(size.x* size.y* size.z),
	status(voxelCount, 0)
{

}

void VoxelGrid::Bake(const std::vector<Model*>& objects)
{
	Eigen::MatrixXd V1; // Vertices voxel
	Eigen::MatrixXi F1; // Faces voxel
	igl::read_triangle_mesh("res/models/cube/cube.obj", V1, F1);

	for (int i = 0; i < objects.size(); i++)
	{
		Model* model = objects[i];
		std::string path = model->getPath();
		
		Eigen::MatrixXd V2; // Vertices
		Eigen::MatrixXi F2; // Faces
		igl::read_triangle_mesh(path, V2, F2);
		// to world
		glm::mat4 toWorld = model->GetModelMatrix();
		Eigen::MatrixXd toWorldEig = glmToEigen(toWorld);
		transformToWorldSpace(V2, toWorldEig);

		for (int j = 0; j < voxelCount; j++)
		{
			// to world
			float xPos = j % (int)size.z;
			float yPos = (j / (int)size.x) % (int)size.y;
			float zPos = int(j / ((int)size.x * (int)size.y));
			glm::vec3 offset = vec3(xPos, yPos, zPos);
			glm::mat4 toWorld = glm::mat4(1.0);
			toWorld = glm::translate(toWorld, offset);
			toWorld = glm::scale(toWorld, glm::vec3(resolution));
			Eigen::MatrixXd toWorldEig = glmToEigen(toWorld);
			transformToWorldSpace(V1, toWorldEig);

			// Check for collisions
			Eigen::MatrixXd VC; // Collision points
			Eigen::MatrixXi FC; // Indices of intersected triangles
			igl::copyleft::cgal::mesh_boolean(V1, F1, V2, F2, igl::MESH_BOOLEAN_TYPE_INTERSECT, VC, FC);

			// Check if there are collisions
			if (VC.rows() > 0) {
				std::cout << "Collision detected!" << std::endl;
				// Process collision data if needed
			}
			else {
				std::cout << "No collision detected." << std::endl;
			}
		}
	}
}
