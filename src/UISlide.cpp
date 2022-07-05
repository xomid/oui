#include "UISlide.h"

UISlide::UISlide()
	: range("0", "na", "0", "1.0"), gradientFunc(0), chosenHandle(0) {
	slideImg.create(10, 10, 3);
	on_range_update();
	strokeWidth = 1;
}

void UISlide::set_range(double minValue, double maxValue) {
	range.set_min(minValue);
	range.set_max(maxValue);
	on_range_update();
}

void UISlide::set_range(std::string minValue, std::string maxValue) {
	range.set_min(minValue);
	range.set_max(maxValue);
	on_range_update();
}

void UISlide::set_step(double stepValue) {
	range.set_step(stepValue);
	on_range_update();
}

void UISlide::set_step(std::string stepValue) {
	range.set_step(stepValue);
	on_range_update();
}

void UISlide::set_value(double value) {
	range.set_value(value);
	on_range_update();
}

void UISlide::set_value(std::string value) {
	range.set_value(value);
	on_range_update();
}

double UISlide::get_value() const {
	return atof(range.value.str().c_str());
}

void UISlide::calc_handle_pos(RangedFloat& number, void* handle) {
	auto sValue = number.get_value(),
		sMin = number.get_min(),
		sMax = number.get_max();
	double fValue, fMin, fMax;
	if (is_number(sValue) && is_number(sMin) && is_number(sMax))
	{
		fValue = atof(sValue.c_str());
		fMin = atof(sMin.c_str());
		fMax = atof(sMax.c_str());
	}
	else {
		fValue = fMin = 0;
		fMax = 1;
	}
	calc_handle_pos(handle, (fValue - fMin) / (fMax - fMin));
}

void UISlide::set_gradient_func(GradientFunc gradFunc) {
	if (!gradFunc) return;
	if (gradFunc == this->gradientFunc) return;
	this->gradientFunc = gradFunc;
	update_slide_img();
	invalidate();
}

void UISlide::apply_theme(bool bInvalidate) {
	OUI::set_background_color(OUITheme::primary);
	if (bInvalidate) invalidate();
}

void UISlide::alert_parent(void* handle) {
	if (parent) parent->process_event(this, Event::Update, 0, true);
}

void UISlide::on_range_update() {
	calc_handle_pos(range, chosenHandle);
}

void UISlide::on_dbl_click(int x, int y, uint32_t flags) {
	on_mouse_down(x, y, flags);
}

void UISlide::on_mouse_down(int x, int y, uint32_t flags) {
	set_capture(this);
	bPressed = true;
	on_mouse_move(x, y, flags);
}

void UISlide::on_mouse_move(int x, int y, uint32_t flags) {
	if (bPressed) {
		double t = calc_value(chosenHandle, x, y, flags);
		auto& resRange = get_range(chosenHandle);
		resRange.value = t * (resRange.maxValue - resRange.minValue) + resRange.minValue;
		resRange.correct(RoundingMethod::Round);
		calc_handle_pos(resRange, chosenHandle);
		alert_parent(chosenHandle);
		invalidate();
	}
	else {
		chosenHandle = get_handle(x, y, flags);
	}
}

void UISlide::on_mouse_up(int x, int y, uint32_t flags) {
	if (get_capture() == this)
		release_capture();
	bPressed = false;
	invalidate();
}

void* UISlide::get_handle(int x, int y, uint32_t flags) {
	return 0;
}

double UISlide::calc_value(void* handle, int x, int y, uint32_t flags) {
	return 0.;
}

void UISlide::calc_handle_pos(void* handle, double t) {
}

RangedFloat& UISlide::get_range(void* handle) {
	return range;
}













void render_rounded_rect(Sheet& sheet, BorderRadius& borderRadius, Spacing& margin,
	double strokeWidth, Color& back, Color& stroke, Color& fill, GradientFunc gradientFunc, bool horizontal) {
	auto w = sheet.w,
		h = sheet.h;

	byte innerR = fill.r, innerG = fill.g, innerB = fill.b,
		outerR = stroke.r, outerG = stroke.g, outerB = stroke.b,
		backR = back.r, backG = back.g, backB = back.b;

#define __SCL 6
#define __SCLB 64
#define __SCLD 64.0
#define _UPS(v) ((v) << __SCL)
#define _DNS(v) ((v) >> __SCL)
#define _CEL(v) (_UPS(_DNS(v + 63)))
#define _FLR(v) (((v) >> __SCL) << __SCL)

	int lmargin = margin.left;
	int lumargin = _UPS(lmargin);
	int ustrokeWidth = int(strokeWidth * __SCLD);
	int ustrokeWidthCeil = _CEL(ustrokeWidth);
	int uh = _UPS(h);
	int uw = _UPS(w);
	int urx1 = _UPS(int(borderRadius.lt));
	int ury1 = _UPS(int(borderRadius.lt));
	int urx2 = _UPS(int(borderRadius.rt));
	int ury2 = _UPS(int(borderRadius.rt));
	int urx3 = _UPS(int(borderRadius.rb));
	int ury3 = _UPS(int(borderRadius.rb));
	int urx4 = _UPS(int(borderRadius.lb));
	int ury4 = _UPS(int(borderRadius.lb));

	double dw = double(w - margin.left - margin.right) - strokeWidth * 2;
	double dl = double(margin.left) + ceil(strokeWidth);
	double dh = double(h - margin.top - margin.bottom) - strokeWidth * 2;

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
	Color res(0, 0, 0);

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

			if (ux < _CEL(urx1 + lumargin) && uy < _CEL(ury1 + lumargin)) {
				// top-left corner
				mode = 2;
				cx = urx1;
				cy = ury1;
				ab = ab1;
				cx2 = rx12;
				cy2 = ry12;
				ccx = cx - __SCLB + lumargin;
				ccy = cy - __SCLB + lumargin;
			}
			else if (ux > _CEL(uw - urx2 - lumargin) && uy < _CEL(ury2 + lumargin)) {
				// top-right corner
				mode = 2;
				cx = urx2;
				cy = ury2;
				ab = ab2;
				cx2 = rx22;
				cy2 = ry22;
				ccx = uw - urx2 - lumargin;
				ccy = cy - __SCLB + lumargin;
			}
			else if (ux > _CEL(uw - urx3 - lumargin) && uy > _FLR(uh - ury3 - lumargin)) {
				// bottom-right corner
				mode = 2;
				cx = urx3;
				cy = ury3;
				ab = ab3;
				cx2 = rx32;
				cy2 = ry32;
				ccx = uw - urx3 - lumargin;
				ccy = uh - ury3 - lumargin;
			}
			else if (ux < _CEL(urx4 + lumargin) && uy > _FLR(uh - ury4 - lumargin)) {
				// bottom-left corner
				mode = 2;
				cx = urx4;
				cy = ury4;
				ab = ab4;
				cx2 = rx42;
				cy2 = ry42;
				ccx = urx4 - __SCLB + lumargin;
				ccy = uh - ury4 - lumargin;
			}
			else if (uy < (ustrokeWidthCeil - __SCLB + lumargin)) {
				// top side
				mode = uy < (lumargin - __SCLB) ? 3 : 1;
			}
			else if (ux > (uw - ustrokeWidthCeil - lumargin)) {
				// right side
				mode = ux > (uw - lumargin) ? 3 : 1;
			}
			else if (uy > (uh - ustrokeWidthCeil - lumargin)) {
				// bottom side
				mode = uy > (uh - lumargin) ? 3 : 1;
			}
			else if (ux < (ustrokeWidthCeil - __SCLB + lumargin)) {
				// left side
				mode = ux < (lumargin - __SCLB) ? 3 : 1;
			}
			else {
				mode = 0;
			}

			if (mode == 0) {
				if (gradientFunc) {
					double t = ((double)x - dl) / dw;
					gradientFunc(0, t, res);
					innerR = res.r;
					innerG = res.g;
					innerB = res.b;
				}
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

							if (gradientFunc) {
								double t = ((double)x - dl) / dw;
								gradientFunc(0, t, res);
								innerR = res.r;
								innerG = res.g;
								innerB = res.b;
							}

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
}

void UILinearSlide::set_direction(SlideDirection newDir) {
	dir = newDir;
	update_slide_img();
	invalidate();
}

SlideDirection UILinearSlide::get_direction() const {
	return dir;
}

void UILinearSlide::on_init() {
	borderRadius.set(0);
	back.set("rgb(0, 0, 0, 0)");
	fill.set("#ddd");
	stroke.set("#777");
	padding.set(1);
	handleWidth = Min(Max(1, boxModel.width - 2), boxModel.height - 2);
	strokeWidth = 1.5;
	chosenHandle = &rcHandle;
	rcHandle.set(0, 0, handleWidth, handleWidth);
}

void UILinearSlide::on_update() {
	OUI::on_update();
	int l, t;
	if (dir == SlideDirection::HORIZONTAL) {
		l = contentArea.left + pad;
		t = contentArea.top + Max(0, (contentArea.height - slideImg.h) >> 1);
	}
	else {
		l = contentArea.left + Max(0, (contentArea.width - slideImg.w) >> 1);
		t = contentArea.top + pad;
	}

	canvas.bit_blt(slideImg, l, t, slideImg.w, slideImg.h, 0, 0, true);

	double cx, cy;
	cx = contentArea.left + rcHandle.width / 2.0 + rcHandle.left;
	cy = contentArea.top + rcHandle.height / 2.0 + rcHandle.top;

	Color c = stroke;
	if (bPressed && bHover)
		c = c.bright(-20);
	else if (bHover)
		c = c.bright(20);

	canvas.draw_circle(cx, cy, rad, strokeWidth, 2, back, c, fill);
}

void UILinearSlide::update_slide_img() {
	if (slideImg.is_useless()) return;
	slideImg.clear(0, 0, 0);

	Color strokeColor, fillColor, backColor;
	Spacing margin;
	BorderRadius borRadius;
	double strokeWidth;

	fillColor.set(backgroundColor.bright(-10));
	strokeColor.set(backgroundColor.bright(-30));

	margin.set(1);
	strokeWidth = 1;
	borRadius.set(Min(slideImg.w >> 1, slideImg.h >> 1));
	render_rounded_rect(slideImg, borRadius, margin, strokeWidth, backgroundColor, strokeColor, fillColor, gradientFunc, true);
}

void UILinearSlide::on_resize(int width, int height) {
	pad = int((Min(contentArea.width, contentArea.height) - 2) * .4);
	slideImg.resize(contentArea.width - int(2 * pad), contentArea.height - int(2 * pad));
	handleWidth = Min(Max(1, contentArea.width - 2), contentArea.height - 2);
	rad = handleWidth / 2.0;
	update_slide_img();
	on_range_update();
}

void* UILinearSlide::get_handle(int x, int y, uint32_t flags) {
	return &rcHandle;
}

double UILinearSlide::calc_value(void* handle, int x, int y, uint32_t flags) {
	double t = 0;
	if (dir == SlideDirection::HORIZONTAL) {
		double v = double(CLAMP3(minPos, x - (rcHandle.width >> 1), maxPos));
		t = v / double(maxPos - minPos);
	}
	return t;
}

void UILinearSlide::calc_handle_pos(void* handle, double t) {
	int left, top;
	if (dir == SlideDirection::HORIZONTAL) {
		top = (contentArea.height - handleWidth) >> 1;
		minPos = 0;
		maxPos = contentArea.width - handleWidth;
		left = t * (maxPos - minPos);
		left = CLAMP3(minPos, left, maxPos);
	}
	Rect* rc = (Rect*)handle;
	if (rc) rc->set(left, top, handleWidth, handleWidth);
}