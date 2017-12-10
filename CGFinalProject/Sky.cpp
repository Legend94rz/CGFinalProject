#include "Sky.h"

Sky::Sky()
{
}


Sky::~Sky()
{
}

void Sky::draw()
{
	D3DXMATRIX World, View, Proj, WVP;
	D3DXMatrixIdentity(&World);
	WVP = World*GetCamera().view()*GetCamera().proj();


	mWVP = mEffect->GetVariableByName("gWVP")->AsMatrix();
	ID3D10EffectShaderResourceVariable* cubeTex = mEffect->GetVariableByName("gCubeMap")->AsShaderResource();

	mWVP->SetMatrix((float*)&WVP);
	cubeTex->SetResource(mCubeTexView);

	UINT stride = sizeof(D3DXVECTOR3);
	UINT offset = 0;

	mDevice->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	mDevice->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDevice->IASetInputLayout(mLayout);				//应当在画之前应用顶点布局,否则绘图不正确。血的教训！！！

	D3D10_TECHNIQUE_DESC tdc;
	mTech->GetDesc(&tdc);
	for (UINT i = 0; i<tdc.Passes; i++)
	{
		ID3D10EffectPass* pass = mTech->GetPassByIndex(i);
		pass->Apply(0);
		mDevice->DrawIndexed(mNumIndices, 0, 0);
	}

}

Sky::Sky(LPTSTR szEffectFile, LPTSTR szTexFile, ID3D10Device * device)
{
	mDevice = device;
	HRESULT hr;

	hr = D3DX10CreateShaderResourceViewFromFile(device, szTexFile, NULL, NULL, &mCubeTexView, NULL);
	hr = D3DX10CreateEffectFromFile(szEffectFile, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, device, NULL, NULL, &mEffect, NULL, 0);

	D3D10_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D10_INPUT_PER_VERTEX_DATA,0 }
	};
	int numDesc = sizeof(layout) / sizeof(layout[0]);
	mTech = mEffect->GetTechniqueByName("SkyTech");
	D3D10_PASS_DESC pd;
	mTech->GetPassByIndex(0)->GetDesc(&pd);
	hr = mDevice->CreateInputLayout(layout, numDesc, pd.pIAInputSignature, pd.IAInputSignatureSize, &mLayout);

	D3DXVECTOR3 vertex[] = {
		D3DXVECTOR3(-1,1,-1),
		D3DXVECTOR3(1,1,-1),
		D3DXVECTOR3(1,1,1),
		D3DXVECTOR3(-1,1,1),

		D3DXVECTOR3(-1,-1,-1),
		D3DXVECTOR3(1,-1,-1),
		D3DXVECTOR3(1,-1,1),
		D3DXVECTOR3(-1,-1,1)
	};
	for (int i = 0; i<8; i++)
	{
		vertex[i] = radius*vertex[i];
	}
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(vertex);
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vertex[0];
	hr = mDevice->CreateBuffer(&bd, &InitData, &mVertexBuffer);

	DWORD index[] = {
		1,3,2,
		1,0,3,		//UP.jpg

		0,4,5,
		1,0,5,		//left.jpg

		3,4,0,
		3,4,7,		//front.jpg

		1,6,5,
		1,2,6,		//back.jpg

		2,6,7,
		2,7,3,		//right.jpg

		6,4,5,
		7,4,6		//down.jpg
	};
	mNumIndices = sizeof(index) / sizeof(index[0]);

	bd.Usage = D3D10_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(index);
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	InitData.pSysMem = &index[0];
	hr = mDevice->CreateBuffer(&bd, &InitData, &mIndexBuffer);
}
