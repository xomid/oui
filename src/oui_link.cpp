
#include "oui_link.h"

void UILink::on_update() {
	if (!isPressed && !isHover) {
		colors["currentColor"] = color;
		return UILabel::on_update();
	}
	color.save();
	color.set(isPressed ? downColor : isHover ? hoverColor : color);
	colors["currentColor"] = color;
	UILabel::on_update();
	color.restore();
}

void UILink::set_color(Color color) {
	UIButton::set_color(color);
	hoverColor.brightness(&color, +20);
	downColor.brightness(&color, -20);
}

