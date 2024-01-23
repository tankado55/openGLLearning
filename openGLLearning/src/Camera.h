#pragma once

#include "Renderer.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/ext/matrix_transform.hpp>

const glm::vec3 WorldUp(0, 1, 0);

class Camera
{
private:
	glm::vec3 m_CameraPos;
	glm::vec3 m_CameraTarget;
	glm::vec3 m_CameraFront;
	glm::vec3 m_CameraUp;
	glm::vec3 m_CameraRight;
	glm::mat4 m_View;

	//mouse
	float u_Yaw, u_Pitch;
	float lastX, lastY;
	const float u_MouseSensitivity = 0.1f;

	const float u_CameraSpeed = 10.00f;

public:
	Camera();
	~Camera();
	void ProcessKeyboardInput(float deltaTime, GLFWwindow* window);
	void ProcessMouseInput(float xoffset, float yoffset, GLboolean constrainPitch = true);
	glm::mat4 GetView() const;
	glm::vec3 GetPos() const;

private:
	void updateCameraVectors();
};

