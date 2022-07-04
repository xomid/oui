#pragma once
#include "oui_stack.h"

class OUI_API UIMenu : public UIStack
{
public:
	using UIStack::create;
	UIMenu();
	OUI* create(int left, int top, int width, int height, OUI* parent, bool bAddToParent = true) override;
	void apply_theme(bool bInvalidate) override;
};