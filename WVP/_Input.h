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

	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	DIMOUSESTATE m_mouseState;

	bool m_keys[NumKeys];
	bool m_prevKeys[NumKeys];

	int m_screenWidth;
	int m_screenHeight;
	int m_mouseX;
	int m_mouseY;
};

#endif