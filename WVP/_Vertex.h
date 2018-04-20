#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

class _Vertex
{
public:
	XMFLOAT3 pos;
	XMFLOAT2 texCoord;

	_Vertex();
	_Vertex(float x, float y, float z,
		float u, float v);
};