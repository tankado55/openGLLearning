#pragma once

class SmokeGrenade
{
private:
	float m_ExpandTime;
	float m_Timer;

public:
	SmokeGrenade();
	void UpdateTimer(float deltaTime);
	float GetTimer() const { return m_Timer; };
};