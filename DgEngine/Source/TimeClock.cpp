////////////////////////////////////////////////////////////////////////////////
// Filename: TimeClock.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "TimeClock.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


unique_ptr<TimeClock> TimeClock::m_Instance = nullptr;

const unique_ptr<TimeClock>& TimeClock::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<TimeClock>(new TimeClock());
	}
	return m_Instance;
}

void TimeClock::Release() {
	m_Instance = nullptr;
}

TimeClock::TimeClock() {
	m_FrameTime = m_StartTime = CurrentTimeMillis();
	m_FrameCount = 0;
	m_Fps = 0.0f;
	m_DeltaTime = 0.0f;
}

TimeClock::~TimeClock() {

}

void TimeClock::ComputeFPS() {
	long long currentMillis = CurrentTimeMillis();
	float currentTime = currentMillis * 0.001f;
	float diff = currentTime-(m_FrameTime*0.001f);

	m_FrameCount++;

	if(diff>=1.0f) {
		m_FrameTime = currentMillis;
		m_Fps = (float) m_FrameCount;
		m_FrameCount = 0;
	}
}

float TimeClock::RunningTime() const {
	return (float) ((double) CurrentTimeMillis()-(double) m_StartTime)/1000.0f;
}

float TimeClock::FPS() const {
	return m_Fps;
}

void TimeClock::SetDeltaTime(float deltaTime) {
	m_DeltaTime = deltaTime;
}

float TimeClock::GetDeltaTime() const {
	return m_DeltaTime;
}
