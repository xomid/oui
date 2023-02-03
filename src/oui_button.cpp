#include "oui_button.h"

UIButton::UIButton() {
	isPressed = isHover = 0;
	isFocusable = false;
	borderRadius.set(4, 4, 4, 4);
	enable();
}

void UIButton::on_update() {
	if (!isEnabled) {
		backgroundColor.save();
		color.save();
		backgroundColor.brightness(&backgroundColor, 50);
		color.brightness(&color, 50);
		UILabel::on_update();
		backgroundColor.restore();
		color.restore();
		return;
	}
	if (!isPressed && !isHover) return UILabel::on_update();
	backgroundColor.save();
	backgroundColor.set(isPressed && isHover ? downBackColor : isHover && !isPressed ? hoverBackColor : backgroundColor);
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
	if (!isEnabled) return;
	isHover = handleArea.is_inside(x, y);
	invalidate();
}

void UIButton::on_mouse_down(int x, int y, uint32_t param) {
	UILabel::on_mouse_down(x, y, param);
	isPressed = isHover;
	invalidate();
}

void UIButton::on_click(int x, int y, uint32_t param) {
	if (!isEnabled) return;
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

	if (isPressed && isEnabled && handleArea.is_inside(x, y))
		on_click(x, y, param);

	isPressed = 0;
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