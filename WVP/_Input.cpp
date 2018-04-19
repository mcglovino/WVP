#include "_Input.h"

//#pragma comment (lib, "dinput8.lib")
//#pragma comment (lib, "dxguid.lib")
//#include <dinput.h>

_Input::_Input()
{
	directInput = 0;
	keyboard = 0;
	//check to see if can removve this, mabye not necessary v
	memset(keys, 0, sizeof(bool)*NumKeys);
}


_Input::~_Input()
{
	//Release the keyboard
	if (keyboard)
	{
		keyboard->Unacquire();
		keyboard->Release();
		keyboard = 0;
	}

	//Release the main interface to direct input
	if (directInput) {
		directInput->Release();
		directInput = 0;
	}
}


bool _Input::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	screenWidth = screenWidth;
	screenHeight = screenHeight;

	//initialize direct input interface
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//initialize keyboard interface
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//set data format for keyboard
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	//Stops keyboard sharing with other programs
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	result = keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool _Input::Update()
{
	memcpy(prevKeys, keys, sizeof(bool)* NumKeys);

	bool result;

	//read keyboard
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	return true;
}

bool _Input::IsKeyDown(unsigned int key)
{
	return keys[key];
}

bool _Input::IsKeyHit(unsigned int key)
{
	return keys[key] && !prevKeys[key];
}

bool _Input::ReadKeyboard()
{
	HRESULT result;
	
	//read keyboard device
	result = keyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}