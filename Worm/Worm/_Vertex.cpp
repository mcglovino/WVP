#include "_Vertex.h"

_Vertex::_Vertex() {};

_Vertex::_Vertex(float x, float y, float z,
	float u, float v)
{
	XMFLOAT3 pos(x, y, z);
	XMFLOAT2 texCoord(u, v);
}
