#pragma once
#include <D3DX10.h>
#include <vector>
#include "Common.h"
#include "Vertex.h"
class Land
{
public:
	Land(ID3D10Device* device, int m, int n);
	~Land();
	float Y(float x,float z);
	void draw();
private:
	DWORD mNumRows;
	DWORD mNumCols;

	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
};

