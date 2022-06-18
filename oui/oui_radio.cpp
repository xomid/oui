#include "oui_radio.h"

UIRadio::UIRadio() {
	type = UIRadioType::Circle;
	ltr = false;
}

void UIRadio::on_update() {
	if (!bSelected && !bDown && !bHover) return UILabel::on_update();
	backgroundColor.save();
	backgroundColor.set(bSelected || bDown ? downBackColor : bHover ? hoverBackColor : backgroundColor);
	UILabel::on_update();
	backgroundColor.restore();
}

bool UIRadio::select(bool bSelect) {
	bool res = OUI::select(bSelect);

	//if (type == UIRadioType::Circle) {
	//	if (bSelected) colors["currentColor2"] = colors["currentColor"];
	//	else colors["currentColor2"] = normalBackgroundColor;
	//	backgroundColor.set(normalBackgroundColor);
	//}
	//else {
	//	color.set(normalColor);
	//	currColor.set(normalBackgroundColor);
	//	backgroundColor.set(normalBackgroundColor);//bSelected ? normalBackgroundColor : bDown ? downBackground : bHover ? hoverBackground : normalBackgroundColor);
	//}

	invalidate();
	return res;
}

void UIRadio::on_click(int x, int y, size_t param) {
	if (!bEnabled) return;
	if (parent) parent->process_event(this, Event::Select, 0, true);
}