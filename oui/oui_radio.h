#pragma once
#include "oui_link.h"

enum class OUI_API UIRadioType {
	Radio,
	Button,
	Toggle
};

class OUI_API UIRadio : public UILink
{
protected:
	UIRadioType type;
	Sheet shape;
	int shapeLeft, shapeMargin;
	Color borderColor, normalBorderColor;
public:
	UIRadio();

	OUI* create(int left, int top, int width, int height, OUI* parent, bool bAddToParent = true) override;
	void on_click(int x, int y, uint32_t param) override;
	bool select(bool bSelect) override;
	void on_update() override;
	void set_type(UIRadioType type);
	void on_mouse_enter(OUI* prev) override;
	void on_mouse_leave(OUI* next) override;
	void on_mouse_move(int x, int y, uint32_t flags) override;
	void on_mouse_down(int x, int y, uint32_t flags) override;
	void on_mouse_up(int x, int y, uint32_t flags) override;
	void on_dbl_click(int x, int y, uint32_t flags) override;
	void gen_circle_shape(Sheet& sheet, Color back, Color border, Color normalBorder);
	virtual void invalidate_shape();
};