#pragma once
#include "oui_button.h"
class OUI_API UILink : public UIButton
{
public:
	Color downColor, hoverColor;
	void on_update() override;
	void set_color(Color color) override;
};