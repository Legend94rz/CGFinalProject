#include "Land.h"
extern struct Vertex;



Land::Land(ID3D10Device* device,int m,int n)
{
	md3dDevice = device;

	mNumRows = m;
	mNumCols = n;
	float dx = 1.0;
	mNumVertices = m*n;
	mNumFaces = (m - 1)*(n - 1) * 2;


	// Create the geometry and fill the vertex buffer. 

	std::vector<Vertex> vertices(mNumVertices);
	float halfWidth = (n - 1)*dx*0.5f;
	float halfDepth = (m - 1)*dx*0.5f;
	for (DWORD i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dx;
		for (DWORD j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			// Graph of this function looks like a mountain range.
			float y = Y(x, z);

			vertices[i*n + j].pos = D3DXVECTOR3(x, y, z);

			// Color the vertex based on its height.
			if (y < -10.0f)
			{
				vertices[i*n + j].diffuse = BEACH_SAND;
				vertices[i*n + j].spec = D3DXCOLOR(0.2f, 0.2f, 0.2f, 32.0f);
			}
			else if (y < 5.0f)
			{
				vertices[i*n + j].diffuse = LIGHT_YELLOW_GREEN;
				vertices[i*n + j].spec = D3DXCOLOR(0.2f, 0.2f, 0.2f, 32.0f);
			}
			else if (y < 12.0f)
			{
				vertices[i*n + j].diffuse = DARK_YELLOW_GREEN;
				vertices[i*n + j].spec = D3DXCOLOR(0.2f, 0.2f, 0.2f, 32.0f);
			}
			else if (y < 20.0f)
			{
				vertices[i*n + j].diffuse = DARKBROWN;
				vertices[i*n + j].spec = D3DXCOLOR(0.4f, 0.4f, 0.4f, 64.0f);
			}
			else
			{
				vertices[i*n + j].diffuse = WHITE;
				vertices[i*n + j].spec = D3DXCOLOR(0.6f, 0.6f, 0.6f, 64.0f);
			}

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
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));


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
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));


}


Land::~Land()
{
}

float Land::Y(float x,float z)
{
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

void Land::draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->DrawIndexed(mNumFaces * 3, 0, 0);
}
