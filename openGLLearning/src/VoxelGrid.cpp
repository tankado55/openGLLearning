#include <glm/gtc/matrix_transform.hpp>
#include <Eigen/Dense>
#include "VoxelGrid.h"

static Eigen::MatrixXd glmToEigen(const glm::mat3& glmMatrix) {
	Eigen::MatrixXd eigenMatrix(3, 3);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
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
	// Construct BVHs
	igl::embree::EmbreeIntersector embree;
	embree.init(V1, F1);

	for (int i = 0; i < objects.size(); i++)
	{
		Model* model = objects[i];
		std::string path = model->getPath();
		
		Eigen::MatrixXd V2; // Vertices
		Eigen::MatrixXi F2; // Faces
		igl::read_triangle_mesh(path, V2, F2);
		// to world
		
		embree.init(V2, F2);

		for (int j = 0; j < voxelCount; j++)
		{
			// to world

			// Check for collisions
			Eigen::MatrixXd C; // Collision points
			Eigen::MatrixXi CI; // Indices of intersected triangles
			embree_triangle_mesh_intersection(V1, F1, V2, F2, C, CI);

			// Check if there are collisions
			if (C.rows() > 0) {
				std::cout << "Collision detected!" << std::endl;
				// Process collision data if needed
			}
			else {
				std::cout << "No collision detected." << std::endl;
			}
		}
	}
}
