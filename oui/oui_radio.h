#pragma once
#include "oui_link.h"

enum class OUI_API UIRadioType {
	Circle = 0,
	ButtonStyle
};

class OUI_API UIRadio : public UILink
{
public:
	UIRadio();
	UIRadioType type;
	void on_click(int x, int y, size_t param) override;
	bool select(bool bSelect) override;
	void on_update() override;
};