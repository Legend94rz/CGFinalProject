#pragma once
#include <D3DX10.h>
struct VertexForBox
{
	VertexForBox() {}
	VertexForBox(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
		: pos(x, y, z), normal(nx, ny, nz), texC(u, v) {}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texC;
};

struct VertexForLand
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXCOLOR   diffuse;
	D3DXCOLOR   spec; // (r, g, b, specPower);
};
