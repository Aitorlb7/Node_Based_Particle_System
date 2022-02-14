#ifndef __TIMER_H__
#define __TIMER_H__

typedef unsigned __int32 uint32;

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Resume();
	void Stop();
	void Pause();
	void Restart();



	uint32 Read();
	float ReadSec();
	
	bool running;
	uint32	started_at;
	uint32	stopped_at;
	uint32	paused_at;
	uint32	resumed_at;
	uint32	time;

};

#endif //__TIMER_H__