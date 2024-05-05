#include "SmokeGrenade.h"
#include "InputManager.h"

SmokeGrenade::SmokeGrenade() :
	m_ExpandTime(2.0),
	m_Timer(0.0),
	m_PrevLeftButtonState(false),
	m_LeftButtonState(false),
	m_IsDetoned(false),
	m_Ellipsoid(glm::vec3(3.0, 2.0, 3.0)),
	m_MaxDistance(m_Ellipsoid.x),
	m_DetonationPos(0.0,0.0,0.0)
{

}

void SmokeGrenade::Update(const double& deltaTime)
{
	if (m_IsDetoned)
		m_Timer += deltaTime;
	if (m_Timer > m_ExpandTime)
	{
		m_IsDetoned = false;
	}

	GLFWwindow* window = InputManager::GetInstance()->GetWindow();

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS) //TODO: refactoring
	{
		m_PrevLeftButtonState = m_LeftButtonState;
		m_LeftButtonState = true;
		if (m_PrevLeftButtonState == false && m_LeftButtonState == true)
		{
			m_IsDetoned = true;
			m_Timer = 0.0;
		}
	}
	else
	{
		m_PrevLeftButtonState = m_LeftButtonState;
		m_LeftButtonState = false;
	}
}

void SmokeGrenade::Draw(Shader& shader) //TODO: refactor
{
	//if (m_Timer == 0.0) return;
	shader.Bind();
	float currentMaxDistance;
	currentMaxDistance = m_MaxDistance / m_ExpandTime * m_Timer;
	glm::vec3 currentElli(std::min(m_Ellipsoid.x, currentMaxDistance), std::min(m_Ellipsoid.y, currentMaxDistance), std::min(m_Ellipsoid.z, currentMaxDistance));
	shader.SetUniformVec3f("u_Ellipsoid", currentElli);
	shader.SetUniformVec3f("explosionPos", m_DetonationPos);
}

void SmokeGrenade::Detonate(glm::vec3 pos)
{
	m_DetonationPos = pos;
}
