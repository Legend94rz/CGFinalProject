#pragma once
#include <D3DX10.h>
#include <vector>
#include "Common.h"
#include "Camera.h"
#include "Light.h"
class Ground
{
private:
	ID3D10Device* mDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	DWORD mNumRows;
	DWORD mNumCols;

	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10EffectTechnique* mTech;
	ID3D10ShaderResourceView* mGrassMapRV;
	ID3D10ShaderResourceView* mDefaultSpecMapRV;
	ID3D10Effect* mEffect;
	Light mParallelLight;
public:
	float Y(float x, float z);

	Ground(LPTSTR szEffectFile, LPTSTR szTexFile, ID3D10Device* device);
	~Ground();
	void draw();
};

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
		: pos(x, y, z), normal(nx, ny, nz), texC(u, v) {}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texC;
};