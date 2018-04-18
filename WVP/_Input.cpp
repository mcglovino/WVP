#include "_Input.h"

//#pragma comment (lib, "dinput8.lib")
//#pragma comment (lib, "dxguid.lib")
//#include <dinput.h>

_Input::_Input()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
	//check to see if can removve this, mabye not necessary v
	memset(m_keys, 0, sizeof(bool)*NumKeys);
}


_Input::~_Input()
{
	//Release the mouse
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	//Release the keyboard
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	//Release the main interface to direct input
	if (m_directInput) {
		m_directInput->Release();
		m_directInput = 0;
	}
}


bool _Input::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_mouseX = 0;
	m_mouseY = 0;

	//initialize direct input interface
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//initialize keyboard interface
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//set data format for keyboard
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	//Stops keyboard sharing with other programs
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	//initialize mouse interface
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//set data format for the mouse
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	//Stops mouse sharing with other programs
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	//Acquire mouse
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool _Input::Update()
{
	memcpy(m_prevKeys, m_keys, sizeof(bool)* NumKeys);

	bool result;

	//read keyboard
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	//read mouse
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	//update mouse position
	ProcessInput();

	return true;
}

bool _Input::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}

bool _Input::IsKeyHit(unsigned int key)
{
	return m_keys[key] && !m_prevKeys[key];
}

void _Input::GetMousePosition(int& x, int&y)
{
	x = m_mouseX;
	y = m_mouseY;
}

bool _Input::ReadKeyboard()
{
	HRESULT result;
	
	//read keyboard device
	result = m_keyboard->GetDeviceState(sizeof(m_keys), (LPVOID)&m_keys);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool _Input::ReadMouse()
{
	HRESULT result;

	//read mouse device
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}
	
	return false;
}

void _Input::ProcessInput()
{
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	if (m_mouseX < 0)
		m_mouseX = 0;

	if (m_mouseX > m_screenWidth)
		m_mouseX = m_screenWidth;

	if (m_mouseY < 0)
		m_mouseY = 0;

	if (m_mouseY > m_screenHeight)
		m_mouseY = m_screenHeight;

}