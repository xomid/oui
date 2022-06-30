#include "oui_button.h"

UIButton::UIButton() {
	bPressed = bHover = 0;
	bFocusable = false;
	borderRadius.set(4, 4, 4, 4);
	enable();
}

void UIButton::enable(bool bEnable) {
	if (this->bEnabled == bEnable) return;
	this->bEnabled = bEnable;
	invalidate();
}

void UIButton::on_update() {
	if (!bEnabled) {
		backgroundColor.save();
		color.save();
		backgroundColor.brightness(&backgroundColor, 50);
		color.brightness(&color, 50);
		UILabel::on_update();
		backgroundColor.restore();
		color.restore();
		return;
	}
	if (!bPressed && !bHover) return UILabel::on_update();
	backgroundColor.save();
	backgroundColor.set(bPressed && bHover ? downBackColor : bHover && !bPressed ? hoverBackColor : backgroundColor);
	UILabel::on_update();
	backgroundColor.restore();
}

void UIButton::apply_theme(bool bInvalidate) {
	OUI::apply_theme(bInvalidate);
	set_background_color(OUITheme::secondary);
	set_color(OUITheme::text);
	if (bInvalidate) invalidate();
}

void UIButton::on_mouse_move(int x, int y, uint32_t param) {
	if (!bEnabled) return;
	bHover = handleArea.is_inside(x, y);
	invalidate();
}

void UIButton::on_mouse_down(int x, int y, uint32_t param) {
	UILabel::on_mouse_down(x, y, param);
	bPressed = bHover;
	invalidate();
}

void UIButton::on_click(int x, int y, uint32_t param) {
	if (!bEnabled) return;
	if (parent) parent->process_event(this, Event::Click, 0, true);
}

void UIButton::on_dbl_click(int x, int y, uint32_t param) {
	on_mouse_down(x, y, param);
}

bool UIButton::select(bool bSelect) {
	OUI::select(bSelect);
	return false;
}

void UIButton::on_mouse_up(int x, int y, uint32_t param) {
	UILabel::on_mouse_up(x, y, param);

	if (bPressed && bEnabled && handleArea.is_inside(x, y))
		on_click(x, y, param);

	bPressed = 0;
	invalidate();
}

void UIButton::on_mouse_enter(OUI* prev) {
	OUI::on_mouse_enter(prev);
	on_mouse_move(1, 1, 0);
}

void UIButton::on_mouse_leave(OUI* next) {
	OUI::on_mouse_leave(next);
	on_mouse_move(-1, -1, 0);
}

void UIButton::on_timer(uint32_t nTimer) {
	invalidate();
}

bool UIButton::is_withinArea(int x, int y)
{
	return handleArea.is_inside(x, y);
}

void UIButton::on_resize(int width, int height) {
	OUI::on_resize(width, height);
	handleArea.set(0, 0, area.width, area.height);
}

void UIButton::set_background_color(Color backgroundColor) {
	OUI::set_background_color(backgroundColor);
	hoverBackColor.brightness(&backgroundColor, +20);
	downBackColor.brightness(&backgroundColor, -20);
}