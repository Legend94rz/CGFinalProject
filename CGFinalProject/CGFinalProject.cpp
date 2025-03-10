#include "BaseApp.h"
#include "Sky.h"
#include "Camera.h"
#include "Ground.h"
class App : public BaseApp
{
private:
	POINT mOldMousePos;
public:
	Sky* sky;
	Ground* land;
	App(HINSTANCE hInst) :BaseApp(hInst) {
		sky = new Sky(_T("SkyBox.fx"), _T("alien.dds"), mDevice);
		land = new Ground(_T("texAlpha.fx"), _T("grass.dds"), mDevice);
	}
	virtual void updateScene()
	{
		BaseApp::updateScene();
		double dt = 0.01;
		if (GetAsyncKeyState('W') & 0x8000)GetCamera().walk(50 * dt);
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
		//sky->draw();
		land->draw();
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
//

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	App app(hInstance);
	return app.run();
}