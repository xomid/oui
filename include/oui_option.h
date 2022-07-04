#pragma once
#include "oui_button.h"

class OUI_API UIOption : public UIButton
{
public:
	using UIButton::create;
	virtual void on_update();
};