#include "oui_number.h"
#include "oui_uix.h"
#include <regex>

UINumber::UINumber() : UIText(), number("0", "1", "na", "na") {
	num_to_text();
	set_place_holder_text(L"");
	showClearIcon = false;
	btnIncrease.create(
		Rect(0, 0, 20, 20),
		0,
		parse_svg(R"(<svg viewBox="0 0 11 7"><path fill="none" stroke="currentColor" stroke-width="1.3" d="m.5 5.5 5-4 5 4"/></svg>)"));
	btnDecrease.create(
		Rect(0, 0, 20, 20),
		0,
		parse_svg(R"(<svg viewBox="0 0 11 7" width="11" height="7" fill="none"><path stroke="currentColor" stroke-width="1.3" d="m.5 1.5 5 4 5-4"/></svg>)"));

	btnIncrease.set_colors(Colors::lightgray, Colors::white, Colors::darkgray);
	btnDecrease.set_colors(Colors::lightgray, Colors::white, Colors::darkgray);
}

void UINumber::set_text(std::wstring newText) {
	if (is_number(newText)) {
		UIText::set_text(newText);
		text_to_num();
	}
	else {
		num_to_text();
	}
}

bool UINumber::pre_append(wchar_t c) {
	if (::iswdigit(c) != 0) return true;
	if (c != L'.' && c != L'+' && c != L'-') return false;

	for (auto ch : text) {
		if (ch == c) return false;
	}

	return true;
}

bool UINumber::pre_append(std::wstring str) {
	return std::regex_match(str, std::wregex(L"[(-|+)|][0-9]+"));
}

void UINumber::blur() {
	if (text.length() == 0)
		set_text(L"0");
	text_to_num();
	trigger_update();
	UIText::blur();
}

void UINumber::on_update() {
	UIText::on_update();
	btnIncrease.draw(canvas);
	btnDecrease.draw(canvas);
}

void UINumber::on_resize(int width, int height) {
	UIText::on_resize(width, height);
	Rect rc; UILabel::get_content_area(rc);
	auto sh = (contentArea.height / 2) * .9;
	int sw = int(sh * btnIncrease.get_aratio());
	int l = area.width;
	int mid = (rc.height) / 2;
	btnIncrease.rc.set(l - sw, (int)round((mid - sh) / 2.0) + rc.top, sw, (int)sh);
	btnDecrease.rc.set(l - sw, (int)round((mid - sh) / 2.0) + mid + rc.top, sw, (int)sh);
}

void UINumber::on_key_down(uint32_t key, uint32_t nrep, uint32_t flags) {
	UIText::on_key_down(key, nrep, flags);
	if (!isActive || is_key_on(VK_CONTROL)) return;
	switch (key) {
	case VK_UP:
		increase();
		break;
	case VK_DOWN:
		decrease();
		break;
	case VK_RETURN:
		text_to_num();
		trigger_update();
		invalidate();
		break;
	default:
		auto str = ocom::to_string(text);
		if (is_number(str) && number.is_in_range(str)) {
			text_to_num();
			trigger_update();
			invalidate();
		}
		break;
	}
}

void UINumber::trigger_update() {
	text_to_num();
	UIText::trigger_update();
}

std::string UINumber::get_number() const {
	return number.get_value();
}

void UINumber::increase() {
	++number;
	num_to_text();
	trigger_update();
	invalidate();
}

void UINumber::decrease() {
	--number;
	num_to_text();
	trigger_update();
	invalidate();
}

void UINumber::on_timer(uint32_t nTimer) {
	UIText::on_timer(nTimer);
	if (nTimer != 1 && nTimer != 2) return;

	if ((clock() - lastChange) < waitingElapse)
		return;
	lastChange = clock();
	waitingElapse = Max(1U, (clock_t)(waitingElapse * UINUMBER_CHANGE_ACCELERATION));

	if (nTimer == 1) {
		increase();
	}
	else if (nTimer == 2) {
		decrease();
	}
}

void UINumber::config(std::string number, std::string tick, std::string minPrice, std::string maxPrice) {
	this->number.set(number, tick, minPrice, maxPrice);
	num_to_text();
}

void UINumber::text_to_num() {
	auto str = ocom::to_string(text);
	if (!is_number(str)) str = "0";
	number.set(str);
	str = number.str();
	text = ocom::to_wstring(str);
	invalidate();
}

void UINumber::num_to_text() {
	auto str = number.str();
	text = ocom::to_wstring(str);
	set_index((int)this->text.length());
	reset_selection();
	invalidate();
}

void UINumber::on_mouse_move(int x, int y, uint32_t flags) {
	if (!btnIncrease.on_mouse_move(x, y, flags).isHover)
		if (!btnDecrease.on_mouse_move(x, y, flags).isHover)
			UIText::on_mouse_move(x, y, flags);
	invalidate();
}

void UINumber::on_mouse_down(int x, int y, uint32_t flags) {
	btnIncrease.on_mouse_down(x, y, flags);
	btnDecrease.on_mouse_down(x, y, flags);
	if (btnIncrease.isPressed) {
		kill_timer(2);
		set_timer(1, UINUMBER_CHANGE_TICK_ELAPSE);
	}
	else if (btnDecrease.isPressed) {
		kill_timer(1);
		set_timer(2, UINUMBER_CHANGE_TICK_ELAPSE);
	}
	waitingElapse = UINUMBER_CHANGE_ELAPSE;
	lastChange = clock();
	lastDown = clock();
	UIText::on_mouse_down(x, y, flags);
	if (!isActive) focus();
	invalidate();
}

void UINumber::on_mouse_up(int x, int y, uint32_t flags) {
	kill_timer(1);
	kill_timer(2);
	if (waitingElapse == UINUMBER_CHANGE_ELAPSE) {
		if (btnIncrease.on_mouse_up(x, y, flags)) {
			increase();
		}
		else if (btnDecrease.on_mouse_up(x, y, flags)) {
			decrease();
		}
		else {
			UIText::on_mouse_up(x, y, flags);
		}
	}
	else {
		UIText::on_mouse_up(x, y, flags);
	}
	invalidate();
}

void UINumber::on_dbl_click(int x, int y, uint32_t flags) {
	UIText::on_dbl_click(x, y, flags);
	btnIncrease.on_mouse_down(x, y, flags);
	btnDecrease.on_mouse_down(x, y, flags);
	invalidate();
}

void UINumber::on_mouse_enter(OUI* prev) {
	UIText::on_mouse_enter(prev);
}

void UINumber::on_mouse_leave(OUI* next) {
	UIText::on_mouse_leave(next);
	btnIncrease.isHover = false;
	btnDecrease.isHover = false;
	btnIncrease.adj_colors();
	btnDecrease.adj_colors();
	invalidate();
}

void UINumber::set_range(double num, double step, double minValue, double maxValue) {
	number.set_min(minValue);
	number.set_max(maxValue);
	number.set_step(step);
	number.set_value(num);
	num_to_text();
}

void UINumber::set_range(std::string num, std::string step, std::string minValue, std::string maxValue) {
	number.set_value(num);
	number.set_step(step);
	number.set_min(minValue);
	number.set_max(maxValue);
	num_to_text();
}

void UINumber::set_value(double value) {
	number.set_value(value);
	num_to_text();
}

void UINumber::set_value(std::string value) {
	number.set_value(value);
	num_to_text();
}