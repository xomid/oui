#include "oui_button.h"

UIButton::UIButton() {
	bDown = bHover = 0;
	aMin = aMax = 0;
	stepf = .25;
	bFocusable = false;
	borderRadius.set(4, 4, 4, 4);
	enable(true);
}

void UIButton::enable(bool enable) {
	if (bEnabled == enable) return;
	bEnabled = enable;
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
	if (!bDown && !bHover) return UILabel::on_update();
	backgroundColor.save();
	backgroundColor.set(bDown && bHover ? downBackColor : bHover && !bDown ? hoverBackColor : backgroundColor);
	UILabel::on_update();
	backgroundColor.restore();
}

OUI* UIButton::create(int left, int top, int width, int height, OUI* parent, bool bAddToParent) {
	UILabel::create(left, top, width, height, parent, bAddToParent);
	if (parent) set_background_color(parent->backgroundColor.bright(+13));
	return this;
}

void UIButton::on_mouse_move(int x, int y, uint32_t param) {
	//UILabel::on_mouse_move(x, y, param);
	if (!bEnabled) return;

	bHover = handleArea.is_inside(x, y);
	//change_curr_color();
	invalidate();

	//if (!bHover && is_withinArea(x, y))
	//{
	//	bHover = 1;
	//	currColor = 
	//	//hf = 0;
	//	//killAllTimer();
	//	//set_timer(SBTN2D_TIMER_HOVER, SBTN2D_TIMER_INTERVAL, 0);
	//}
	//else if (bHover && !is_withinArea(x, y))
	//{
	//	bHover = 0;
	//	//hf = 1;
	//	//killAllTimer();
	//	//set_timer(SBTN2D_TIMER_LEAVE, SBTN2D_TIMER_INTERVAL, 0);
	//}
}

void UIButton::on_mouse_down(int x, int y, uint32_t param) {
	UILabel::on_mouse_down(x, y, param);

	bDown = false;

	if (bHover)
	{
		//df = 0;
		//killAllTimer();
		bDown = true;
		//set_timer(SBTN2D_TIMER_DOWN, SBTN2D_TIMER_INTERVAL, 0);
	}

	//change_curr_color();
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

	if (bDown && bEnabled && handleArea.is_inside(x, y))
	{
		//df = 1;
		//killAllTimer();
		//if (bHover) {
			//set_timer(SBTN2D_TIMER_UP, SBTN2D_TIMER_INTERVAL, 0);
		//}
		on_click(x, y, param);
	}

	bDown = 0;
	//change_curr_color();
	invalidate();
}

void UIButton::show_window(bool bShow) {
	OUI::show_window(bShow);
	killAllTimer();
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
	//if (nTimer == SBTN2D_TIMER_HOVER)
	//{
	//	hf += stepf;
	//	backgroundColor.mix(&normalBackgroundColor, bDown ? &downBackground : &hoverBackground, min(hf, 1));
	//	if (bDown)
	//		int r = 0;
	//	if (hf >= 1.0 || !bEnabled)
	//	{
	//		kill_timer(nTimer);
	//		hf = 1;
	//		backgroundColor.mix(&normalBackgroundColor, bDown ? &downBackground : &hoverBackground, hf);
	//	}
	//}
	//else if (nTimer == SBTN2D_TIMER_LEAVE)
	//{
	//	hf -= stepf;
	//	backgroundColor.mix(&normalBackgroundColor, bDown ? &downBackground : &hoverBackground, max(hf, 0));
	//	if (hf <= 0 || !bEnabled)
	//	{
	//		kill_timer(nTimer);
	//		hf = 0;// bHover = 0;
	//		backgroundColor.mix(&normalBackgroundColor, bDown ? &downBackground : &hoverBackground, hf);
	//	}
	//}
	//else if (nTimer == SBTN2D_TIMER_DOWN)
	//{
	//	df += stepf;
	//	backgroundColor.mix(&normalBackgroundColor, &downBackground, min(df, 1));

	//	if (df >= 1.0 || !bEnabled)
	//	{
	//		kill_timer(nTimer);
	//		df = 1;
	//		backgroundColor.mix(&hoverBackground, &downBackground, df);

	//	}
	//}
	//else if (nTimer == SBTN2D_TIMER_UP)
	//{
	//	df -= stepf;
	//	backgroundColor.mix(&normalBackgroundColor, &downBackground, max(df, 0));
	//	if (df <= 0 || !bEnabled)
	//	{
	//		kill_timer(nTimer);
	//		df = 0;
	//		backgroundColor.mix(&hoverBackground, &downBackground, df);
	//	}
	//}

	//if (bSelected) backgroundColor.set(normalBackgroundColor);
	invalidate();
}

void UIButton::killAllTimer()
{
	//kill_timer(SBTN2D_TIMER_HOVER);
	//kill_timer(SBTN2D_TIMER_LEAVE);
	//kill_timer(SBTN2D_TIMER_DOWN);
	//kill_timer(SBTN2D_TIMER_UP);
}

bool UIButton::is_withinArea(int x, int y)
{
	return handleArea.is_inside(x, y);
}

void UIButton::on_resize(int width, int height) {
	OUI::on_resize((int)cx, (int)cy);
	handleArea.set(0, 0, area.width, area.height);
}

void UIButton::set_background_color(Color color) {
	OUI::set_background_color(color);
	hoverBackColor.brightness(&color, +20);
	downBackColor.brightness(&color, -20);
}

//void UIButton::change_curr_color() {
//	currColor.set(bDown ? downBackground : bHover ? hoverBackground : normalBackgroundColor);
//	backgroundColor.set(currColor);
//}
