#pragma once
#include <D3DX10.h>
#include "Camera.h"
class Sky
{
private:
	ID3D10Device* mDevice;
	ID3D10EffectMatrixVariable* mWVP;
	ID3D10ShaderResourceView* mCubeTexView;

	ID3D10Effect* mEffect;
	ID3D10InputLayout* mLayout;
	ID3D10Buffer* mIndexBuffer;
	ID3D10Buffer* mVertexBuffer;
	int mNumIndices;
	ID3D10EffectTechnique* mTech;

public:
	Sky();
	~Sky();
	void draw();
	Sky(LPTSTR szEffeectFile, LPTSTR szTexFile, ID3D10Device* device);
	static const int radius = 2000;

};

