#include "SmokeGrenade.h"

SmokeGrenade::SmokeGrenade() :
	m_ExpandTime(2.0),
	m_Timer(0.0)
{

}

void SmokeGrenade::UpdateTimer(float deltaTime)
{
	m_Timer += deltaTime;
}
