#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <String>

#include "_Vertex.h"
#include "_cbPerObject.h"

class _Object
{
public:



	_Object(LPCWSTR texT, std::string modT);

	//~_Object();

	void Init();
	void Update();
	void Draw();
	void loadModel();
	void setRot(float X, float Y, float Z, float rotspeedT);
	void setTranslate(float X, float Y, float Z);
};

