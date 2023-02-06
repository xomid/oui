#pragma once
#include "oui_button.h"
class OUI_API UILink : public UIButton
{
public:
	using UIButton::create;

	Color downColor, hoverColor;
	void on_init() override;
	void on_update() override;
};