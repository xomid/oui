#include <iostream>
#include "oui_window_manager.h"
#include "container.h"

int main() {
	if (OUI::init()) return 0;
	WindowManager wmg;
	Rect rc(100, 100, 600, 500);
	wmg.append(L"foo", new UIContainer(), &rc);
	auto wnd = wmg.get(0);
	//wnd->show_box_model();
	return wmg.run();
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	int argc = 0;
	auto winArgv = GetCommandLineW();
	auto argv = CommandLineToArgvW(winArgv, &argc);
	return main();
}

