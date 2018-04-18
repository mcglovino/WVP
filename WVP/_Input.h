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
	void GetMousePosition(int& x, int& y);

private:
	const static int NumKeys = 256;

	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;
	DIMOUSESTATE mouseState;

	bool keys[NumKeys];
	bool prevKeys[NumKeys];

	int screenWidth;
	int screenHeight;
	int mouseX;
	int mouseY;
};

#endif