#include <iostream>
#include "oui_window_manager.h"
#include "oui_container.h"

int main(int argc, char* argv[]) 
{
	if (OUI::init()) return 0;
	WindowManager wmg;
	wmg.append(L"foo", new UIContainer());
	if (argc > 1) {
		if (!strcmp(argv[1], "--inspect")) {
			auto wnd = wmg.get(0);
			if (wnd) wnd->bDrawBoxModel = true;
		}
	}
	return wmg.run();
}
