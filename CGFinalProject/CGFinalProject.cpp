#include "BaseApp.h"



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	BaseApp app(hInstance);
	return app.run();
}