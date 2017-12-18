#pragma once
#include <D3DX10.h>
#include <vector>
#include "Common.h"
#include "Vertex.h"
#include "Camera.h"
#include "Light.h"
class Land
{
public:
	Land(ID3D10Device* device, int m, int n,Light light);
	~Land();
	float Y(float x,float z);
	void draw();
private:
	void initFX();
	void initVertexLayout();
	void initIndexBuffer(int m, int n);
	void initVertexBuffer(int n, float dx, int m);

	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;

	ID3D10InputLayout* mVertexLayout;

	Light mGlobalLight;
	D3DXMATRIX mWVP;
	ID3D10ShaderResourceView* mGrassMapRV;
	ID3D10ShaderResourceView* mDefaultSpecMapRV;

};

