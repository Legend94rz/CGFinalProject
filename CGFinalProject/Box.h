#pragma once
#include <D3DX10.h>
#include "Common.h"
#include "Camera.h"
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


struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}

	D3DXVECTOR3 pos;
	float pad1;      // not used
	D3DXVECTOR3 dir;
	float pad2;      // not used
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	D3DXVECTOR3 att;
	float spotPow;
};


class Box
{
private:
	ID3D10Device* mDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	DWORD mNumVertices;
	DWORD mNumFaces;

	Light mParallelLight;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10ShaderResourceView* mDiffuseMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;

	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;

	D3DXMATRIX mWVP;


public:
	Box(LPTSTR szEffectFile,LPTSTR szTexFile,ID3D10Device* device);
	~Box();
	void draw();
};

