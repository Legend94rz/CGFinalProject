#include "BaseApp.h"
#include "Camera.h"
#include "Land.h"
#include "Light.h"

class App:public BaseApp
{
private:
	POINT mOldMousePos;
	Land* land;
	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectScalarVariable* mfxLightType;

	ID3D10InputLayout* mVertexLayout;

	Light light;
	D3DXMATRIX mWVP;
public:
	App(HINSTANCE hinst):BaseApp(hinst)
	{
		initFX();
		initVertexLayout();
		land = new Land(mDevice, 129, 129);
		light.dir = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
		light.ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		light.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}
	void initFX()
	{
		ID3D10Blob* compilationErrors = 0;
		HRESULT hr = 0;
		//Todo 精简:
		hr = D3DX10CreateEffectFromFile(L"lighting.fx", 0, 0,
			"fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, mDevice, 0, 0, &mFX, &compilationErrors, 0);
		if (FAILED(hr))
		{
			if (compilationErrors)
			{
				MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
				ReleaseCOM(compilationErrors);
			}
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
		}

		mTech = mFX->GetTechniqueByName("LightTech");
		mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
		mfxWorldVar = mFX->GetVariableByName("gWorld")->AsMatrix();
		mfxEyePosVar = mFX->GetVariableByName("gEyePosW");
		mfxLightVar = mFX->GetVariableByName("gLight");
		mfxLightType = mFX->GetVariableByName("gLightType")->AsScalar();//todo 从代码及效果文件中删掉这个变量
	}
	void initVertexLayout()
	{
		// Create the vertex input layout.
		D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		// Create the input layout
		D3D10_PASS_DESC PassDesc;
		mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
		HR(mDevice->CreateInputLayout(vertexDesc, 4, PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize, &mVertexLayout));
	}

	virtual void updateScene()
	{
		BaseApp::updateScene();
		double dt = 0.01;
		if (GetAsyncKeyState('W') & 0x8000) GetCamera().walk(50 * dt);
		if (GetAsyncKeyState('S') & 0x8000) GetCamera().walk(-50 * dt);
		if (GetAsyncKeyState('A') & 0x8000) GetCamera().strafe(-50 * dt);
		if (GetAsyncKeyState('D') & 0x8000) GetCamera().strafe(50 * dt);
		GetCamera().rebuildView();
	}
	virtual void onResize()
	{
		BaseApp::onResize();
		float aspect = (float)mClientWidth / mClientHeight;
		GetCamera().setLens(0.25f*D3DX_PI, aspect, 0.5f, 3000.0f);
	}
	virtual void render()
	{
		BaseApp::render();
		// Restore default states, input layout and primitive topology 
		// because mFont->DrawText changes them.  Note that we can 
		// restore the default states by passing null.
		mDevice->OMSetDepthStencilState(0, 0);
		float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		mDevice->OMSetBlendState(0, blendFactors, 0xffffffff);

		mDevice->IASetInputLayout(mVertexLayout);
		mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set per frame constants.
		mfxEyePosVar->SetRawValue(&GetCamera().position(), 0, sizeof(D3DXVECTOR3));
		mfxLightVar->SetRawValue(&light, 0, sizeof(Light));

		mfxLightType->SetInt(0);

		D3D10_TECHNIQUE_DESC techDesc;
		mTech->GetDesc(&techDesc);
		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);

		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			ID3D10EffectPass* pass = mTech->GetPassByIndex(i);

			mWVP = world*GetCamera().view()*GetCamera().proj();
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxWorldVar->SetMatrix((float*)&world);
			pass->Apply(0);
			land->draw();
			/*
			mWVP = world*GetCamera().view()*GetCamera().proj();
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxWorldVar->SetMatrix((float*)&world);
			pass->Apply(0);
			mWaves.draw();*/
		}

		mSwapChain->Present(0, 0);
	}
	virtual LRESULT CALLBACK msgProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT mousePos;
		int dx, dy;
		switch (message)
		{
		case WM_LBUTTONDOWN:
			if (wParam & MK_LBUTTON)
			{
				SetCapture(this->hwnd);
				mOldMousePos.x = LOWORD(lParam);
				mOldMousePos.y = HIWORD(lParam);
			}
			return 0;

		case WM_LBUTTONUP:
			ReleaseCapture();
			return 0;

		case WM_MOUSEMOVE:
			if (wParam & MK_LBUTTON)
			{
				mousePos.x = (int)LOWORD(lParam);
				mousePos.y = (int)HIWORD(lParam);

				dx = mousePos.x - mOldMousePos.x;
				dy = mousePos.y - mOldMousePos.y;

				GetCamera().pitch(dy * 0.0087266f);
				GetCamera().rotateY(dx * 0.0087266f);

				mOldMousePos = mousePos;
			}
			return 0;
		}
		return BaseApp::msgProc(message, wParam, lParam);
	}
	virtual ~App()
	{
		BaseApp::~BaseApp();
	}

};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	App app(hInstance);
	return app.run();
}