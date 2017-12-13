#pragma once
#include <Windows.h>
#include <D3DX10.h>
#include "Common.h"
#include "Camera.h"

typedef struct {
	D3DXVECTOR3 pos;
}SkyBoxVertex;

class Sky
{
public:
	static const int radius = 2000;
	static unsigned t;

public:
	Sky(LPTSTR szEffectName, LPTSTR szTextureName, ID3D10Device* device);
	~Sky(void) {};
	void draw();
public:
	ID3D10Device* mpDevice;
	ID3D10EffectMatrixVariable* mpWVP;

	ID3D10EffectShaderResourceVariable* mpCubeTex;
	ID3D10ShaderResourceView* mpCubeTexView;

	ID3D10Effect* mpEffect;
	ID3D10EffectTechnique* mpTech;
	ID3D10InputLayout* mpLayout;
	ID3D10Buffer* mpVertexBuffer;
	ID3D10Buffer* mpIndexBuffer;
private:
	HRESULT hr;
	int mNumIndices;
	void initFX(ID3D10Device * &device, LPTSTR szTextureName);
	void initVertexLayout();
	void initVertexBuffer();
	void initIndexBuffer();

};
