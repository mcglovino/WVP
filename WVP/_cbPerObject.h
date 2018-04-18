#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

class _cbPerObject
{
public:

	XMMATRIX  WVP;

	_cbPerObject();
	~_cbPerObject();
};

