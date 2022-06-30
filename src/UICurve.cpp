#include "UICurve.h"

void UICurve::get_histo(Sheet* image)
{
	int x, y, nbpp, p, irmax, igmax, ibmax, igraymax, gray;
	pyte data = image->data, d;
	nbpp = image->nbpp;
	int w = image->w;
	int h = image->h;
	p = image->pitch;

	for (int i = 0; i < 256; i++)
		gray_histo[i] = b_histo[i] = g_histo[i] = r_histo[i] = 0;

	for (y = 0; y < h; y++) {
		d = data + y * p;
		for (x = 0; x < w; x++)
		{
			gray = d[0] * 0.114 + d[1] * 0.587 + d[2] * 0.299;
			gray_histo[gray]++;
			b_histo[*d++]++;
			g_histo[*d++]++;
			r_histo[*d++]++;
		}
	}

	irmax = 1;
	igmax = 1;
	ibmax = 1;
	igraymax = 1;

	for (x = 0; x < 256; x++) {
		igraymax = Max(gray_histo[x], igraymax);
		ibmax = Max(b_histo[x], ibmax);
		igmax = Max(g_histo[x], igmax);
		irmax = Max(r_histo[x], irmax);
	}

	for (x = 0; x < 256; x++)
	{
		gray_histo[x] = ((double)gray_histo[x] / ibmax) * 255.0;
		b_histo[x] = ((double)b_histo[x] / ibmax) * 255.0;
		g_histo[x] = ((double)g_histo[x] / igmax) * 255.0;
		r_histo[x] = ((double)r_histo[x] / irmax) * 255.0;
	}
}

void UICurve::on_init() {
	img.create(256, 256, 3);
}

void UICurve::on_resize(int width, int height) {
	OUI::get_content_area(contentArea);
	img.resize(contentArea.width, contentArea.height);
	fill_image();
}

void UICurve::set_source(Sheet* srcImage) {

}

void UICurve::fill_image() {
	img.clear(0xff, 0xff, 0xff);
	Canvas can(0, &img);

	int divV = img.h / 4;
	int divH = img.w / 4;
	int h = img.h;
	int w = img.w;

	Color stroke(210, 210, 210);
	can.art.strokeColor.set("#eee");

	can.draw_vertical_line(divH, 0, h, 1);
	can.draw_vertical_line(divH * 2, 0, h, 1);
	can.draw_vertical_line(divH * 3, 0, h, 1);

	can.draw_horizontal_line(divV, 0, w, 1);
	can.draw_horizontal_line(divV * 2, 0, w, 1);
	can.draw_horizontal_line(divV * 3, 0, w, 1);

	int l, t; range = w - 4;
	imode = 0;
	unsigned long* histo = (imode == 0) ? gray_histo : (imode == 1) ? b_histo : (imode == 2) ? g_histo : (imode == 3) ? r_histo : gray_histo;
	l = t = 2;
	double s = (range - 1) / 255.0;

	can.art.strokeColor.set("#eee");

	for (int i = 0; i < range; i++)
		can.draw_vertical_line(l + i, range + t, range + t - histo[int((double)i / (range - 1) * 255)] * s + 0.5, 1);

	/*dcBack.SetDCPenColor(RGB(230, 230, 230));
	int x, y, ra = range - 1;

	dcBack.MoveTo(l, range - 1 + t); dcBack.LineTo(range - 1 + l, t);
	dcBack.SetPixel(range - 1 + l, t, crHisto);

	static Curve* cur[3] =
	{
		&red_curve,
		&green_curve,
		&blue_curve
	}, * curv;

	static DWORD crs[3] =
	{
		RGB(255, 0, 0),
		RGB(0, 255, 0),
		RGB(0, 0, 255)
	}, cr;

	if (!imode)
	{
		int i, q;
		if (bSmooth)
		{
			int maxx, minx;

			for (q = 0; q < 3; q++)
			{
				curv = cur[q];
				if (!curv->IsIntact()) continue;
				minx = curv->xs[0];
				maxx = curv->xs[curv->nCount - 1];
				cr = crs[q];
				dcBack.SetDCPenColor(cr);
				dcBack.MoveTo(2, curv->ys[0] + 2);

				for (i = minx; i <= maxx; i++)
				{
					y = min(max(curv->evalSpline(i), 0), ra);
					dcBack.LineTo(i + 2, y + 2);
				}

				dcBack.LineTo(range + 2, curv->ys[curv->nCount - 1] + 2);
			}
		}
		else
		{
			for (q = 0; q < 3; q++)
			{
				curv = cur[q];
				if (!curv->IsIntact())
					continue;

				cr = crs[q];
				dcBack.SetDCPenColor(cr);
				dcBack.MoveTo(2, curv->points_lookup[0] + 2);
				for (i = 0; i < range; i++)
				{
					y = curv->points_lookup[i] + 2.5;
					dcBack.LineTo(i + 2, y);
				}
			}
		}
	}*/
}

void UICurve::on_dbl_click(int x, int y, uint32_t flags) {
	on_mouse_down(x, y, flags);
}

void UICurve::on_mouse_down(int x, int y, uint32_t flags) {
	set_capture(this);
	bPressed = true;
}

void UICurve::on_mouse_move(int x, int y, uint32_t flags) {
	if (bPressed) {

	}
}

void UICurve::on_mouse_up(int x, int y, uint32_t flags) {
	if (get_capture() == this)
		release_capture();
	bPressed = false;
	invalidate();
}

void UICurve::on_update() {
	OUI::on_update();
	canvas.bit_blt(img, contentArea.left, contentArea.top, contentArea.width, contentArea.height, 0, 0, true);
}

void UICurve::apply_theme(bool bInvalidate) {
	OUI::apply_theme(bInvalidate);
}