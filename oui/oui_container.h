#pragma once
#include "oui.h"
#include "oui_button.h"

class UIContainer : public OUI
{
	UIButton btn;
public:
	void on_init() override;
};

