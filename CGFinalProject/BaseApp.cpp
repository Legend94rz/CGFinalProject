#include "BaseApp.h"
LRESULT CALLBACK MainMsgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BaseApp* theApp = NULL;
	switch (message)
	{
	case WM_CREATE:
	{
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		theApp = (BaseApp*)(cs->lpCreateParams);
		return 0;
	}
	}
	if (theApp)
		return theApp->msgProc(message, wParam, lParam);
	else
		return DefWindowProc(hwnd, message, wParam, lParam);
}



BaseApp::BaseApp(HINSTANCE hinst)
{
	//default values
	this->hinst = hinst;
	this->hwnd = NULL;
	mResizing = false;
	mDevice = 0;
	mSwapChain = 0;
	mDepthBuffer = 0;
	mDepthStencilView = 0;
	mRenderTarget = 0;

	mDriverType = D3D10_DRIVER_TYPE_HARDWARE;
	mClearColor = D3DXCOLOR(97 / 255.0, 133 / 255.0, 218 / 255.0, 1.0);
	mClientHeight = 600;
	mClientWidth = 800;

	//init window
	WNDCLASS wndclass = { 0 };
	wndclass.hInstance = this->hinst;
	wndclass.lpfnWndProc = MainMsgProc;		//回调函数不能是一个类的成员函数
	wndclass.lpszClassName = _T("className");
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wndclass);
	hwnd = CreateWindow(_T("className"), _T("CGFinalProject"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		mClientWidth, mClientHeight, NULL, NULL, hinst, this);
	UpdateWindow(hwnd);
	ShowWindow(hwnd, SW_SHOW);

	//init dx10
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.Width = mClientWidth;

	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.BufferCount = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	HRESULT hr;
	hr = D3D10CreateDeviceAndSwapChain(NULL, mDriverType, NULL,
		D3D10_CREATE_DEVICE_DEBUG, D3D10_SDK_VERSION, &sd, &mSwapChain, &mDevice);
	if (FAILED(hr))return;

	onResize();

}
void BaseApp::onResize()
{
	ReleaseCOM(mRenderTarget);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthBuffer);

	mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight,DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	//Todo: 不重新创建试试看
	ID3D10Texture2D* backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D),
		reinterpret_cast<void**>(&backBuffer));
	mDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTarget);
	backBuffer->Release();
	//===================================================

	D3D10_TEXTURE2D_DESC td;
	td.Width = mClientWidth;
	td.Height = mClientHeight;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D16_UNORM;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D10_USAGE_DEFAULT;
	td.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	mDevice->CreateTexture2D(&td, 0, &mDepthBuffer);
	mDevice->CreateDepthStencilView(mDepthBuffer, 0, &mDepthStencilView);

	mDevice->OMSetRenderTargets(1, &mRenderTarget, mDepthStencilView);

	D3D10_VIEWPORT vp;
	vp.TopLeftX = vp.TopLeftY = 0;
	vp.MaxDepth = 1;
	vp.MinDepth = 0;
	vp.Width = mClientWidth;
	vp.Height = mClientHeight;

	mDevice->RSSetViewports(1, &vp);

}
LRESULT BaseApp::msgProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_SIZE:
	{
		mClientHeight = HIWORD(lParam);
		mClientWidth = LOWORD(lParam);
		if (mDevice && !mResizing)
		{
			if (wParam != SIZE_MINIMIZED)
			{
				onResize();
			}
		}
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_ENTERSIZEMOVE:
	{
		mResizing = true;
		return 0;
	}
	case WM_EXITSIZEMOVE:
	{
		mResizing = false;
		onResize();
		return 0;
	}
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
void BaseApp::render()
{
	mDevice->ClearRenderTargetView(mRenderTarget, mClearColor);
	mDevice->ClearDepthStencilView(mDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
}
int BaseApp::run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			updateScene();
			render();
		}
	}
	return msg.wParam;
}
void BaseApp::updateScene()
{
}
BaseApp::~BaseApp()
{
	ReleaseCOM(mRenderTarget);
	ReleaseCOM(mDepthBuffer);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDevice);
	ReleaseCOM(mDepthStencilView);
}
