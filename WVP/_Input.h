#ifndef _INPUT_H
#define _INPUT_H

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>


class _Input
{
public:
	_Input();
	~_Input();

	bool Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight);
	bool Update();

	bool IsKeyDown(unsigned int key);
	bool IsKeyHit(unsigned int key);

private:
	const static int NumKeys = 256;

	bool ReadKeyboard();

	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;

	bool keys[NumKeys];
	bool prevKeys[NumKeys];
};

#endif