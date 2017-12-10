#include "Ground.h"



float Ground::Y(float x, float z)
{
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

Ground::Ground(LPTSTR szEffectFile, LPTSTR szTexFile, ID3D10Device* device)
{
	int m = mNumRows = 129;
	int n = mNumCols = 129;
	mNumVertices = n * m;
	mNumFaces = (n - 1)*(m - 1) * 2;
	mDevice = device;
	float dx = 1.0;
	HRESULT hr;

	// Create the geometry and fill the vertex buffer. 
	std::vector<Vertex> vertices(mNumVertices);
	float halfWidth = (n - 1)*dx*0.5f;
	float halfDepth = (m - 1)*dx*0.5f;
	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);
	for (DWORD i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dx;
		for (DWORD j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			// Graph of this function looks like a mountain range.
			float y = Y(x, z);

			vertices[i*n + j].pos = D3DXVECTOR3(x, y, z);

			// Stretch texture over grid.
			vertices[i*n + j].texC.x = j*du;
			vertices[i*n + j].texC.y = i*dv;

			// n = (-df/dx, 1, -df/dz)
			D3DXVECTOR3 normal;
			normal.x = -0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z);
			normal.y = 1.0f;
			normal.z = -0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z);
			D3DXVec3Normalize(&vertices[i*n + j].normal, &normal);
		}
	}
	D3D10_BUFFER_DESC vbd;
	vbd.Usage = D3D10_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
	vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(mDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	// Create the index buffer.  The index buffer is fixed, so we only 
	// need to create and set once.

	std::vector<DWORD> indices(mNumFaces * 3); // 3 indices per face

											   // Iterate over each quad and compute indices.
	int k = 0;
	for (DWORD i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			indices[k] = i*n + j;
			indices[k + 1] = i*n + j + 1;
			indices[k + 2] = (i + 1)*n + j;

			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = i*n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}

	D3D10_BUFFER_DESC ibd;
	ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumFaces * 3;
	ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR( mDevice->CreateBuffer(&ibd, &iinitData, &mIB));

	HR( D3DX10CreateShaderResourceViewFromFile(device, szTexFile, NULL, NULL, &this->mGrassMapRV, NULL));
	HR( D3DX10CreateEffectFromFile(szEffectFile, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, device, NULL, NULL, &mEffect, NULL, 0));
	HR( D3DX10CreateShaderResourceViewFromFile(mDevice, TEXT("defaultspec.dds"), 0, 0, &mDefaultSpecMapRV, 0));
	mTech = mEffect->GetTechniqueByName("TexAlphaTech");

	mParallelLight.dir = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	mParallelLight.ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	mParallelLight.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mParallelLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);


}


Ground::~Ground()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Ground::draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3DXMATRIX world,mWVP;
	D3DXMatrixIdentity(&world);
	mDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	mDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	mDevice->DrawIndexed(mNumFaces * 3, 0, 0);
	D3DXMATRIX landTexMtx;
	D3DXMatrixScaling(&landTexMtx, 5.0f, 5.0f, 1.0f);
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	mEffect->GetVariableByName("gLight")->SetRawValue(&mParallelLight, 0, sizeof(Light));
	mEffect->GetVariableByName("gEyePosW")->SetRawValue(&GetCamera().position(), 0, sizeof(D3DXVECTOR3));
	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		ID3D10EffectPass* pass = mTech->GetPassByIndex(i);
		mWVP = world*GetCamera().view()*GetCamera().proj();
		mEffect->GetVariableByName("gWVP")->AsMatrix()->SetMatrix((float*)&mWVP);
		mEffect->GetVariableByName("gWorld")->AsMatrix()->SetMatrix((float*)&world);
		mEffect->GetVariableByName("gTexMtx")->AsMatrix()->SetMatrix((float*)&landTexMtx);

		mEffect->GetVariableByName("gDiffuseMap")->AsShaderResource()->SetResource(mGrassMapRV);
		mEffect->GetVariableByName("gSpectMap")->AsShaderResource()->SetResource(mDefaultSpecMapRV);
		pass->Apply(0);
	}
}
