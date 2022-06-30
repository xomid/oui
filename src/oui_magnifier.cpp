#pragma once
#include "oui_magnifier.h"

void UIMagnifier::move(int left, int top) {
	OUI::move(left, top);
	update_zoom_rect();
}

void UIMagnifier::move(int left, int top, int width, int height) {
	OUI::move(left, top, width, height);
	update_zoom_rect();
}

void UIMagnifier::update_zoom_rect() {
	scale = fmax(1., scale);
	int sw = int(area.width / scale);
	int sh = int(area.width / scale);
	int l = (area.width - sw) >> 1;
	int t = (area.height - sh) >> 1;
	zoomArea.set(area.left + l, area.top + t, sw, sh);
}

void UIMagnifier::set_scale(double scale) {
	this->scale = fmax(1., fmin(50, scale));
}

void UIMagnifier::on_init() {
	canvas.art.textColor.set(Colors::darkgray);
	set_scale(10);
	img.create(100, 100, 3);
	border.set(1, Colors::gray);
	add_box_shadow(false, 0, 0, 10, 0, Color("rgb(0, 0, 0, 0.2)"));
}

void UIMagnifier::on_resize(int width, int height) {
	img.resize(contentArea.width, contentArea.height);
}

void UIMagnifier::on_update() {
	if (!canvas.sheet || canvas.sheet->is_useless()) return;

	int dw, dh, sw, sh, dp, sp, dx, dy, sx, sy, ssx, ssy, mulx, muly, sright, sbottom;
	bool drawGrid;
	pyte dst, src, d, s, drow;
	byte lr, lg, lb, la, ca, r, g, b;

	dw = img.w;
	dh = img.h;
	dp = img.pitch;
	dst = img.data;

	sp = canvas.sheet->pitch;
	src = canvas.sheet->data;
	sright = canvas.sheet->w - 1;
	sbottom = canvas.sheet->h - 1;

	lr = 0xff;
	lg = 0xff;
	lb = 0xff;
	la = 0x4f;
	ca = 0xff - la;
	drawGrid = scale >= 20.;

	sw = Min(zoomArea.width, canvas.sheet->w);
	sh = Min(zoomArea.height, canvas.sheet->h);
	ssx = zoomArea.left;
	ssy = zoomArea.top;

	int lx, ly;
	lx = -1;
	ly = -1;

	if (!img.is_useless()) {
		for (dy = 0; dy < dh; ++dy) {
			drow = dst + dy * dp;
			muly = dy * sh;
			sy = muly / dh + ssy;
			lx = -1;
			for (dx = 0; dx < dw; ++dx) {
				d = drow + 3 * dx;
				mulx = dx * sw;
				sx = mulx / dw + ssx;
				s = src + sy * sp + 3 * sx;

				if (sy < 0 || sx < 0 || sx > sright || sy > sbottom)
					b = g = r = 0;
				else {
					b = *s++;
					g = *s++;
					r = *s;
				}

				if (drawGrid && (sx != lx || sy != ly)) {
					*d++ = CLAMP255(DIV255(lb * la + b * ca));
					*d++ = CLAMP255(DIV255(lg * la + g * ca));
					*d++ = CLAMP255(DIV255(lr * la + r * ca));
				}
				else {
					*d++ = b;
					*d++ = g;
					*d++ = r;
				}
				lx = sx;
			}
			ly = sy;
		}
	}

	if (outsetBoxShadows.size()) {
		canvas.bit_blt(outsetBoxShadowSheet,
			outsetBoxShadowSheet.offset.x,
			outsetBoxShadowSheet.offset.y,
			outsetBoxShadowSheet.w, outsetBoxShadowSheet.h, 0, 0, true);
	}

	canvas.bit_blt(img, contentArea.left, contentArea.top, img.w, img.h, 0, 0, true);
	Rect rcBorder(0, 0, area.width, area.height);
	canvas.draw_box(rcBorder, border, 0xff);

	sx = (area.width >> 1) + area.left;
	sy = (area.height >> 1) + area.top;
	s = src + sy * sp + 3 * sx;

	if (sy < 0 || sx < 0 || sx > sright || sy > sbottom)
		b = g = r = 0;
	else {
		b = *s++;
		g = *s++;
		r = *s;
	}

	auto sr = std::to_string(r);
	auto sg = std::to_string(g);
	auto sb = std::to_string(b);

	std::string scolor = "rgb(" + sr + ", " + sg + ", " + sb + ")";
	Rect rc(contentArea.left, contentArea.bottom() - 20, contentArea.width, 20);
	canvas.clear(&rc, (const Color*)&Colors::lightblue);
	canvas.draw_text8(scolor, rc);
}


