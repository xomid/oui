
#include "oui_link.h"

void UILink::on_init() {
	UIButton::on_init();

	set_background_color(OUITheme::primary);
	set_color(Colors::blue);
	hoverColor = Colors::aqua;
	downColor = Colors::red;
}

void UILink::on_update() {
	if (!isPressed && !isHover) {
		colors["currentColor"] = color;
		canvas.set_color(color);
		return UILabel::on_update();
	}
	canvas.set_color(isPressed ? downColor : isHover ? hoverColor : color);
	colors["currentColor"] = canvas.art.textColor;
	UILabel::on_update();
}
