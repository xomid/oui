#pragma once
#include "oui_radio.h"

class OUI_API UICheck : public UIRadio
{
private:
	PSVGShape checkShape;
	ShapeStorage roundedRect;
	Color checkColor;
public:
	UICheck();
	void invalidate_shape() override;
	void on_click(int x, int y, uint32_t param) override;
	void gen_check_shape(Sheet& sheet, Color back, Color border, Color normalBorder, Color checkColor);
};