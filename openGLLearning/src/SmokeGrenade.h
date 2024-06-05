#pragma once

#include "Shader.h"

class SmokeGrenade
{
private:
	float m_ExpandTime;
	float m_Timer;
	bool m_IsDetoned;
	glm::vec3 m_DetonationWorldPos;
	glm::vec3 m_Color;
	glm::vec3 m_Ellipsoid;

public:
	SmokeGrenade();
	void Update(const double& deltaTime);
	void SetUniforms(Shader& shader);
	float GetTimer() const { return m_Timer; };
	void Detonate(glm::vec3 pos);
	glm::vec3 GetColor(){ return m_Color; }
	glm::vec3* GetEllipsoidPtr() { return &m_Ellipsoid; }
};