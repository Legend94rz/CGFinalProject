#include "Sky.h"
unsigned Sky::t = 0;
void Sky::initFX(ID3D10Device * &device, LPTSTR szEffectName)
{
	HR(D3DX10CreateEffectFromFile(szEffectName, 0, 0, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, device, 0, 0, &mpEffect, NULL, 0));
	mpTech = mpEffect->GetTechniqueByName("SkyTech");
	mpWVP = mpEffect->GetVariableByName("gWVP")->AsMatrix();
	mpCubeTex = mpEffect->GetVariableByName("gCubeMap")->AsShaderResource();
}
void Sky::initVertexLayout()
{
	D3D10_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D10_INPUT_PER_VERTEX_DATA,0 }
	};
	int numDesc = sizeof(layout) / sizeof(layout[0]);

	D3D10_PASS_DESC pd;
	mpTech->GetPassByIndex(0)->GetDesc(&pd);
	HR(mpDevice->CreateInputLayout(layout, numDesc, pd.pIAInputSignature, pd.IAInputSignatureSize, &mpLayout));
}
void Sky::initVertexBuffer()
{
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
	HR(mpDevice->CreateBuffer(&bd, &InitData, &mpVertexBuffer));
}
void Sky::initIndexBuffer()
{
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

	D3D10_BUFFER_DESC ibd;
	ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(index);
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	D3D10_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = &index[0];
	HR(mpDevice->CreateBuffer(&ibd, &iInitData, &mpIndexBuffer));
}
Sky::Sky(LPTSTR szEffectName, LPTSTR szTextureName, ID3D10Device* device)
{
	mpDevice = device;

	HR(D3DX10CreateShaderResourceViewFromFile(device, szTextureName, 0, 0, &mpCubeTexView, 0));
	initFX(device, szEffectName);
	initVertexLayout();
	initVertexBuffer();
	initIndexBuffer();
}
void Sky::draw()
{
	D3DXMATRIX World, View, Proj, WVP;
	D3DXMatrixIdentity(&World);
	WVP = World*GetCamera().view()*GetCamera().proj();

	HR(mpWVP->SetMatrix((float*)&WVP));
	HR(mpCubeTex->SetResource(mpCubeTexView));

	UINT stride = sizeof(SkyBoxVertex);
	UINT offset = 0;

	mpDevice->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
	mpDevice->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	mpDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mpDevice->IASetInputLayout(mpLayout);

	D3D10_TECHNIQUE_DESC tdc;
	mpTech->GetDesc(&tdc);
	for (UINT i = 0; i<tdc.Passes; i++)
	{
		ID3D10EffectPass* pass = mpTech->GetPassByIndex(i);
		pass->Apply(0);
		mpDevice->DrawIndexed(mNumIndices, 0, 0);
	}

}

