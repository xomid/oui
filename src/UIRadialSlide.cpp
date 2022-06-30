#include "UIRadialSlide.h"


void draw_circle_grad(Canvas& canvas, double cx, double cy, double radius, double strokeWidth, int strokeMode, // 0 outer, 1 mid, 2 inner
	Color& back, Color& stroke, Color& fill, GradientFunc gradFunc) {

	byte a, ca, resA, resR, resG, resB, resAA;
	pyte data = canvas.sheet->data, d;
	auto pitch = canvas.sheet->pitch;
	double outerRad, innerRad, dis, rem, outerRadCeil, innerRadCeil, disx, disy;
	int x, y, dy, w, h, sx, sy, dright, dbottom;

	outerRad = strokeWidth + radius;
	if (strokeMode == 1)
		outerRad = radius + strokeWidth / 2.;
	else if (strokeMode == 2)
		outerRad = radius;

	byte innerR = fill.r, innerG = fill.g, innerB = fill.b, innerA = fill.a, innerAA = 0xff - innerA,
		outerR = stroke.r, outerG = stroke.g, outerB = stroke.b, outerA = stroke.a, outerAA = 0xff - outerA,
		backR = back.r, backG = back.g, backB = back.b, backA = back.a, backAA = 0xff - innerA;

	innerRad = outerRad - strokeWidth;
	outerRadCeil = outerRad + 1;
	innerRadCeil = innerRad + 1;

	w = h = int(ceil(radius) * 2.);
	dright = canvas.sheet->w - 1;
	dbottom = canvas.sheet->h - 1;

	// areas:
	// outer empty
	// outer AA
	// outer solid
	// inner/outer common AA
	// inner solid

	if (canvas.area) {
		sx = canvas.area->left;
		sy = canvas.area->top;
	}
	else {
		sx = 0;
		sy = 0;
	}

	int fx = (cx - radius);
	int fy = (cy - radius);
	sx += fx;
	sy += fy;
	cx -= fx;
	cy -= fy;
	Color res;

	for (y = 0; y < h; ++y) {
		dy = y + sy;

		if (dy < 0 || dy > dbottom)
			continue;

		d = data + dy * pitch + sx * 3;
		for (x = 0; x < w; ++x) {
			disx = fabs((double)x - cx);
			disy = fabs((double)y - cy);
			if (x >= cx) disx += 1;
			if (y >= cy) disy += 1;
			dis = hypot(disx, disy);

			if (dis < outerRadCeil) {
				if (dis <= outerRad) {
					if (dis <= innerRadCeil) {
						if (gradFunc) {
							double t = (atan2((double)x - cx, (double)y - cy) + PI) / (PI * 2);
							gradFunc(0, t, res);
							innerR = res.r;
							innerG = res.g;
							innerB = res.b;
						}

						// inner circle solid
						if (dis <= innerRad) {
							if (innerA == 0xff) {
								*d++ = innerB;
								*d++ = innerG;
								*d++ = innerR;
							}
							else if (innerA > 0) {
								*d = CLAMP255(DIV255(innerA * innerB + innerAA * *d)); ++d;
								*d = CLAMP255(DIV255(innerA * innerG + innerAA * *d)); ++d;
								*d = CLAMP255(DIV255(innerA * innerR + innerAA * *d)); ++d;
							}
							else {
								d += 3;
							}
						}
						// inner/outer common AA
						else {
							rem = dis - innerRad;
							a = CLAMP255(int(rem * 0xff));
							ca = 0xff - a;

							a = DIV255(outerA * a);
							ca = DIV255(innerA * ca);
							resA = 0xff - DIV255((0xff - a) * (0xff - ca));

							if (resA == 0) {
								d += 3;
							}
							else {
								byte aa = 0xff - a;
								resR = (outerR * a * 255 / resA + innerR * ca * aa / resA) / 255;
								resG = (outerG * a * 255 / resA + innerG * ca * aa / resA) / 255;
								resB = (outerB * a * 255 / resA + innerB * ca * aa / resA) / 255;

								if (resA == 0xff) {
									*d++ = resB;
									*d++ = resG;
									*d++ = resR;
								}
								else if (resA > 0) {
									resAA = 0xff - resA;
									*d = CLAMP255(DIV255(resA * resB + resAA * *d)); ++d;
									*d = CLAMP255(DIV255(resA * resG + resAA * *d)); ++d;
									*d = CLAMP255(DIV255(resA * resR + resAA * *d)); ++d;
								}
							}
						}
					}
					// outer circle solid
					else {
						if (outerA == 0xff) {
							*d++ = outerB;
							*d++ = outerG;
							*d++ = outerR;
						}
						else if (outerA > 0) {
							*d = CLAMP255(DIV255(outerA * outerB + outerAA * *d)); ++d;
							*d = CLAMP255(DIV255(outerA * outerG + outerAA * *d)); ++d;
							*d = CLAMP255(DIV255(outerA * outerR + outerAA * *d)); ++d;
						}
						else {
							d += 3;
						}
					}
				}
				//outer circle AA part;
				else {
					// outerRad < dis < outerRad + 1
					rem = dis - outerRad;
					ca = CLAMP255(int(rem * 0xff));
					a = 0xff - ca;

					a = DIV255(outerA * a);
					ca = DIV255(backA * ca);
					resA = 0xff - DIV255((0xff - a) * (0xff - ca));

					if (resA == 0) {
						d += 3;
					}
					else {
						byte aa = 0xff - a;
						resR = (outerR * a * 255 / resA + backR * ca * aa / resA) / 255;
						resG = (outerG * a * 255 / resA + backG * ca * aa / resA) / 255;
						resB = (outerB * a * 255 / resA + backB * ca * aa / resA) / 255;

						if (resA == 0xff) {
							*d++ = resB;
							*d++ = resG;
							*d++ = resR;
						}
						else if (resA > 0) {
							resAA = 0xff - resA;
							*d = CLAMP255(DIV255(resA * resB + resAA * *d)); ++d;
							*d = CLAMP255(DIV255(resA * resG + resAA * *d)); ++d;
							*d = CLAMP255(DIV255(resA * resR + resAA * *d)); ++d;
						}
					}
				}
			}
			// empty outer circle
			else {
				if (backA == 0xff) {
					*d++ = backB;
					*d++ = backG;
					*d++ = backR;
				}
				else if (backA > 0) {
					*d = CLAMP255(DIV255(backA * backB + backAA * *d)); ++d;
					*d = CLAMP255(DIV255(backA * backG + backAA * *d)); ++d;
					*d = CLAMP255(DIV255(backA * backR + backAA * *d)); ++d;
				}
				else {
					d += 3;
				}
			}
		}
	}
}

void UIRadialSlide::on_init() {
	back.set("rgb(0, 0, 0, 0)");
	//fill.set("rgb(0, 0, 0, 0)");
	//fill.set("#ddd");
	padding.set(1);
	strokeWidth = 2;
	shift = 0;
	angle = 0;
}

void UIRadialSlide::apply_theme(bool bInvalidate) {
	fill.set(OUITheme::primary.bright(-10));
	stroke.set(fill.bright(-10));
	if (bInvalidate) invalidate();
}

void UIRadialSlide::on_update()
{
	canvas.art.thickness = 3;
	double ang = angle + shift;
	int hw = center.y - canvas.art.thickness - strokeWidth;

	int x = int(cos(ang) * hw + center.x);
	int y = int(sin(ang) * hw + center.y);

	double cx, cy;
	cx = center.x;
	cy = center.y;

	Color c = fill;
	Color s = stroke;
	if (bPressed && bHover) {
		c = c.bright(-5);
		s = s.bright(-5);
	}
	else if (bHover) {
		c = c.bright(+5);
		s = s.bright(+5);
	}

	canvas.art.strokeColor.set(s.bright(-100));
	draw_circle_grad(canvas, cx, cy, rad, strokeWidth, 2, back, s, c, gradientFunc);
	canvas.draw_line(cx, cy, x, y);
}

void UIRadialSlide::on_resize(int width, int height)
{
	center.x = width / 2;
	center.y = height / 2;
	rad = Min(width - 2, height - 2) / 2;
}

void UIRadialSlide::set_shift(double shift) {
	this->shift = shift;
	invalidate();
}

void* UIRadialSlide::get_handle(int x, int y, uint32_t flags) {
	return 0;
}

double UIRadialSlide::calc_value(void* handle, int x, int y, uint32_t flags) {
	double t = atan2((double)(y - center.y), (double)(x - center.x));
	double pi2 = PI + PI;
	t = t + pi2;
	t = fmod(t, pi2);
	t = t / pi2;
	return t;
}

void UIRadialSlide::calc_handle_pos(void* handle, double t) {
	angle = t * PI * 2;
	if (t > .5) angle = -(PI - (angle - PI));
}


