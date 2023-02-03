#include "oui_check.h"

UICheck::UICheck() {
	checkShape = parse_svg(R"(
		<svg viewBox="0 0 24 24" fill="none"><path d="m5 13 4 4L19 7" stroke="currentColor" stroke-width="3" stroke-linecap="round" stroke-linejoin="round"/></svg>
	)");
	checkShape->m_colors = &colors;
	checkColor.set("#ddd");
}


void UICheck::gen_check_shape(Sheet& sheet, Color back, Color color, Color normalColor, Color checkColor) {
	auto w = sheet.w,
		h = sheet.h;

	if (isPressed && isHover) color = color.bright(-20);
	else if (isHover) color = color.bright(20);
	
	byte innerR = backgroundColor.r, innerG = backgroundColor.g, innerB = backgroundColor.b,
		outerR = color.r, outerG = color.g, outerB = color.b,
		backR = backgroundColor.r, backG = backgroundColor.g, backB = backgroundColor.b;

	if (isSelected)
		innerR = color.r, innerG = color.g, innerB = color.b;
	else {
		color = normalColor;
		if (isPressed && isHover) {
			color = normalColor.bright(-20);
			checkColor = checkColor.bright(-20);
		}
		else if (isHover) {
			color = normalColor.bright(20);
			checkColor = checkColor.bright(20);
		}
		outerR = color.r, outerG = color.g, outerB = color.b;
	}

#define __SCL 6
#define __SCLB 64
#define __SCLD 64.0
#define _UPS(v) ((v) << __SCL)
#define _DNS(v) ((v) >> __SCL)
#define _CEL(v) (_UPS(_DNS(v + 63)))
#define _FLR(v) (((v) >> __SCL) << __SCL)

	int margin = 1;
	int umargin = _UPS(margin);
	int ustrokeWidth = int(1 * __SCLD);
	int r = 3;
	int ustrokeWidthCeil = _CEL(ustrokeWidth);
	int uh = _UPS(h);
	int uw = _UPS(w);
	int urx1 = _UPS(r);
	int ury1 = _UPS(r);
	int urx2 = _UPS(r);
	int ury2 = _UPS(r);
	int urx3 = _UPS(r);
	int ury3 = _UPS(r);
	int urx4 = _UPS(r);
	int ury4 = _UPS(r);

	urx1 = Min(uw - ustrokeWidth, urx1);
	ury1 = Min(uh - ustrokeWidth, ury1);

	double rx12 = double(urx1 / __SCLD) * double(urx1 / __SCLD);
	double ry12 = double(ury1 / __SCLD) * double(ury1 / __SCLD);
	double ab1 = double(urx1 / __SCLD) * double(ury1 / __SCLD);

	double rx22 = double(urx2 / __SCLD) * double(urx2 / __SCLD);
	double ry22 = double(ury2 / __SCLD) * double(ury2 / __SCLD);
	double ab2 = double(urx2 / __SCLD) * double(ury2 / __SCLD);

	double rx32 = double(urx3 / __SCLD) * double(urx3 / __SCLD);
	double ry32 = double(ury3 / __SCLD) * double(ury3 / __SCLD);
	double ab3 = double(urx3 / __SCLD) * double(ury3 / __SCLD);

	double rx42 = double(urx4 / __SCLD) * double(urx4 / __SCLD);
	double ry42 = double(ury4 / __SCLD) * double(ury4 / __SCLD);
	double ab4 = double(urx4 / __SCLD) * double(ury4 / __SCLD);

	int rem;
	int x, y;
	byte a, ca;
	pyte data = sheet.data, d;
	auto pitch = sheet.pitch;
	for (y = 0; y < h; ++y) {
		d = data + y * pitch;
		for (x = 0; x < w; ++x) {

			bool empty = false;
			bool foo = false;
			int mode = 0;// 0: none, 1: side, 2: corners

			int ux = _UPS(x);
			int uy = _UPS(y);
			int cx, cy, ccx, ccy;
			double ab, cx2, cy2;
			double inab, incx2, incy2;

			if (ux < _CEL(urx1 + umargin) && uy < _CEL(ury1 + umargin)) {
				// top-left corner
				mode = 2;
				cx = urx1;
				cy = ury1;
				ab = ab1;
				cx2 = rx12;
				cy2 = ry12;
				ccx = cx - __SCLB + umargin;
				ccy = cy - __SCLB + umargin;
			}
			else if (ux > _CEL(uw - urx2 - umargin) && uy < _CEL(ury2 + umargin)) {
				// top-right corner
				mode = 2;
				cx = urx2;
				cy = ury2;
				ab = ab2;
				cx2 = rx22;
				cy2 = ry22;
				ccx = uw - urx2 - umargin;
				ccy = cy - __SCLB + umargin;
			}
			else if (ux > _CEL(uw - urx3 - umargin) && uy > _FLR(uh - ury3 - umargin)) {
				// bottom-right corner
				mode = 2;
				cx = urx3;
				cy = ury3;
				ab = ab3;
				cx2 = rx32;
				cy2 = ry32;
				ccx = uw - urx3 - umargin;
				ccy = uh - ury3 - umargin;
			}
			else if (ux < _CEL(urx4 + umargin) && uy > _FLR(uh - ury4 - umargin)) {
				// bottom-left corner
				mode = 2;
				cx = urx4;
				cy = ury4;
				ab = ab4;
				cx2 = rx42;
				cy2 = ry42;
				ccx = urx4 - __SCLB + umargin;
				ccy = uh - ury4 - umargin;
			}
			else if (uy < (ustrokeWidthCeil - __SCLB + umargin)) {
				// top side
				mode = uy < (umargin - __SCLB) ? 3 : 1;
			}
			else if (ux > (uw - ustrokeWidthCeil - umargin)) {
				// right side
				mode = ux > (uw - umargin) ? 3 : 1;
			}
			else if (uy > (uh - ustrokeWidthCeil - umargin)) {
				// bottom side
				mode = uy > (uh - umargin) ? 3 : 1;
			}
			else if (ux < (ustrokeWidthCeil - __SCLB + umargin)) {
				// left side
				mode = ux < (umargin - __SCLB) ? 3 : 1;
			}
			else {
				mode = 0;
			}

			if (mode == 0) {
				*d++ = innerB;
				*d++ = innerG;
				*d++ = innerR;
			}
			else if (mode == 1) {
				*d++ = outerB;
				*d++ = outerG;
				*d++ = outerR;
			}
			else if (mode == 2) {
				double ang = atan2(uy - ccy, ux - ccx);
				int urad = int((ab / sqrt(
					cx2 * pow(sin(ang), 2) +
					cy2 * pow(cos(ang), 2)
				)) * __SCLD);
				int udis = int(hypot(ux - ccx, uy - ccy));

				inab = double((cx - ustrokeWidth) / __SCLD) * double((cy - ustrokeWidth) / __SCLD);
				incx2 = double((cx - ustrokeWidth) / __SCLD) * double((cx - ustrokeWidth) / __SCLD);
				incy2 = double((cy - ustrokeWidth) / __SCLD) * double((cy - ustrokeWidth) / __SCLD);

				int uinnerRad = int((inab / sqrt(
					incx2 * pow(sin(ang), 2) +
					incy2 * pow(cos(ang), 2)
				)) * __SCLD);

				if (udis < (urad + __SCLB)) {
					if (udis < urad) {
						if (udis < (uinnerRad + __SCLB)) {
							// inner circle solid
							if (udis <= uinnerRad) {
								*d++ = innerB;
								*d++ = innerG;
								*d++ = innerR;
							}
							// inner/outer common AA
							else {
								rem = udis - uinnerRad;
								ca = CLAMP255(rem << 2);
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
						// outerRad < dis < outerRad + __SCLB
						rem = udis - urad;
						ca = CLAMP255(rem << 2);
						a = 0xff - ca;
						*d++ = CLAMP255((a * outerB + ca * backB) / 255);
						*d++ = CLAMP255((a * outerG + ca * backG) / 255);
						*d++ = CLAMP255((a * outerR + ca * backR) / 255);
					}
				}
				else mode = 3;
			}

			if (mode == 3) {
				*d++ = backB;
				*d++ = backG;
				*d++ = backR;
			}
		}
	}

	Canvas can(NULL, &shape);
	if (isSelected) {

		colors["currentColor"].set(checkColor);
		can.render_svg(checkShape, 1, 1, shape.w, shape.h, 0xff, NULL, NULL);
	}
}

void UICheck::invalidate_shape() {
	Color color(borderColor), normalColor(normalBorderColor);
	gen_check_shape(shape, backgroundColor, color, normalColor, checkColor);
	invalidate();
}

void UICheck::on_click(int x, int y, uint32_t param) {
	if (!isEnabled) return;
	if (parent) parent->process_event(this, isSelected ? Event::Deselect : Event::Select, 0, true);
}

bool UICheck::select(bool bSelect) {
	bool res = OUI::select(bSelect);
	invalidate_shape();
	return res;
}
