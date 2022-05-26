#include "oui_window_manager.h"
#include <iostream>

void WindowManager::append(std::wstring title, OUI* container)
{
	UIX* pWindow = new UIX();
	pWindow->set_container(container);
	pWindow->set_title(title);
	pWindow->show_window(true);
	windows.push_back(pWindow);
}

int WindowManager::run()
{
	size_t i = 0, sz = windows.size();
	while (i < sz) {
		auto pWindow = windows[i];
		if (!pWindow->ProcessMessages())
		{
			windows.erase(windows.begin() + i--);
			sz = windows.size();
		}
		pWindow->OnIdle();
		if (++i >= sz) i = 0;
	}

    return 0;
}
