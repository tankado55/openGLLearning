#include "SmokeGrenade.h"
#include "InputManager.h"

SmokeGrenade::SmokeGrenade() :
	m_ExpandTime(1.2),
	m_Timer(0.0),
	m_IsDetoned(false),
	m_Ellipsoid(glm::vec3(2.5, 2.0, 2.5)),
	m_MaxDistance(m_Ellipsoid.x),
	m_DetonationWorldPos(0.0,0.0,0.0),
	m_Color(0.05,0.05,0.05)
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
}

void SmokeGrenade::SetUniforms(Shader& shader)
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
	shader.SetUniformVec3f("u_Radius", currentElli);
	shader.SetUniformVec3f("explosionPos", m_DetonationWorldPos);
}

void SmokeGrenade::Detonate(glm::vec3 pos)
{
	m_DetonationWorldPos = pos;
	m_IsDetoned = true;
	m_Timer = 0.0;
}
