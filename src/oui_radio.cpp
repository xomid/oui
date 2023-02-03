#include "oui_radio.h"

void UIRadio::gen_circle_shape(Sheet& sheet, Color back, Color border, Color normalBorder) {
	auto w = sheet.w,
		h = sheet.h;
	int margin = 0;
	if (w <= 2 * margin || h <= 2 * margin)
		return;

	byte a, ca;
	pyte data = sheet.data, d;
	auto pitch = sheet.pitch;
	double outerRad, outerWidth, innerRad, dis, cx, cy, rem, outerRadCeil, innerRadCeil, dx, dy;
	int x, y;
	outerRad = double((h - 2 * margin) >> 1);
	outerWidth = Min(4, int(w + 3) / 4);

	if (!isSelected) {
		border.set(normalBorder);
		outerWidth = 1.3;
	}

	if (isPressed && isHover) border = border.bright(-20);
	else if (isHover) border = border.bright(20);

	byte innerR = back.r, innerG = back.g, innerB = back.b,
		outerR = border.r, outerG = border.g, outerB = border.b,
		backR = back.r, backG = back.g, backB = back.b;

	innerRad = outerRad - outerWidth;
	cx = (w - 2 * margin) / 2.0 + margin;
	cy = (h - 2 * margin) / 2.0 + margin;
	outerRadCeil = outerRad + 1;
	innerRadCeil = innerRad + 1;

	// areas:
	// outer empty
	// outer AA
	// outer solid
	// inner/outer common AA
	// inner solid

	for (y = 0; y < h; ++y) {
		d = data + y * pitch;
		for (x = 0; x < w; ++x) {
			dx = fabs((double)x - cx);
			dy = fabs((double)y - cy);
			if (x >= cx) dx += 1;
			if (y >= cy) dy += 1;
			dis = hypot(dx, dy);

			if (dis < outerRadCeil) {
				if (dis <= outerRad) {
					if (dis <= innerRadCeil) {
						// inner circle solid
						if (dis <= innerRad) {
							*d++ = innerB;
							*d++ = innerG;
							*d++ = innerR;
						}
						// inner/outer common AA
						else {
							rem = dis - innerRad;
							ca = CLAMP255(int(rem * 0xff));
							a = 0xff - ca;
							*d++ = CLAMP255(DIV255(a * innerB + ca * outerB));
							*d++ = CLAMP255(DIV255(a * innerG + ca * outerG));
							*d++ = CLAMP255(DIV255(a * innerR + ca * outerR));
						}
					}
					// outer circle solid
					else {
						*d++ = outerB;
						*d++ = outerG;
						*d++ = outerR;
					}
				}
				//outer circle AA part;
				else {
					// outerRad < dis < outerRad + 1
					rem = dis - outerRad;
					ca = CLAMP255(int(rem * 0xff));
					a = 0xff - ca;
					*d++ = CLAMP255((a * outerB + ca * backB) / 255);
					*d++ = CLAMP255((a * outerG + ca * backG) / 255);
					*d++ = CLAMP255((a * outerR + ca * backR) / 255);
				}
			}
			// empty outer circle
			else {
				*d++ = backB;
				*d++ = backG;
				*d++ = backR;
			}
		}
	}
}

UIRadio::UIRadio() {
	set_type(UIRadioType::Radio);
	ltr = false;
	canvas.art.alignX = Align::LEFT;
}

OUI* UIRadio::create(int left, int top, int width, int height, OUI* parent, bool bAddToParent) {
	int shapeH = CLAMP3(8, height, 20), shapeW = shapeH;

	auto res = UILink::create(left, top, width, height, parent, bAddToParent);

	if (parent && type != UIRadioType::Button) {
		set_background_color(parent->backgroundColor);
	}

	shapeLeft = 0;
	shapeMargin = 5;
	shape.create(shapeW, shapeH, 3);

	borderColor.set("#0067c0");
	normalBorderColor.set("#666");

	invalidate_shape();
	return res;
}

void UIRadio::set_type(UIRadioType type) {
	if (this->type == type) return;
	this->type = type;

	if (type == UIRadioType::Radio) {
		int shapeH = CLAMP3(8, boxModel.height, 20), shapeW = shapeH;
		shape.resize(shapeW, shapeH);
	}

	invalidate_shape();
}

void UIRadio::on_update() {
	if (type == UIRadioType::Radio)
	{
		int l = padding.left;
		padding.left = shapeLeft + shape.w + shapeMargin;
		UILabel::on_update();
		padding.left = l;
		canvas.bit_blt(shape, shapeLeft, (boxModel.height - shape.h) / 2, shape.w, shape.h, 0, 0, true);
	}
	else {
		auto c = backgroundColor;
		if (isSelected || (isPressed && isHover))
			backgroundColor.set(downBackColor);
		else if (isHover)
			backgroundColor.set(hoverBackColor);
		UILabel::on_update();
		backgroundColor = c;
	}
}

bool UIRadio::select(bool bSelect) {
	bool res = OUI::select(bSelect);
	invalidate_shape();
	process_event(this, Event::PostSelect, 0, true);
	return res;
}

void UIRadio::invalidate_shape() {
	switch (type) {
	case UIRadioType::Button:
		break;
	case UIRadioType::Toggle:
		break;
	default:
		gen_circle_shape(shape, backgroundColor, borderColor, normalBorderColor);
		break;
	}
	invalidate();
}

void UIRadio::on_click(int x, int y, uint32_t param) {
	if (!isEnabled) return;
	if (parent) parent->process_event(this, Event::Select, 0, true);
}

void UIRadio::on_mouse_enter(OUI* prev) {
	UILink::on_mouse_enter(prev);
	invalidate_shape();
}

void UIRadio::on_mouse_leave(OUI* next) {
	UILink::on_mouse_leave(next);
	invalidate_shape();
}

void UIRadio::on_mouse_move(int x, int y, uint32_t flags) {
	UILink::on_mouse_move(x, y, flags);
	invalidate_shape();
}

void UIRadio::on_mouse_down(int x, int y, uint32_t flags) {
	UILink::on_mouse_down(x, y, flags);
	invalidate_shape();
}

void UIRadio::on_mouse_up(int x, int y, uint32_t flags) {
	UILink::on_mouse_up(x, y, flags);
	invalidate_shape();
}

void UIRadio::on_dbl_click(int x, int y, uint32_t flags) {
	on_mouse_down(x, y, flags);
}

void UIRadio::on_resize(int width, int height) {
	UILink::on_resize(width, height);
	if (type == UIRadioType::Radio) {
		int shapeH = CLAMP3(8, boxModel.height, 20), shapeW = shapeH;
		shape.resize(shapeW, shapeH);
	}
	invalidate_shape();
}

void UIRadio::apply_theme(bool bInvalidate) {
	set_background_color(OUITheme::primary);
	set_color(OUITheme::text);
	if (bInvalidate) invalidate();
}