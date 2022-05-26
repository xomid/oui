#include <iostream>
#include "oui_window_manager.h"
#include "oui_container.h"

int main() 
{
	if (OUI::init()) return 0;
	WindowManager wmg;
	wmg.append(L"foo", new UIContainer());
	return wmg.run();
}
