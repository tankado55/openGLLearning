#include "SmokeGrenade.h"
#include "InputManager.h"

SmokeGrenade::SmokeGrenade() :
	m_ExpandTime(1.5),
	m_Timer(0.0),
	m_PrevLeftButtonState(false),
	m_LeftButtonState(false),
	m_IsDetoned(false),
	m_Ellipsoid(glm::vec3(2.1, 1.8, 2.1)),
	m_MaxDistance(m_Ellipsoid.x),
	m_DetonationPos(0.0,0.0,0.0)
{

}

void SmokeGrenade::Update(const double& deltaTime)
{
	//if (m_IsDetoned)
		m_Timer += deltaTime;
	if (m_Timer > m_ExpandTime)
	{
		//m_Timer = m_ExpandTime;
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
	float x;
	x = m_Timer / m_ExpandTime;
	if (x >= 0 and x <= 0.5)
	{
		x = 2 * (x * x);
	}
	else
	{
		x = 1 - (1 / (5 * (2 * x - 0.8) + 1));
	}
	glm::vec3 currentElli(m_Ellipsoid.x * x, m_Ellipsoid.y * x, m_Ellipsoid.z * x);
	shader.SetUniformVec3f("u_Ellipsoid", currentElli);
	shader.SetUniformVec3f("explosionPos", m_DetonationPos);
}

void SmokeGrenade::Detonate(glm::vec3 pos)
{
	m_DetonationPos = pos;
}
