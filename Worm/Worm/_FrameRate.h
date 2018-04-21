#ifndef _FRAMERATE_H
#define _FRAMERATE_H

#include <windows.h>

class _FrameRate
{
public:
	int lastTick;
	int lastFrameRate;
	int frameRate;

	int CalculateFrameRate();

	_FrameRate();
	~_FrameRate();
};

#endif