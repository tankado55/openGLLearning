#pragma once

#include "Shader.h"

class SmokeGrenade
{
private:
	float m_ExpandTime;
	float m_Timer;
	bool m_PrevLeftButtonState;
	bool m_LeftButtonState;
	bool m_IsDetoned;
	glm::vec3 m_Ellipsoid;
	float m_MaxDistance;

public:
	SmokeGrenade();
	void Update(const double& deltaTime, int itest);
	void Draw(Shader& shader);
	float GetTimer() const { return m_Timer; };
};