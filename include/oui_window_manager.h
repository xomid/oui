#pragma once
#include "oui_uix.h"

class OUI_API WindowManager
{
private:
	std::vector<UIX*> windows;
public:
	void append(std::wstring title, OUI* container, Rect* rc = NULL);
	UIX* get(size_t id);
	int run();
};

