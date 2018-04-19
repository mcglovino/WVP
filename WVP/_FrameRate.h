#pragma once

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

