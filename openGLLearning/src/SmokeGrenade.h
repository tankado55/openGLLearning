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
	glm::vec3 m_DetonationWorldPos;

public:
	SmokeGrenade();
	void Update(const double& deltaTime);
	void Draw(Shader& shader);
	float GetTimer() const { return m_Timer; };
	void Detonate(glm::vec3 pos);
};