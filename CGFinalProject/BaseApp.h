#pragma once
#include <Windows.h>
#include <tchar.h>
#include <D3DX10.h>
#include "Common.h"
class BaseApp
{
public:
	BaseApp(HINSTANCE);
	~BaseApp();
	virtual LRESULT CALLBACK msgProc(UINT, WPARAM, LPARAM);
	virtual void onResize();
	virtual void render();
	virtual int run();
	virtual void updateScene();

	HWND hwnd;
	HINSTANCE hinst;

	ID3D10Device* mDevice;
	IDXGISwapChain* mSwapChain;
	ID3D10Texture2D* mDepthBuffer;
	ID3D10RenderTargetView* mRenderTarget;
	ID3D10DepthStencilView* mDepthStencilView;
	D3D10_DRIVER_TYPE mDriverType;
	D3DXCOLOR mClearColor;

	int mClientWidth, mClientHeight;
	bool mResizing;
};

