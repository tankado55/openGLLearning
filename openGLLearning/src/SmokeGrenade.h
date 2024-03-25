#pragma once

class SmokeGrenade
{
private:
	float m_ExpandTime;
	float m_Timer;

public:
	void UpdateTimer(float deltaTime);
	float GetTimer() const { return m_Timer };
};