#include "_FrameRate.h"

int lastTick;
int lastFrameRate;
int frameRate;

int _FrameRate::CalculateFrameRate()
{
	if (GetTickCount() - lastTick >= 1000)
	{
		lastFrameRate = frameRate;
		frameRate = 0;
		lastTick = GetTickCount();
	}
	frameRate++;
	return lastFrameRate;
}

_FrameRate::_FrameRate()
{
}


_FrameRate::~_FrameRate()
{
}


