#include <glm/gtc/matrix_transform.hpp>
#include "VoxelGrid.h"

VoxelGrid::VoxelGrid():
	size(30.0, 10.0, 30.0),
	resolution(0.5),
	model(glm::translate(glm::mat4(1.0f), glm::vec3(-size.x, 0.0, -size.z))),
	status(size.x * size.y * size.z * resolution)
{

}