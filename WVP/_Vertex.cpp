#include "_Vertex.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

/*struct Vertex	//Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;

};*/

XMFLOAT3 pos;
XMFLOAT2 texCoord;

_Vertex::_Vertex() {};

_Vertex::_Vertex(float x, float y, float z,
	float u, float v)
{
	XMFLOAT3 pos(x, y, z);
	XMFLOAT2 texCoord(u, v);
}


_Vertex::~_Vertex()
{
}
