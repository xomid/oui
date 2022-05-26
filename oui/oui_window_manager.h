#pragma once
#include "oui_uix.h"

class WindowManager
{
private:
	std::vector<UIX*> windows;
public:
	void append(std::wstring title, OUI* container);
	int run();
};

