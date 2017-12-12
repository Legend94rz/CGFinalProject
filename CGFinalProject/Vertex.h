#pragma once
#include <D3DX10.h>

struct Vertex {
	Vertex() {}
	Vertex(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
		: pos(x, y, z), normal(nx, ny, nz), texC(u, v) {}
	D3DXVECTOR3 pos;	//0
	D3DXVECTOR3 normal;	//12
	D3DXCOLOR   diffuse;//24
	D3DXCOLOR   spec;	//40 (r, g, b, specPower);
	D3DXVECTOR2 texC;	//56
};