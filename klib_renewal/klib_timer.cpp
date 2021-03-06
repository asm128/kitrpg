/// Copyright 2014-2017 - asm128
#include "klib_timer.h"
#include <chrono>

#if defined(GPK_WINDOWS)
#	include <windows.h>
#else
#	include <chrono>
#endif

void													klib::STimer::Reset								()									{
#if defined(GPK_WINDOWS)
	QueryPerformanceFrequency( (LARGE_INTEGER*)&CountsPerSecond );
#endif
	SecondsPerCount											= (1.0 / (CountsPerSecond ? CountsPerSecond : 1));
	CountsPerMicroSecond									= CountsPerSecond/1000000.0;
	if( 0 == CountsPerMicroSecond )
		CountsPerMicroSecond									= 1;
#if defined(GPK_WINDOWS)
	QueryPerformanceCounter( (LARGE_INTEGER*)&PrevTimeStamp );		
#else
	PrevTimeStamp											= ::std::chrono::duration_cast<::std::chrono::microseconds>(::std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#endif
	LastTimeMicroSeconds									= 0;
	LastTimeSeconds											= 0;

	FramesLastSecond										= 0;
}

void													klib::STimer::Frame								()									{
#if defined(GPK_WINDOWS)
	QueryPerformanceCounter( (LARGE_INTEGER*)&CurrentTimeStamp );
#else
	CurrentTimeStamp										= ::std::chrono::duration_cast<::std::chrono::microseconds>(::std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#endif
	int64_t														timeDifference									= (CurrentTimeStamp - PrevTimeStamp);
	LastTimeMicroSeconds									= (int64_t)(timeDifference/CountsPerMicroSecond);
	LastTimeSeconds											= timeDifference * SecondsPerCount;//LastTimeMicroSeconds*1000000;//(CurrentTimeStamp - PrevTimeStamp) * SecondsPerCount;
	PrevTimeStamp											= CurrentTimeStamp;

	++FramesThisSecond;
	FrameCounterSeconds										+= LastTimeSeconds;

	int32_t														totalFrames										= int32_t(FramesThisSecond	/ FrameCounterSeconds);
	int32_t														framesPerSecond									= int32_t(totalFrames		/ FrameCounterSeconds);
	while(FrameCounterSeconds >= 1.0) {
		FramesLastSecond										= framesPerSecond;
		FrameCounterSeconds										-= 1.0f;
		FramesThisSecond										= 0;
	}
}
