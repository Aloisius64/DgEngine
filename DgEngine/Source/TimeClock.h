////////////////////////////////////////////////////////////////////////////////
// Filename: TimeClock.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <memory>
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

///////////////////////
// MY CLASS INCLUDES //
///////////////////////

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class TimeClock {
	public:
		inline static long long CurrentTimeMillis();
		static const unique_ptr<TimeClock>& Instance();
		static void Release();

	public:
		virtual ~TimeClock();

		void ComputeFPS();
		float RunningTime() const;
		float FPS() const;
		void SetDeltaTime(float);
		float GetDeltaTime() const;

	private:
		TimeClock();

	private:
		static unique_ptr<TimeClock>	m_Instance;
		long long						m_FrameTime;
		long long						m_StartTime;
		int								m_FrameCount;
		float							m_Fps;
		float							m_DeltaTime;
	};

	long long TimeClock::CurrentTimeMillis() {
#ifdef WIN32    
		return GetTickCount();
#else
		timeval t;
		gettimeofday(&t, NULL);
		long long ret = t.tv_sec*1000+t.tv_usec/1000;
		return ret;
#endif    
	}
}
