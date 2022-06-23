#include "oui_canvas.h"

Canvas::Canvas(Rect* area, Sheet* sheet) : Canvas() {
	set(area, sheet);
}

Canvas::~Canvas() {
	sl.reset_spans();
	ras.reset_clipping();
	ras.reset();
}

Canvas::Canvas() {
	sheet = NULL;
	area = NULL;
	set_line_height(1.5);
}

void render_scanline_aa_solid(scanline& sl, pixfmt_bgr& ren, Color& color)
{
	if (color.a == 0) return;
	int y = sl.y();
	unsigned num_spans = sl.num_spans();
	typename scanline::const_iterator span = sl.begin();

	for (;;)
	{
		int x = span->x;
		if (span->len > 0)
		{
			ren.blend_solid_hspan(x, y, (unsigned)span->len,
				color,
				span->covers);
		}
		else
		{
			ren.blend_hline(x, y, (unsigned)(x - span->len - 1),
				color,
				*(span->covers));
		}
		if (--num_spans == 0) break;
		++span;
	}
}

bool sweep_scanline(rasterizer_scanline& ras, scanline& sl)
{
	if (ras.m_scan_y > ras.m_outline.max_y()) return false;
	sl.reset_spans();
	unsigned num_cells = ras.m_outline.scanline_num_cells(ras.m_scan_y);
	const agg::cell_aa* const* cells = ras.m_outline.scanline_cells(ras.m_scan_y);
	int cover = 0;

	while (num_cells)
	{
		const agg::cell_aa* cur_cell = *cells;
		int x = cur_cell->x;
		int area = cur_cell->area;
		unsigned alpha;

		cover += cur_cell->cover;

		//accumulate all cells with the same X
		while (--num_cells)
		{
			cur_cell = *++cells;
			if (cur_cell->x != x) break;
			area += cur_cell->area;
			cover += cur_cell->cover;
		}

		if (area)
		{
			alpha = ras.calculate_alpha((cover << (agg::poly_subpixel_shift + 1)) - area);
			if (alpha == 0xff)
			{
				sl.add_cell(x, alpha);
			}
			x++;
		}

		if (num_cells && cur_cell->x > x)
		{
			alpha = ras.calculate_alpha(cover << (agg::poly_subpixel_shift + 1));
			if (alpha == 0xff)
			{
				sl.add_span(x, cur_cell->x - x, alpha);
			}
		}
	}

	sl.finalize(ras.m_scan_y);
	return true;
}

void render_scanlines(rasterizer_scanline& ras, scanline& sl, pixfmt_bgr& ren, Color& color, byte opacity)
{
	Color cr(color, opacity);
	if (cr.a == 0) return;
	if (ras.rewind_scanlines()) {
		sl.reset(ras.min_x(), ras.max_x());
		while (ras.sweep_scanline(sl)) {
			render_scanline_aa_solid(sl, ren, cr);
		}
	}
}

void clear_scanline_aa_solid_gray(scanline& sl, pixfmt_bgr& ren)
{
	int y = sl.y();
	unsigned num_spans = sl.num_spans();
	typename scanline::const_iterator span = sl.begin();

	for (;;)
	{
		int x = span->x;
		if (span->len > 0)
		{
			ren.clear_solid_hspan_gray(x, y, (unsigned)span->len,
				span->covers);
		}
		else
		{
			ren.clear_hline_gray(x, y, (unsigned)(x - span->len - 1),
				*(span->covers));
		}
		if (--num_spans == 0) break;
		++span;
	}
}

void clear_scanline_aa_solid_rgba(scanline& sl, pixfmt_bgr& ren, Color& color)
{
	int y = sl.y();
	unsigned num_spans = sl.num_spans();
	typename scanline::const_iterator span = sl.begin();

	for (;;)
	{
		int x = span->x;
		if (span->len > 0)
		{
			ren.clear_solid_hspan_bgra(x, y, (unsigned)span->len,
				color,
				span->covers);
		}
		else
		{
			ren.clear_hline_bgra(x, y, (unsigned)(x - span->len - 1),
				color,
				*(span->covers));
		}
		if (--num_spans == 0) break;
		++span;
	}
}

void clear_scanlines(rasterizer_scanline& ras, scanline& sl, pixfmt_bgr& ren, Color& color)
{
	int nbpp = ren.get_nbpp();
	if (ras.rewind_scanlines()) {
		sl.reset(ras.min_x(), ras.max_x());
		if (nbpp == OUI_GRAY) {
			while (ras.sweep_scanline(sl)) {
				clear_scanline_aa_solid_gray(sl, ren);
			}
		}
		else if (nbpp == OUI_BGRA) {
			while (ras.sweep_scanline(sl)) {
				clear_scanline_aa_solid_rgba(sl, ren, color);
			}
		}
		else return;
	}
}

FT_Face Canvas::add_font(std::wstring name, std::wstring path) {
	return SlotCacher::add_font(name, path);
}

void Canvas::set(Rect* area, Sheet* sheet) {
	this->area = area;
	this->sheet = sheet;
}

Sheet* Canvas::set_sheet(Sheet* newSheet) {
	Sheet* old = sheet;
	sheet = newSheet;
	return old;
}

Rect* Canvas::set_area(Rect* newArea) {
	Rect* temp = area;
	area = newArea;
	return temp;
}

void Canvas::bit_blt(Sheet& srcSheet, int dstx, int dsty, int width, int height, int srcLeft, int srcTop, Color& color) {
	int count, dw, dh, dpitch, dnbpp, sw, sh, spitch, snbpp, x, y, dx, dy, sx, sy;
	byte a, _a, cla, _cla, fa, da, red, grn, blu, alp, * s, * d, * dst, * src;

	dw = sheet->w;
	dh = sheet->h;
	dpitch = sheet->pitch;
	dnbpp = sheet->nbpp;
	dst = sheet->data;
	src = srcSheet.data;
	sw = srcSheet.w;
	sh = srcSheet.h;
	spitch = srcSheet.pitch;
	snbpp = srcSheet.nbpp;
	red = color.r;
	grn = color.g;
	blu = color.b;
	alp = color.a;
	sx = Max(srcLeft, 0);
	sy = Max(srcTop, 0);
	if (area) {
		dstx += area->left;
		dsty += area->top;
	}

	if (dnbpp == OUI_BGRA && snbpp == OUI_GRAY) {
		for (y = sy, dy = dsty; y < sh; y++, dy++) {
			if (dy < 0) continue;
			if (dy >= dh) break;

			dx = dstx;
			s = src + y * spitch + sx;
			d = dst + dy * dpitch + 4 * dx;
			sheet->clip(dx, dy, &count, &cla);
			_cla = 0xff - cla;

			for (x = sx; x < sw; x++, dx++, d += 4, s++, --count) {
				if (count < 1) {
					sheet->clip(dx, &count, &cla);
					_cla = 0xff - cla;
				}
				if (dx < 0) continue;
				if (dx >= dw) break;

				a = DIV255(DIV255(alp * *s) * cla);
				_a = 0xff - a;

				if (a == 0) continue;
				if (a == 0xff) {
					d[0] = blu;
					d[1] = grn;
					d[2] = red;
					d[3] = 0xff;
					continue;
				}

				da = d[3];
				fa = CLAMP255(a + DIV255(da * _a));
				d[3] = fa;

				if (fa == 0xff) {
					d[0] = byte(CLAMP255(DIV255(blu * a) + DIV255(DIV255(d[0] * da) * _a)));
					d[1] = byte(CLAMP255(DIV255(grn * a) + DIV255(DIV255(d[1] * da) * _a)));
					d[2] = byte(CLAMP255(DIV255(red * a) + DIV255(DIV255(d[2] * da) * _a)));
				}
				else {
					d[0] = byte(CLAMP255((DIV255(blu * a) + DIV255(DIV255(d[0] * da) * _a)) * 0xff / fa));
					d[1] = byte(CLAMP255((DIV255(grn * a) + DIV255(DIV255(d[1] * da) * _a)) * 0xff / fa));
					d[2] = byte(CLAMP255((DIV255(red * a) + DIV255(DIV255(d[2] * da) * _a)) * 0xff / fa));
				}
			}
		}
	}
}

void Canvas::draw_box_model(Rect* rect, Spacing* margin, Spacing* padding, Border* border) {
	if (!sheet || !sheet->data) return;
	int sx, sy, ex, ey, x, y, w, h, pitch, nbpp, count;
	byte red, grn, blu, cla, _cla, * d, * data, alpha, a, _a;

	w = sheet->w;
	h = sheet->h;
	pitch = sheet->pitch;
	nbpp = sheet->nbpp;
	data = sheet->data;

	// content: (0, 149, 210, 71)
	// margin : (255, 255, 0, 93)
	// padding: (23, 0, 174, 101)
	// lines:   (12, 141, 206, 255)

	if (!w || !h) return;

	int sx1, ex1, sy1, ey1, sx2, ex2, sy2, ey2;
	sx1 = rect->left;
	sy1 = rect->top;
	if (area) {
		sx1 += area->left;
		sy1 += area->top;
	}
	ex1 = sx1 + rect->width;
	ey1 = sy1 + rect->height;

	sx2 = sx1 + border->left + padding->left;
	sy2 = sy1 + border->top + padding->top;
	ex2 = sx2 + rect->width - (border->left + padding->left + border->right + padding->right);
	ey2 = sy2 + rect->height - (border->top + padding->top + border->bottom + padding->bottom);

	sx = rect->left - margin->left;
	sy = rect->top - margin->top;
	if (area) {
		sx += area->left;
		sy += area->top;
	}
	ex = sx + rect->width + margin->right + margin->left;
	ey = sy + rect->height + margin->top + margin->bottom;

	for (y = sy; y < ey; ++y) {
		if (y < 0) continue;
		if (y >= h) break;
		d = data + y * pitch + sx * 3;
		sheet->clip(sx, y, &count, &cla);
		_cla = 0xff - cla;

		for (x = sx; x < ex; ++x, d += 3, --count) {
			if (count < 1) {
				sheet->clip(x, &count, &cla);
				_cla = 0xff - cla;
			}
			if (x < 0) continue;
			if (x >= w) break;

			if (cla == 0)
				continue;

			if (x < sx1 || x >= ex1 || y < sy1 || y >= ey1) {
				red = 255;
				grn = 255;
				blu = 0;
				alpha = 93;
			}
			else if (x < sx2 || x >= ex2 || y < sy2 || y >= ey2) {
				red = 23;
				grn = 0;
				blu = 174;
				alpha = 101;
			}
			else {
				red = 0;
				grn = 149;
				blu = 210;
				alpha = 71;
			}

			a = DIV255(alpha * cla);
			_a = 0xff - a;
			d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
			d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
			d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
		}
	}

	art.strokeColor.set(12, 141, 206, 255);
	art.lineStyle = LineStyle::DASH_5x5;
	draw_vertical_line(sx1, 0, h, 1);
	draw_vertical_line(ex1 - 1, 0, h, 1);
	draw_horizontal_line(sy1, 0, w, 1);
	draw_horizontal_line(ey1 - 1, 0, w, 1);
}

void Canvas::clear(Rect* rect, const Color* color) {
	if (!sheet || !sheet->data) return;
	int sx, sy, ex, ey, px, x, y, cx, cy, w, h, pitch, nbpp, count;
	byte red, grn, blu, cla, _cla, * d, * data, alpha, calpha, a, _a;

	w = sheet->w;
	h = sheet->h;
	pitch = sheet->pitch;
	nbpp = sheet->nbpp;
	data = sheet->data;

	if (!w || !h) return;
	x = 0;
	y = 0;
	cx = sheet->w;
	cy = sheet->h;

	if (rect) {
		x = rect->left;
		y = rect->top;
		cx = rect->width;
		cy = rect->height;
	}
	if (area) {
		x += area->left;
		y += area->top;
		if (!rect) {
			cx = area->width;
			cy = area->height;
		}
	}
	red = color->r;
	grn = color->g;
	blu = color->b;
	alpha = color->a;
	calpha = 0xff - alpha;

	ex = CLAMP2(cx + x, w);
	ey = CLAMP2(cy + y, h);
	sx = CLAMP2(x, w - 1);
	sy = CLAMP2(y, h - 1);
	px = ((red & 0xff) << 16) | ((grn & 0xff) << 8) | (blu & 0xff);

	/*for (y = sy; y < ey; y++) {
		d = data + y * pitch + 3 * sx;
		for (x = sx; x < ex; x++, d += 3) {
			d[0] = blu;
			d[1] = grn;
			d[2] = red;
		}
	}*/

	/*for (y = sy; y < ey; y++) {
		d = data + y * pitch + 3 * sx;
		sheet->clip(sx, y, &count, &cla);
		_cla = 0xff - cla;

		for (x = sx; x < ex; x++, d += 3, --count) {
			if (count < 1) {
				sheet->clip(x, &count, &cla);
				_cla = 0xff - cla;
			}

			if (cla == 0) {
				continue;
			}
			else if (cla == 0xff && alpha == 0xff) {
				d[0] = blu;
				d[1] = grn;
				d[2] = red;
			}
			else {
				a = DIV255(alpha * cla);
				_a = 0xff - a;
				d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
				d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
				d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
			}
		}
	}*/

	for (y = sy; y < ey; y++) {
		d = data + y * pitch + 3 * sx;
		sheet->clip(sx, y, &count, &cla);
		_cla = 0xff - cla;

		for (x = sx; x < ex; ) {
			if (count < 1) {
				sheet->clip(x, &count, &cla);
				if (count < 1) break;
				_cla = 0xff - cla;
			}

			if (cla == 0) {
				x += count, d += 3 * count;
				count = 0;
			}
			else if (cla == 0xff && alpha == 0xff) {
				for (; x < ex && count > 0; x++, d += 3, --count) {
					d[0] = blu;
					d[1] = grn;
					d[2] = red;
				}
			}
			else {
				a = DIV255(alpha * cla);
				_a = 0xff - a;
				for (; x < ex && count > 0; x++, d += 3, --count) {
					d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
					d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
					d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
				}
			}
		}
	}
}

void Canvas::draw_line(int x0, int y0, int x1, int y1) {
	if (!sheet || !area) return;
	pixfmt_bgr pf(sheet, area);
	agg::path_storage path;
	agg::conv_stroke<agg::path_storage> stroke(path);
	path.move_to(x0, y0);
	path.line_to(x1, y1);
	stroke.width(art.thickness);
	stroke.line_join(agg::line_join_e::miter_join_round);
	stroke.line_cap(agg::line_cap_e::round_cap);
	stroke.inner_join(agg::inner_join_e::inner_round);
	stroke.inner_miter_limit(1.01);
	reset();
	add_path<agg::conv_stroke<agg::path_storage>>(stroke);
	render(art.strokeColor);
}

void Canvas::clear_bystencil(Rect* rect, Border* bor, Color* background, rasterizer_scanline& sras, scanline& ssl)
{

	if (!sheet || !sheet->data) return;
	int sx, sy, ex, ey, dx, dy, x, y, cx, cy, w, h, pitch, nbpp, count;
	byte balp, bcalp, alp, calp, * d, * data;
	byte bred, bgrn, bblu, red, grn, blu, cla, _cla;
	int ibred, ibgrn, ibblu, ired, igrn, iblu;

	w = sheet->w;
	h = sheet->h;
	pitch = sheet->pitch;
	nbpp = sheet->nbpp;
	data = sheet->data;

	if (!w || !h) return;
	x = rect->left;
	y = rect->top;
	if (area) {
		x += area->left;
		y += area->top;
	}
	cx = rect->width;
	cy = rect->height;

	bred = bor->leftColor.r;
	bgrn = bor->leftColor.g;
	bblu = bor->leftColor.b;
	balp = bor->leftColor.a;
	bcalp = 0xff - balp;

	red = background->r;
	grn = background->g;
	blu = background->b;
	alp = background->a;
	calp = 0xff - alp;

	ired = red * alp;
	igrn = grn * alp;
	iblu = blu * alp;

	ibred = bred * balp;
	ibgrn = bgrn * balp;
	ibblu = bblu * balp;

	ex = cx + x;
	ey = cy + y;
	sx = x;
	sy = y;

	dy = CLAMP2(sy + bor->top, h - 1);
	for (y = sy; y < dy; y++) {
		if (y < 0) continue;
		if (y >= h) break;
		d = data + y * pitch + 3 * sx;
		sheet->clip(sx, y, &count, &cla);
		_cla = 0xff - cla;

		for (x = sx; x < ex; x++, d += 3, --count) {
			if (x < 0) continue;
			if (x >= w) break;
			if (count < 1) {
				sheet->clip(x, &count, &cla);
				_cla = 0xff - cla;
			}

			if (cla == 0) {
				continue;
			}
			else if (cla == 0xff) {
				d[0] = bblu;
				d[1] = bgrn;
				d[2] = bred;
			}
			else {
				d[0] = CLAMP255(DIV255(bblu * cla + (d[0] * _cla)));
				d[1] = CLAMP255(DIV255(bgrn * cla + (d[1] * _cla)));
				d[2] = CLAMP255(DIV255(bred * cla + (d[2] * _cla)));
			}
		}
	}

	for (y = ey - bor->bottom; y < ey; y++) {
		if (y < 0) continue;
		if (y >= h) break;
		d = data + y * pitch + 3 * sx;
		sheet->clip(sx, y, &count, &cla);
		_cla = 0xff - cla;

		for (x = sx; x < ex; x++, d += 3, --count) {
			if (x < 0) continue;
			if (x >= w) break;
			if (count < 1) {
				sheet->clip(x, &count, &cla);
				_cla = 0xff - cla;
			}

			if (cla == 0) {
				continue;
			}
			else if (cla == 0xff) {
				d[0] = bblu;
				d[1] = bgrn;
				d[2] = bred;
			}
			else {
				d[0] = CLAMP255(DIV255(bblu * cla + (d[0] * _cla)));
				d[1] = CLAMP255(DIV255(bgrn * cla + (d[1] * _cla)));
				d[2] = CLAMP255(DIV255(bred * cla + (d[2] * _cla)));
			}
		}
	}

	ey -= bor->bottom;
	dx = Min(Max(sx + bor->left, 0), w - 1);
	for (y = sy + bor->top; y < ey; y++) {
		if (y < 0) continue;
		if (y >= h) break;
		d = data + y * pitch + 3 * sx;
		sheet->clip(sx, y, &count, &cla);
		_cla = 0xff - cla;

		for (x = sx; x < dx; x++, d += 3, --count) {
			if (x < 0) continue;
			if (x >= w) break;
			if (count < 1) {
				sheet->clip(x, &count, &cla);
				_cla = 0xff - cla;
			}

			if (cla == 0) {
				continue;
			}
			else if (cla == 0xff) {
				d[0] = bblu;
				d[1] = bgrn;
				d[2] = bred;
			}
			else {
				d[0] = CLAMP255(DIV255(bblu * cla + (d[0] * _cla)));
				d[1] = CLAMP255(DIV255(bgrn * cla + (d[1] * _cla)));
				d[2] = CLAMP255(DIV255(bred * cla + (d[2] * _cla)));
			}
		}
	}

	for (y = sy + bor->top; y < ey; y++) {
		if (y < 0) continue;
		if (y >= h) break;
		x = Min(Max(ex - bor->right, 0), w);
		d = data + y * pitch + 3 * x;
		sheet->clip(x, y, &count, &cla);
		_cla = 0xff - cla;

		for (; x < ex; x++, d += 3, --count) {
			if (x < 0) continue;
			if (x >= w) break;
			if (count < 1) {
				sheet->clip(x, &count, &cla);
				_cla = 0xff - cla;
			}

			if (cla == 0) {
				continue;
			}
			else if (cla == 0xff) {
				d[0] = bblu;
				d[1] = bgrn;
				d[2] = bred;
			}
			else {
				d[0] = CLAMP255(DIV255(bblu * cla + (d[0] * _cla)));
				d[1] = CLAMP255(DIV255(bgrn * cla + (d[1] * _cla)));
				d[2] = CLAMP255(DIV255(bred * cla + (d[2] * _cla)));
			}
		}
	}

	ex -= bor->right;
	sx += bor->left;

	if (alp < 0xff) {

		for (y = sy + bor->top; y < ey; y++) {
			if (y < 0) continue;
			if (y >= h) break;

			d = data + y * pitch + 3 * sx;
			sheet->clip(sx, y, &count, &cla);
			_cla = 0xff - cla;

			for (x = sx; x < ex; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					_cla = 0xff - cla;
				}

				if (cla == 0) {
					continue;
				}
				else {
					int r, g, b;

					b = CLAMP255(DIV255(iblu + (d[0] * calp)));
					g = CLAMP255(DIV255(igrn + (d[1] * calp)));
					r = CLAMP255(DIV255(ired + (d[2] * calp)));

					d[0] = CLAMP255(DIV255(b * cla + (d[0] * _cla)));
					d[1] = CLAMP255(DIV255(g * cla + (d[1] * _cla)));
					d[2] = CLAMP255(DIV255(r * cla + (d[2] * _cla)));
				}
			}
		}
	}
	else {

		int minx, maxx, miny, maxy;
		minx = sras.min_x();
		maxx = sras.max_x();
		miny = sras.min_y();
		maxy = sras.max_y();

		for (y = sy + bor->top; y < ey; y++) {
			if (y < 0) continue;
			if (y >= h) break;
			d = data + y * pitch + 3 * sx;
			if (y < miny || y >= maxy) {
				for (x = sx; x < ex; x++, d += 3) {
					if (x < 0) continue;
					if (x >= w) break;
					d[0] = blu;
					d[1] = grn;
					d[2] = red;
				}
			}
			else if (sras.rewind_scanlines())
			{
				ssl.reset(minx, maxx);
				sras.navigate_scanline(y);
				unsigned num_cells = sras.m_outline.scanline_num_cells(y);
				if (!num_cells) {
					for (x = sx; x < ex; x++, d += 3) {
						if (x < 0) continue;
						if (x >= w) break;
						d[0] = blu;
						d[1] = grn;
						d[2] = red;
					}
				}
				else if (sweep_scanline(sras, ssl)) {
					int num_spans = ssl.num_spans();
					scanline::const_iterator span = ssl.begin();

					for (x = sx; x < ex; ) {
						if (x < 0) continue;
						if (x >= w) break;

						if (num_spans < 1) {
							for (; x < ex; x++, d += 3) {
								d[0] = blu;
								d[1] = grn;
								d[2] = red;
							}
						}
						else {
							for (; num_spans > 0; ) {
								int len = span->len;
								agg::int8u* covers = span->covers;

								if (x < span->x) {
									int e = x < span->x ? span->x : ex;
									for (; x < e; x++, d += 3) {
										d[0] = blu;
										d[1] = grn;
										d[2] = red;
									}
								}

								if (len > 0) {
									if (len < 1) {
										--num_spans;
										++span;
										continue;
									}

									x += len;
									d += len * 3;
								}
								else {
									len = span->x - len - 1;
									if (len < 1) {
										--num_spans;
										++span;
										continue;
									}

									x += len;
									d += len * 3;
								}

								if (--num_spans < 1) break;
								++span;
							}
						}
					}
				}
			}
		}
	}
}

void Canvas::draw_vertical_line(int x, int y0, int y1, size_t width) {
	if (!sheet || !sheet->data) return;
	int sx, sy, ex, ey, y, w, h, pitch, nbpp, count;
	byte alpha, calpha, * d, * data, red, blu, grn, cla, _cla, a, _a;

	w = sheet->w;
	h = sheet->h;
	pitch = sheet->pitch;
	nbpp = sheet->nbpp;
	data = sheet->data;

	if (!w || !h) return;
	if (!width) width = art.thickness;

	red = art.strokeColor.r;
	grn = art.strokeColor.g;
	blu = art.strokeColor.b;
	alpha = art.strokeColor.a;
	calpha = 0xff - alpha;

	if (area) {
		x += area->left;
		y0 += area->top;
		y1 += area->top;
	}

	sy = CLAMP2(y0, h - 1);
	ey = CLAMP2(y1, h);

	sx = (int)(x - width / 2);
	ex = CLAMP2((int)(sx + width), w);
	if (sx >= w || ex < 1) return;
	sx = CLAMP2(sx, w - 1);

	if (sx > ex) SWAP(sx, ex);
	if (sy > ey) SWAP(sy, ey);

	if (art.lineStyle == LineStyle::DASH_5x5) {
		int shift = art.shiftPatternY;
		for (y = sy; y < ey; y++) {
			if ((10 + ((y - y0 - shift) % 10)) % 10 < 5) {
				d = data + y * pitch + 3 * sx;
				sheet->clip(sx, y, &count, &cla);
				_cla = 0xff - cla;
				for (x = sx; x < ex; x++, d += 3, --count) {
					if (count < 1) {
						sheet->clip(x, &count, &cla);
						_cla = 0xff - cla;
					}

					if (cla == 0) {
						continue;
					}
					else if (cla == 0xff && alpha == 0xff) {
						d[0] = blu;
						d[1] = grn;
						d[2] = red;
					}
					else {
						a = DIV255(alpha * cla);
						_a = 0xff - a;
						d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
						d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
						d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
					}
				}
			}
		}
	}
	else {
		for (y = sy; y < ey; y++) {
			d = data + y * pitch + 3 * sx;
			sheet->clip(sx, y, &count, &cla);
			_cla = 0xff - cla;
			for (x = sx; x < ex; x++, d += 3, --count) {
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					_cla = 0xff - cla;
				}
				if (cla == 0) {
					continue;
				}
				else if (cla == 0xff && alpha == 0xff) {
					d[0] = blu;
					d[1] = grn;
					d[2] = red;
				}
				else {
					a = DIV255(alpha * cla);
					_a = 0xff - a;
					d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
					d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
					d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
				}
			}
		}
	}
}

void Canvas::draw_horizontal_line(int y, int x0, int x1, size_t width) {
	if (!sheet || !sheet->data) return;
	int sx, sy, ex, ey, x, w, h, pitch, nbpp, count;
	byte alpha, calpha, * d, * data, red, blu, grn, cla, _cla, a, _a;

	w = sheet->w;
	h = sheet->h;
	pitch = sheet->pitch;
	nbpp = sheet->nbpp;
	data = sheet->data;

	if (!w || !h) return;
	if (!width) width = art.thickness;

	red = art.strokeColor.r;
	grn = art.strokeColor.g;
	blu = art.strokeColor.b;
	alpha = art.strokeColor.a;
	calpha = 0xff - alpha;

	if (area) {
		y += area->top;
		x0 += area->left;
		x1 += area->left;
	}

	sx = CLAMP2(x0, w - 1);
	ex = CLAMP2(x1, w);

	sy = (int)(y - width / 2);
	ey = CLAMP2((int)(sy + width), h);
	if (sy >= h || ey < 1) return;
	sy = CLAMP2(sy, h - 1);

	if (sx > ex) SWAP(sx, ex);
	if (sy > ey) SWAP(sy, ey);

	if (art.lineStyle == LineStyle::DASH_5x5) {
		int shift = art.shiftPatternX;
		for (y = sy; y < ey; y++) {
			d = data + y * pitch + 3 * sx;
			sheet->clip(sx, y, &count, &cla);
			_cla = 0xff - cla;
			for (x = sx; x < ex; x++, d += 3, --count) {
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					_cla = 0xff - cla;
				}

				if ((10 + ((x - x0 - shift) % 10)) % 10 < 5) {
					if (cla == 0) {
						continue;
					}
					else if (cla == 0xff && alpha == 0xff) {
						d[0] = blu;
						d[1] = grn;
						d[2] = red;
					}
					else {
						a = DIV255(alpha * cla);
						_a = 0xff - a;
						d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
						d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
						d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
					}
				}
			}
		}
	}
	else {
		for (y = sy; y < ey; y++) {
			d = data + y * pitch + 3 * sx;
			sheet->clip(sx, y, &count, &cla);
			_cla = 0xff - cla;
			for (x = sx; x < ex; x++, d += 3, --count) {
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					_cla = 0xff - cla;
				}

				if (cla == 0) {
					continue;
				}
				else if (cla == 0xff && alpha == 0xff) {
					d[0] = blu;
					d[1] = grn;
					d[2] = red;
				}
				else {
					a = DIV255(alpha * cla);
					_a = 0xff - a;
					d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
					d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
					d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
				}
			}
		}
	}
}

void Canvas::fill_rounded_rect(Rect* rect, size_t radius, Color* color) {
	if (!sheet) return;

	int x = rect->left;
	int y = rect->top;
	int cx = rect->width;
	int cy = rect->height;

	int margin = 0;
	double l = (double)(x + margin),
		r = (double)(x + cx - margin),
		t = (double)(y + margin),
		b = (double)(y + cy - margin);

	agg::rounded_rect roundedRect(l, t, r, b, (double)radius);
	roundedRect.normalize_radius();
	ras.add_path(roundedRect);
	render(ras, sl, *color);
}

void Canvas::draw_rounded_rect(Rect* rect, size_t radius, Color* color) {
	if (!sheet) return;

	int x = rect->left;
	int y = rect->top;
	int cx = rect->width;
	int cy = rect->height;

	int margin = 0;
	double l = (double)(x + margin),
		r = (double)(x + cx - margin),
		t = (double)(y + margin),
		b = (double)(y + cy - margin);

	agg::rounded_rect roundedRect(l, t, r, b, (double)radius);
	agg::conv_stroke<agg::rounded_rect> stroke(roundedRect);
	roundedRect.normalize_radius();
	stroke.width(art.thickness);
	ras.add_path(stroke);
	render(ras, sl, *color);
}

void Canvas::reset() {
	ras.reset();
}

void Canvas::clear_solid(ShapeStorage& shape, int offsetX, int offsetY, Color& color) {
	//agg::trans_affine mtx;
	//mtx *= agg::trans_affine_translation(offsetX, offsetY);
	//trans_shape trans(shape, mtx);
	//clear_solid(trans, color);
}

void Canvas::clear_solid(trans_shape& shape, Color& color) {
	//pixfmt_bgr pf(sheet, area);
	//ras.add_path(shape);
	//clear_scanlines(ras, sl, pf, color);
}

void Canvas::render(Color color, byte opacity) {
	pixfmt_bgr pf(sheet, area);
	render_scanlines(ras, sl, pf, color, opacity);
}

void Canvas::render(rasterizer_scanline& ras, scanline& sl, Color color, byte opacity) {
	pixfmt_bgr pf(sheet, area);
	render_scanlines(ras, sl, pf, color, opacity);
}

void Canvas::draw_box(Rect& rect, Border bor, byte opacity) {
	if (!sheet || !sheet->data) return;
	int sx, sy, ex, ey, dx, dy, x, y, cx, cy, w, h, pitch, nbpp, count;
	byte* d, * data, cla, a, p, borLA, borLP, borTA, borTP, borRA, borRP, borBA, borBP;

	w = sheet->w;
	h = sheet->h;
	pitch = sheet->pitch;
	nbpp = sheet->nbpp;
	data = sheet->data;

	if (!w || !h) return;
	x = rect.left;
	y = rect.top;
	if (area) {
		x += area->left;
		y += area->top;
	}
	cx = rect.width;
	cy = rect.height;

	ex = cx + x;
	ey = cy + y;
	sx = x;
	sy = y;

	borTA = DIV255(bor.topColor.a * opacity);
	if (borTA) {
		borTP = 0xff - borTA;
		dy = CLAMP2(sy + bor.top, h - 1);
		for (y = sy; y < dy; y++) {
			if (y < 0) continue;
			if (y >= h) break;
			d = data + y * pitch + 3 * sx;
			sheet->clip(sx, y, &count, &cla);
			if (!cla) continue;

			for (x = sx; x < ex; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					if (!cla) continue;
				}

				a = DIV255(cla * borTA);
				if (INT_IS_ZERO(a)) {
					continue;
				}
				else if (BYTE_OPAQUE(a)) {
					d[0] = bor.topColor.b;
					d[1] = bor.topColor.g;
					d[2] = bor.topColor.r;
				}
				else {
					p = 0xff - a;
					d[0] = CLAMP255(DIV255(bor.topColor.b * a + (d[0] * p)));
					d[1] = CLAMP255(DIV255(bor.topColor.g * a + (d[1] * p)));
					d[2] = CLAMP255(DIV255(bor.topColor.r * a + (d[2] * p)));
				}
			}
		}
	}

	borBA = DIV255(bor.botColor.a * opacity);
	if (borBA) {
		borBP = 0xff - borBA;
		for (y = ey - bor.bottom; y < ey; y++) {
			if (y < 0) continue;
			if (y >= h) break;
			d = data + y * pitch + 3 * sx;
			sheet->clip(sx, y, &count, &cla);
			if (!cla) continue;

			for (x = sx; x < ex; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					if (!cla) continue;
				}

				a = DIV255(cla * borBA);
				if (INT_IS_ZERO(a)) {
					continue;
				}
				else if (BYTE_OPAQUE(a)) {
					d[0] = bor.botColor.b;
					d[1] = bor.botColor.g;
					d[2] = bor.botColor.r;
				}
				else {
					p = 0xff - a;
					d[0] = CLAMP255(DIV255(bor.botColor.b * a + (d[0] * p)));
					d[1] = CLAMP255(DIV255(bor.botColor.g * a + (d[1] * p)));
					d[2] = CLAMP255(DIV255(bor.botColor.r * a + (d[2] * p)));
				}
			}
		}
	}

	ey -= bor.bottom;
	dx = Min(Max(sx + bor.left, 0), w - 1);

	borLA = DIV255(bor.leftColor.a * opacity);
	if (borLA) {
		borLP = 0xff - borLA;
		for (y = sy + bor.top; y < ey; y++) {
			if (y < 0) continue;
			if (y >= h) break;
			d = data + y * pitch + 3 * sx;
			sheet->clip(sx, y, &count, &cla);
			if (!cla) continue;

			for (x = sx; x < dx; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					if (!cla) continue;
				}

				a = DIV255(cla * borLA);
				if (INT_IS_ZERO(a)) {
					continue;
				}
				else if (BYTE_OPAQUE(a)) {
					d[0] = bor.leftColor.b;
					d[1] = bor.leftColor.g;
					d[2] = bor.leftColor.r;
				}
				else {
					p = 0xff - a;
					d[0] = CLAMP255(DIV255(bor.leftColor.b * a + (d[0] * p)));
					d[1] = CLAMP255(DIV255(bor.leftColor.g * a + (d[1] * p)));
					d[2] = CLAMP255(DIV255(bor.leftColor.r * a + (d[2] * p)));
				}
			}
		}
	}

	borRA = DIV255(bor.rightColor.a * opacity);
	if (borRA) {
		borRP = 0xff - borRA;
		int _sx = Min(Max(ex - bor.right, 0), w);
		dx = ex;

		for (y = sy + bor.top; y < ey; y++) {
			if (y < 0) continue;
			if (y >= h) break;
			d = data + y * pitch + 3 * _sx;
			sheet->clip(sx, y, &count, &cla);
			if (!cla) continue;

			for (x = _sx; x < dx; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					if (!cla) continue;
				}

				a = DIV255(cla * borRA);
				if (INT_IS_ZERO(a)) {
					continue;
				}
				else if (BYTE_OPAQUE(a)) {
					d[0] = bor.rightColor.b;
					d[1] = bor.rightColor.g;
					d[2] = bor.rightColor.r;
				}
				else {
					p = 0xff - a;
					d[0] = CLAMP255(DIV255(bor.rightColor.b * a + (d[0] * p)));
					d[1] = CLAMP255(DIV255(bor.rightColor.g * a + (d[1] * p)));
					d[2] = CLAMP255(DIV255(bor.rightColor.r * a + (d[2] * p)));
				}
			}
		}
	}
}

void Canvas::set_line_height(double lineHeight) {
	this->lineHeight = lineHeight;
}

void Canvas::set_color(const Color& crText) {
	art.textColor.set(crText);
}

size_t Canvas::get_ch() {
	return faceHandle.get_ch();
}

size_t Canvas::get_max_height() {
	return faceHandle.get_max_height();
}

TextBox* Canvas::get_box8(char* text, size_t len) {
	SlotCacher::load_box8(text, len, faceHandle);
	return &faceHandle.box;
}

TextBox* Canvas::get_box16(wchar_t* text, size_t len) {
	SlotCacher::load_box16(text, len, faceHandle);
	return &faceHandle.box;
}

inline void Canvas::get_size8(char* text, size_t len, Size* size, bool multiLine) {
	if (IS_NULL(size)) return;
	int textW = 0, textH = 0;

	if (multiLine) {
		int maxH = (int)faceHandle.get_max_height(), absLineHeight = int(maxH * this->lineHeight);
		char* b, * e;
		size_t i = 0;
		while (true) {
			b = text + i;
			if (multiLine) e = (char*)memchr(b, '\n', len - i);
			else e = text + len;
			SlotCacher::load_box8(b, e ? (e - b - 1) : int(len - i), faceHandle);
			auto& box = faceHandle.box;
			textW = Max(textW, box.get_width());
			textH += e ? absLineHeight : box.get_height();
			if (!e || !multiLine) break;
			i = i + e - b + 1;
		}
	}
	else {
		SlotCacher::load_box8(text, len, faceHandle);
		auto& box = faceHandle.box;
		textW = box.get_width();
		textH = box.get_height();
	}

	size->width = textW;
	size->height = textH;
}

inline void Canvas::get_size16(wchar_t* text, size_t len, Size* size, bool multiLine) {
	if (IS_NULL(size)) return;
	int textW = 0, textH = 0;

	if (multiLine) {
		int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight);
		wchar_t* b, * e;
		size_t i = 0;
		while (true) {
			b = text + i;
			if (multiLine) e = wmemchr(b, L'\n', len - i);
			else e = text + len;
			SlotCacher::load_box16(b, e ? (e - b - 1) : len - i, faceHandle);
			auto& box = faceHandle.box;
			textW = Max(textW, box.get_width());
			textH += e ? absLineHeight : box.get_height();
			if (!e || !multiLine) break;
			i = i + e - b + 1;
		}
	}
	else {
		SlotCacher::load_box16(text, len, faceHandle);
		auto& box = faceHandle.box;
		textW = box.get_width();
		textH = box.get_height();
	}

	size->width = textW;
	size->height = textH;
}

inline void Canvas::_draw_glyphs(int penLeft, int penTop, Rect* clip) {
	// it should be called after calling shape and load_box
	int x, y, ix, iy, dy, dix, diy, posx, posy, w, h, pitch, nbpp, glyphw, glyphh, glyphPitch, count;
	byte a, p, red, grn, blu, alp, cla, clp;
	pyte d, s, data, glyphData;
	short px;
	if (area) {
		penLeft += area->left;
		penTop += area->top;
	}

	w = sheet->w - 1;
	h = sheet->h - 1;
	pitch = sheet->pitch;
	nbpp = sheet->nbpp;
	data = sheet->data;
	red = art.textColor.r;
	grn = art.textColor.g;
	blu = art.textColor.b;
	alp = art.textColor.a;
	px = (art.textColor.b | art.textColor.g << 8);

	auto gpos = faceHandle.gpos;
	auto ginfo = faceHandle.ginfo;
	auto gcount = faceHandle.gcount;
	posx = posy = 0;
	int sx, sy, ex, ey;

	if (clip) {
		sx = clip->left,
			sy = clip->top,
			ex = clip->width - 1,
			ey = clip->height - 1;
		if (area) {
			sx += area->left;
			sy += area->top;
		}
		ex = ex + sx;
		ey = ey + sy;

		sx = CLAMP3(0, sx, w);
		sy = CLAMP3(0, sy, h);
		ex = CLAMP3(0, ex, w);
		ey = CLAMP3(0, ey, h);
	}
	else {
		sx = 0, sy = 0, ex = w, ey = h;
	}

	for (size_t g = 0; g < gcount; g++) {
		auto* slot = SlotCacher::load_glyph(ginfo[g].codepoint, faceHandle);
		if (!slot) break;
		ix = posx + __SL(gpos[g].x_offset + slot->metrics.horiBearingX);
		iy = posy + __SL(gpos[g].y_offset + slot->metrics.horiBearingY);
		posx += __SL(gpos[g].x_advance);
		posy += __SL(gpos[g].y_advance);

		if ((posx + penLeft) < sx || (ix + penLeft) > ex) continue;

		glyphw = slot->bitmap.width;
		glyphh = slot->bitmap.rows;
		glyphPitch = slot->bitmap.pitch;
		glyphData = slot->bitmap.buffer;
		y = 0, dy = (penTop - iy);

		if (dy < sy) {
			int ddy = sy - dy;
			y += ddy;
			dy += ddy;
		}

		for (; y < glyphh; y++, dy++) {
			dix = ix + penLeft;
			diy = dy;
			x = 0;

			if (diy > ey) break;

			if (dix < sx) {
				int dx = sx - dix;
				dix += dx;
				dx = Min(dx, glyphPitch);
				x += dx;
			}
			if (dix > ex) break;

			d = data + diy * pitch + 3 * dix;
			s = glyphData + y * glyphPitch + x;
			sheet->clip(dix, diy, &count, &cla);
			clp = 0xff - cla;

			for (; x < glyphw; x++, s++, dix++, d += 3, --count) {
				if (dix < sx) continue;
				if (dix > ex) break;

				if (count < 1) {
					sheet->clip(dix, &count, &cla);
					clp = 0xff - cla;
				}

				if (cla == 0 || *s == 0) {
					continue;
				}
				else {
					a = DIV255(DIV255(cla * *s) * alp);
					p = 0xff - a;

					d[0] = DIV255(a * blu) + DIV255(p * d[0]);
					d[1] = DIV255(a * grn) + DIV255(p * d[1]);
					d[2] = DIV255(a * red) + DIV255(p * d[2]);
				}
			}
		}
	}
}

int Canvas::draw_text8(char* text, Rect& rc, bool multiLine) {
	Size textSize;
	size_t len = strlen(text);
	if (!len || !text) return 0;
	get_size8(text, len, &textSize, multiLine);
	int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight),
		penLeft = rc.left, penTop = rc.top + maxH;

	if (art.alignY == Align::CENTER)
		penTop += (rc.height - textSize.height) / 2;
	else if (art.alignY == Align::BOTTOM)
		penTop += rc.height - textSize.height;

	char* b, * e;
	size_t i = 0;
	int textW;
	while (true) {
		b = text + i;
		if (multiLine) e = (char*)memchr(b, '\n', len - i);
		else e = text + len;
		size_t lineLen = e ? size_t(e - b) : len - i;

		if (lineLen > 0) {
			SlotCacher::load_box8(b, lineLen, faceHandle);
			auto& box = faceHandle.box;
			textW = box.get_width();

			penLeft = rc.left;
			if (art.alignX == Align::CENTER)
				penLeft += (rc.width - textW) / 2;
			else if (art.alignX == Align::RIGHT)
				penLeft += rc.width - textW;

			_draw_glyphs(penLeft, penTop, &rc);
		}

		penTop += absLineHeight;
		if (!e || !multiLine) break;
		i = i + e - b + 1;
	}

	return textSize.width;
}

int Canvas::draw_text16(wchar_t* text, Rect& rc, bool multiLine) {
	Size textSize;
	size_t len = wcslen(text);
	if (!len || !text) return 0;
	get_size16(text, len, &textSize, multiLine);
	int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight),
		penLeft = rc.left, penTop = rc.top + maxH;

	if (art.alignY == Align::CENTER)
		penTop += (rc.height - textSize.height) / 2;
	else if (art.alignY == Align::BOTTOM)
		penTop += rc.height - textSize.height;

	wchar_t* b, * e;
	size_t i = 0;
	int textW;
	while (true) {
		b = text + i;
		if (multiLine) e = wmemchr(b, L'\n', len - i);
		else e = text + len;
		size_t lineLen = e ? size_t(e - b) : len - i;

		if (lineLen > 0) {
			SlotCacher::load_box16(b, lineLen, faceHandle);
			auto& box = faceHandle.box;
			textW = box.get_width();

			penLeft = rc.left;
			if (art.alignX == Align::CENTER)
				penLeft += (rc.width - textW) / 2;
			else if (art.alignX == Align::RIGHT)
				penLeft += rc.width - textW;

			_draw_glyphs(penLeft, penTop, &rc);
		}

		penTop += absLineHeight;
		if (!e || !multiLine) break;
		i = i + e - b + 1;
	}

	return textSize.width;
}

int Canvas::draw_text8(std::string& text, Rect& rc, bool multiLine) {
	return draw_text8((char*)text.c_str(), rc, multiLine);
}

int Canvas::draw_text16(std::wstring& text, Rect& rc, bool multiLine) {
	return draw_text16((wchar_t*)text.c_str(), rc, multiLine);
}

int Canvas::draw_text8(char* text, Rect& rc, std::vector<ColoredSpan>* spans, bool multiLine) {
	if (!spans || spans->size() == 0) return draw_text8(text, rc, multiLine);
	Size textSize;
	size_t len = strlen(text);
	if (!len || !text) return 0;
	get_size8(text, len, &textSize, multiLine);
	int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight),
		penLeft = rc.left, penTop = rc.top + maxH;

	if (art.alignY == Align::CENTER)
		penTop += (rc.height - textSize.height) / 2;
	else if (art.alignY == Align::BOTTOM)
		penTop += rc.height - textSize.height;

	char* b, * e;
	size_t i = 0, ispan = 0;
	int textW;
	Color cr = art.textColor;
	auto* span = &spans->at(0);

	while (true) {
		b = text + i;
		if (multiLine) e = (char*)memchr(b, '\n', len - i);
		else e = text + len;
		size_t lineLen = e ? size_t(e - b) : len - i;
		if (lineLen < 1) {
			penTop += absLineHeight;
			if (!e || !multiLine) break;
			++i;
			continue;
		}

		SlotCacher::load_box8(b, lineLen, faceHandle);
		auto& box = faceHandle.box;
		textW = box.get_width();

		penLeft = rc.left;
		if (art.alignX == Align::CENTER)
			penLeft += (rc.width - textW) / 2;
		else if (art.alignX == Align::RIGHT)
			penLeft += rc.width - textW;

		int l = penLeft, w;
		size_t currlen;
		while (lineLen > 0)
		{
			auto spanLen = lineLen;
			bool lastSpan = (ispan >= spans->size() - 1);
			bool forwSpan = false;
			currlen = lineLen;

			if (span) {
				if (span->length < 0) {
					if (!lastSpan) {
						spanLen = span[1].start - span->start;
					}
					else {
						spanLen = lineLen;
					}
				}
				else {
					spanLen = span->length;
				}
			}

			if (!span) {
				currlen = lineLen;
				art.textColor.set(cr);
			}
			else if (i < span->start) {
				currlen = span->start - i;
				art.textColor.set(cr);
			}
			else if (lastSpan && (i >= (span->start + span->length))) {
				currlen = lineLen;
				if (span->length < 0)
					art.textColor.set(span->color);
				else
					art.textColor.set(cr);
				span = !e ? NULL : span;
			}
			else if (span->start <= i) {
				if ((span->start + spanLen) <= (i + lineLen)) {
					currlen = spanLen - (i - span->start);
					forwSpan = !lastSpan;
				}
				else {
					currlen = lineLen;
				}
				art.textColor.set(span->color);
			}
			else {
				currlen = lineLen;
			}

			currlen = Min(currlen, lineLen);
			SlotCacher::load_box8(b, currlen, faceHandle);
			auto& box = faceHandle.box;
			w = box.get_width();
			_draw_glyphs(l, penTop, &rc);
			l += w;
			b += currlen;
			i += currlen;
			lineLen -= currlen;
			if (!span) break;
			if (forwSpan) {
				if (lastSpan) {
					span = NULL;
				}
				else {
					span = &spans->at(++ispan);
				}
			}
		}

		penTop += absLineHeight;
		if (!e || !multiLine) break;
		++i;
	}

	art.textColor.set(cr);
	return textSize.width;
}

int Canvas::draw_text16(wchar_t* text, Rect& rc, std::vector<ColoredSpan>* spans, bool multiLine) {
	if (!spans || spans->size() == 0) return draw_text16(text, rc, multiLine);
	Size textSize;
	size_t len = wcslen(text);
	if (!len || !text) return 0;
	get_size16(text, len, &textSize, multiLine);
	int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight),
		penLeft = rc.left, penTop = rc.top + maxH;

	if (art.alignY == Align::CENTER)
		penTop += (rc.height - textSize.height) / 2;
	else if (art.alignY == Align::BOTTOM)
		penTop += rc.height - textSize.height;

	wchar_t* b, * e;
	size_t i = 0, ispan = 0;
	int textW;
	Color cr = art.textColor;
	auto* span = &spans->at(0);

	while (true) {
		b = text + i;
		if (multiLine) e = wmemchr(b, L'\n', len - i);
		else e = text + len;
		size_t lineLen = e ? size_t(e - b) : len - i;
		if (lineLen < 1) {
			penTop += absLineHeight;
			if (!e || !multiLine) break;
			++i;
			continue;
		}

		SlotCacher::load_box16(b, lineLen, faceHandle);
		auto& box = faceHandle.box;
		textW = box.get_width();

		penLeft = rc.left;
		if (art.alignX == Align::CENTER)
			penLeft += (rc.width - textW) / 2;
		else if (art.alignX == Align::RIGHT)
			penLeft += rc.width - textW;

		int l = penLeft, w;
		size_t currlen;
		while (lineLen > 0)
		{
			auto spanLen = lineLen;
			bool lastSpan = (ispan >= spans->size() - 1);
			bool forwSpan = false;
			currlen = lineLen;

			if (span) {
				if (span->length < 0) {
					if (!lastSpan) {
						spanLen = span[1].start - span->start;
					}
					else {
						spanLen = lineLen;
					}
				}
				else {
					spanLen = span->length;
				}
			}

			if (!span) {
				currlen = lineLen;
				art.textColor.set(cr);
			}
			else if (i < span->start) {
				currlen = span->start - i;
				art.textColor.set(cr);
			}
			else if (lastSpan && (i >= (span->start + span->length))) {
				currlen = lineLen;
				if (span->length < 0)
					art.textColor.set(span->color);
				else
					art.textColor.set(cr);
				span = !e ? NULL : span;
			}
			else if (span->start <= i) {
				if ((span->start + spanLen) <= (i + lineLen)) {
					currlen = spanLen - (i - span->start);
					forwSpan = !lastSpan;
				}
				else {
					currlen = lineLen;
				}
				art.textColor.set(span->color);
			}
			else {
				currlen = lineLen;
			}

			currlen = Min(currlen, lineLen);
			SlotCacher::load_box16(b, currlen, faceHandle);
			auto& box = faceHandle.box;
			w = box.get_width();
			_draw_glyphs(l, penTop, &rc);
			l += w;
			b += currlen;
			i += currlen;
			lineLen -= currlen;
			if (!span) break;
			if (forwSpan) {
				if (lastSpan) {
					span = NULL;
				}
				else {
					span = &spans->at(++ispan);
				}
			}
		}

		penTop += absLineHeight;
		if (!e || !multiLine) break;
		++i;
	}

	art.textColor.set(cr);
	return textSize.width;
}

int Canvas::draw_text8(char* text, size_t len, int penLeft, int penTop, bool multiLine) {
	Size textSize;
	if (IS_NULL(text) || INT_IS_ZERO(len)) return 0;
	get_size8(text, len, &textSize, multiLine);
	int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight);
	penTop += maxH;

	char* b, * e;
	size_t i = 0;
	while (true) {
		b = text + i;
		if (multiLine) e = (char*)memchr(b, '\n', len - i);
		else e = text + len;
		size_t lineLen = e ? size_t(e - b) : len - i;
		if (lineLen > 0) {
			SlotCacher::load_box8(b, lineLen, faceHandle);
			_draw_glyphs(penLeft, penTop, 0);
		}
		penTop += absLineHeight;
		if (!e || !multiLine) break;
		i = i + e - b + 1;
	}

	return textSize.width;
}

int Canvas::draw_text16(wchar_t* text, size_t len, int penLeft, int penTop, bool multiLine) {
	Size textSize;
	if (!text || !len) return 0;
	get_size16(text, len, &textSize, multiLine);
	int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight);
	penTop += maxH;

	wchar_t* b, * e;
	size_t i = 0;
	while (true) {
		b = text + i;
		if (multiLine) e = wmemchr(b, L'\n', len - i);
		else e = text + len;
		size_t lineLen = e ? size_t(e - b) : len - i;
		if (lineLen > 0) {
			SlotCacher::load_box16(b, lineLen, faceHandle);
			_draw_glyphs(penLeft, penTop, 0);
		}
		penTop += absLineHeight;
		if (!e || !multiLine) break;
		i = i + e - b + 1;
	}

	return textSize.width;
}

int Canvas::draw_label8(char* text, size_t len, Rect& rect, BorderRadius& borderRadius, Spacing& padd, Border& border, Color& fill) {
	Size textSize;
	get_size8(text, len, &textSize, true);
	if (!text || !len) return 0;
	int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight);
	int xdiff = (rect.width - textSize.width) / 2;
	int ydiff = (rect.height - textSize.height) / 2;

	Rect rc(rect);
	rc.shift(xdiff, ydiff);
	rc.width -= 2 * xdiff;
	rc.height -= 2 * ydiff;
	Rect rcx(rc);

	rc.expand(padd);
	rc.expand(border);
	ShapeStorage roundedBox;
	roundedBox.rounded_rect(rc.left, rc.top, rc.width, rc.height, borderRadius);
	roundedBox.normalize_radius();
	reset();
	ras.add_path(roundedBox);
	render(ras, sl, fill);
	if (border.left) {
		agg::conv_stroke<ShapeStorage> stroke(roundedBox);
		stroke.width(border.left);
		reset();
		add_path(stroke);
		render(border.leftColor);
		reset();
	}

	int penLeft, penTop;
	penTop = rcx.top + maxH;
	size_t i = 0;
	char* b, * e;
	while (true) {
		b = text + i;
		e = (char*)memchr(b, '\n', len - i);
		size_t lineLen = e ? size_t(e - b - 1) : len - i;

		if (lineLen > 0) {
			SlotCacher::load_box8(b, lineLen, faceHandle);
			auto& box = faceHandle.box;
			int textW = box.get_width();

			penLeft = rc.left;
			if (art.alignX == Align::CENTER)
				penLeft += (rc.width - textW) / 2;
			else if (art.alignX == Align::RIGHT)
				penLeft += rc.width - textW;

			_draw_glyphs(penLeft, penTop, &rc);
		}

		penTop += absLineHeight;
		if (!e) break;
		i = i + e - b + 1;
	}

	return textSize.width;
}

int Canvas::draw_label16(wchar_t* text, size_t len, Rect& rect, BorderRadius& borderRadius, Spacing& padd, Border& border, Color& fill) {
	Size textSize;
	get_size16(text, len, &textSize, true);
	if (!text || !len) return 0;
	int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight);
	int xdiff = (rect.width - textSize.width) / 2;
	int ydiff = (rect.height - textSize.height) / 2;

	Rect rc(rect);
	rc.shift(xdiff, ydiff);
	rc.width -= 2 * xdiff;
	rc.height -= 2 * ydiff;
	Rect rcx(rc);

	rc.expand(padd);
	rc.expand(border);
	ShapeStorage roundedBox;
	roundedBox.rounded_rect(rc.left, rc.top, rc.width, rc.height, borderRadius);
	roundedBox.normalize_radius();
	reset();
	ras.add_path(roundedBox);
	render(ras, sl, fill);
	if (border.left) {
		agg::conv_stroke<ShapeStorage> stroke(roundedBox);
		stroke.width(border.left);
		reset();
		add_path(stroke);
		render(border.leftColor);
		reset();
	}

	int penLeft, penTop;
	penTop = rcx.top + maxH;
	size_t i = 0;
	wchar_t* b, * e;
	while (true) {
		b = text + i;
		e = wmemchr(b, L'\n', len - i);
		size_t lineLen = e ? size_t(e - b - 1) : len - i;

		if (lineLen > 0) {
			SlotCacher::load_box16(b, lineLen, faceHandle);
			auto& box = faceHandle.box;
			int textW = box.get_width();

			penLeft = rc.left;
			if (art.alignX == Align::CENTER)
				penLeft += (rc.width - textW) / 2;
			else if (art.alignX == Align::RIGHT)
				penLeft += rc.width - textW;

			_draw_glyphs(penLeft, penTop, &rc);
		}

		penTop += absLineHeight;
		if (!e) break;
		i = i + e - b + 1;
	}

	return textSize.width;
}

void Canvas::render_svg(agg::svg::path_renderer* path, int left, int top, int width, int height, byte opacity, Spacing* extraPadding, agg::trans_affine* overlayMatrix)
{
	if (IS_NULL(path)) return;
	pixfmt_bgr pf(sheet, area);
	renderer_base rb(pf);
	agg::renderer_scanline_aa_solid<renderer_base> ren(rb);
	agg::rasterizer_scanline_aa<> ras;
	agg::scanline_p8 sl;
	agg::trans_affine mtx;

	double ds = 0;
	if (extraPadding) {
		ds = fmin(float(width - 1.0 - path->pl - path->pr - extraPadding->left - extraPadding->right) / fabs(path->width),
			float(height - 1.0 - path->pt - path->pb - extraPadding->top - extraPadding->bottom) / fabs(path->height));
	}
	else {
		ds = fmin(double(width - 1.0 - path->pl - path->pr) / fabs(path->width),
			double(height - 1.0 - path->pt - path->pb) / fabs(path->height));
	}

	if (path->width <= 0 || path->height <= 0)
		ds = 0;

	mtx *= agg::trans_affine_translation(-path->left - path->width / 2, -path->top - path->height / 2);
	mtx *= agg::trans_affine_scaling(ds);
	if (overlayMatrix) mtx *= *overlayMatrix;
	mtx *= agg::trans_affine_translation(path->width / 2 * ds, path->height / 2 * ds);
	if (extraPadding)
		mtx *= agg::trans_affine_translation((double)left + path->pl + extraPadding->left, (double)top + path->pt + extraPadding->top);
	else
		mtx *= agg::trans_affine_translation((double)left + path->pl, (double)top + path->pt);

	path->render(ras, sl, ren, mtx, rb.clip_box(), opacity);
}

void Canvas::render_svg_origin(agg::svg::path_renderer* path, int centerX, int centerY, double scale, byte opacity, agg::trans_affine* overlayMatrix)
{
	if (IS_NULL(path)) return;
	pixfmt_bgr pf(sheet, area);
	renderer_base rb(pf);
	agg::renderer_scanline_aa_solid<renderer_base> ren(rb);
	agg::rasterizer_scanline_aa<> ras;
	agg::scanline_p8 sl;
	agg::trans_affine mtx;

	mtx *= agg::trans_affine_translation(-path->left - path->width / 2, -path->top - path->height / 2);
	mtx *= agg::trans_affine_scaling(scale);
	if (overlayMatrix) mtx *= *overlayMatrix;
	mtx *= agg::trans_affine_translation(centerX, centerY);

	path->render(ras, sl, ren, mtx, rb.clip_box(), opacity);
}

void Canvas::clear(Rect& rect, Border& bor, Color& color, byte opacity) {
	if (IS_NULL(sheet) || IS_NULL(sheet->data) || INT_IS_ZERO(sheet->w) || INT_IS_ZERO(sheet->h)) return;
	int w, h, px, eex, sx, sy, ex, ey, dx, dy, x, y, cx, cy, count;
	byte cla, clp, a, p, borLA, borLP, borTA, borTP, borRA, borRP, borBA, borBP;

	w = sheet->w;
	h = sheet->h;
	x = rect.left;
	y = rect.top;
	cx = rect.width;
	cy = rect.height;
	if (area) {
		x += area->left;
		y += area->top;
	}

	ex = cx + x;
	ey = cy + y;
	sx = x;
	sy = y;

	byte* d;
	int begx = 3 * sx, remx = (sheet->pitch - 3 * ex), lenx = 3 * (ex - sx), remp = begx + remx;

	if (BYTE_OPAQUE(color.a) && BYTE_OPAQUE(opacity)) {
		px = color.to_int();
		eex = ex - 1;
		for (y = sy; y < ey; ++y) {
			if (y < 0) continue;
			if (y >= sheet->h) break;
			sheet->clip(sx, y, &count, &cla);
			d = sheet->data + y * sheet->pitch + begx;
			clp = 0xff - cla;
			x = sx;
			if (INT_IS_ZERO(cla)) {
				x += count, d += 3 * count;
				count = 0;
			}
			for (; x < ex; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= sheet->w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					if (INT_IS_ZERO(cla)) {
						int rem = Max(0, count - 1);
						x += rem, d += rem * 3, count = 1;
						continue;
					}
					clp = 0xff - cla;
				}

				if (BYTE_OPAQUE(cla)) {
					if (x < eex) {
						*(int*)d = px;
					}
					else {
						byte c = d[3];
						*(int*)d = px;
						d[3] = c;
					}
				}
				else {
					d[0] = CLAMP255(DIV255(color.b * cla + (d[0] * clp)));
					d[1] = CLAMP255(DIV255(color.g * cla + (d[1] * clp)));
					d[2] = CLAMP255(DIV255(color.r * cla + (d[2] * clp)));
				}
			}
		}
	}
	else {
		byte backA = DIV255(color.a * opacity);
		for (y = sy; y < ey; ++y) {
			if (y < 0) continue;
			if (y >= sheet->h) break;
			sheet->clip(sx, y, &count, &cla);
			d = sheet->data + y * sheet->pitch + begx;
			x = sx;
			if (INT_IS_ZERO(cla)) {
				x += count, d += 3 * count;
				count = 0;
			}
			for (; x < ex; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= sheet->w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					if (INT_IS_ZERO(cla)) {
						int rem = Max(0, count - 1);
						x += rem, d += rem * 3, count = 1;
						continue;
					}
				}

				a = DIV255(backA * cla);
				p = 0xff - a;

				d[0] = CLAMP255(DIV255(color.b * a + (d[0] * p)));
				d[1] = CLAMP255(DIV255(color.g * a + (d[1] * p)));
				d[2] = CLAMP255(DIV255(color.r * a + (d[2] * p)));
			}
		}
	}

	borTA = DIV255(bor.topColor.a * opacity);
	if (borTA) {
		borTP = 0xff - borTA;
		dy = CLAMP2(sy + bor.top, h - 1);
		eex = ex - 1;
		px = bor.topColor.to_int();
		for (y = sy; y < dy; y++) {
			if (y < 0) continue;
			if (y >= h) break;
			sheet->clip(sx, y, &count, &cla);
			d = sheet->data + y * sheet->pitch + 3 * sx;
			x = sx;
			if (INT_IS_ZERO(cla)) {
				x += count, d += 3 * count;
				count = 0;
			}

			for (; x < ex; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					if (!cla) continue;
				}

				a = DIV255(cla * borTA);
				if (INT_IS_ZERO(a)) {
					continue;
				}
				else if (BYTE_OPAQUE(a)) {
					if (x < eex) {
						*(int*)d = px;
					}
					else {
						byte c = d[3];
						*(int*)d = px;
						d[3] = c;
					}
				}
				else {
					p = 0xff - a;
					d[0] = CLAMP255(DIV255(bor.topColor.b * a + (d[0] * p)));
					d[1] = CLAMP255(DIV255(bor.topColor.g * a + (d[1] * p)));
					d[2] = CLAMP255(DIV255(bor.topColor.r * a + (d[2] * p)));
				}
			}
		}
	}

	borBA = DIV255(bor.botColor.a * opacity);
	if (borBA) {
		borBP = 0xff - borBA;
		eex = ex - 1;
		px = bor.botColor.to_int();
		for (y = ey - bor.bottom; y < ey; y++) {
			if (y < 0) continue;
			if (y >= h) break;
			d = sheet->data + y * sheet->pitch + 3 * sx;
			sheet->clip(sx, y, &count, &cla);
			x = sx;
			if (INT_IS_ZERO(cla)) {
				x += count, d += 3 * count;
				count = 0;
			}

			for (; x < ex; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					if (!cla) continue;
				}

				a = DIV255(cla * borBA);
				if (INT_IS_ZERO(a)) {
					continue;
				}
				else if (BYTE_OPAQUE(a)) {
					if (x < eex) {
						*(int*)d = px;
					}
					else {
						byte c = d[3];
						*(int*)d = px;
						d[3] = c;
					}
				}
				else {
					p = 0xff - a;
					d[0] = CLAMP255(DIV255(bor.botColor.b * a + (d[0] * p)));
					d[1] = CLAMP255(DIV255(bor.botColor.g * a + (d[1] * p)));
					d[2] = CLAMP255(DIV255(bor.botColor.r * a + (d[2] * p)));
				}
			}
		}
	}

	ey -= bor.bottom;
	dx = Min(Max(sx + bor.left, 0), w - 1);

	borLA = DIV255(bor.leftColor.a * opacity);
	if (borLA) {
		borLP = 0xff - borLA;
		eex = dx - 1;
		px = bor.leftColor.to_int();
		for (y = sy + bor.top; y < ey; y++) {
			if (y < 0) continue;
			if (y >= h) break;
			d = sheet->data + y * sheet->pitch + 3 * sx;
			sheet->clip(sx, y, &count, &cla);
			x = sx;
			if (INT_IS_ZERO(cla)) {
				x += count, d += 3 * count;
				count = 0;
			}

			for (; x < dx; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					if (!cla) continue;
				}

				a = DIV255(cla * borLA);
				if (INT_IS_ZERO(a)) {
					continue;
				}
				else if (BYTE_OPAQUE(a)) {
					if (x < eex) {
						*(int*)d = px;
					}
					else {
						byte c = d[3];
						*(int*)d = px;
						d[3] = c;
					}
				}
				else {
					p = 0xff - a;
					d[0] = CLAMP255(DIV255(bor.leftColor.b * a + (d[0] * p)));
					d[1] = CLAMP255(DIV255(bor.leftColor.g * a + (d[1] * p)));
					d[2] = CLAMP255(DIV255(bor.leftColor.r * a + (d[2] * p)));
				}
			}
		}
	}

	borRA = DIV255(bor.rightColor.a * opacity);
	if (borRA) {
		borRP = 0xff - borRA;
		int _sx = Min(Max(ex - bor.right, 0), w);
		dx = ex;
		eex = dx - 1;
		px = bor.rightColor.to_int();

		for (y = sy + bor.top; y < ey; y++) {
			if (y < 0) continue;
			if (y >= h) break;
			d = sheet->data + y * sheet->pitch + 3 * _sx;
			sheet->clip(sx, y, &count, &cla);
			x = _sx;
			if (INT_IS_ZERO(cla)) {
				x += count, d += 3 * count;
				count = 0;
			}

			for (; x < dx; x++, d += 3, --count) {
				if (x < 0) continue;
				if (x >= w) break;
				if (count < 1) {
					sheet->clip(x, &count, &cla);
					if (!cla) continue;
				}

				a = DIV255(cla * borRA);
				if (INT_IS_ZERO(a)) {
					continue;
				}
				else if (BYTE_OPAQUE(a)) {
					if (x < eex) {
						*(int*)d = px;
					}
					else {
						byte c = d[3];
						*(int*)d = px;
						d[3] = c;
					}
				}
				else {
					p = 0xff - a;
					d[0] = CLAMP255(DIV255(bor.rightColor.b * a + (d[0] * p)));
					d[1] = CLAMP255(DIV255(bor.rightColor.g * a + (d[1] * p)));
					d[2] = CLAMP255(DIV255(bor.rightColor.r * a + (d[2] * p)));
				}
			}
		}
	}
}

void Canvas::clear_opaque(Rect* rect, Color& color) {
	if (IS_NULL(sheet) || IS_NULL(sheet->data) || INT_IS_ZERO(sheet->w) || INT_IS_ZERO(sheet->h)) return;
	int x, y, sx, sy, ex, ey, px, cx, cy;

	if (rect) {
		x = rect->left;
		y = rect->top;
		cx = rect->width;
		cy = rect->height;
	}
	else {
		x = 0;
		y = 0;
		if (area) {
			cx = area->width;
			cy = area->height;
		}
		else {
			cx = sheet->w;
			cy = sheet->h;
		}
	}
	if (area) {
		x += area->left;
		y += area->top;
	}

	ex = CLAMP2(cx + x, sheet->w);
	ey = CLAMP2(cy + y, sheet->h);
	sx = CLAMP3(0, x, sheet->w - 1);
	sy = CLAMP3(0, y, sheet->h - 1);

	if (ex - sx < 1 || ey - sy < 1) return;

	byte* __restrict dst, * __restrict end;
	int begx = 3 * sx, remx = (sheet->pitch - 3 * ex), lenx = 3 * (ex - sx - 1), remp = begx + remx;
	px = color.to_int();
	for (y = sy, dst = sheet->data + sy * sheet->pitch + begx; y < ey; ++y) {
		for (end = dst + lenx; dst < end; dst += 3) {
			*(int*)dst = px;
		}
		byte c = dst[3];
		*(int*)dst = px;
		dst[3] = c;
		dst += 3 + remp;
	}
}

void Canvas::bit_blt(Sheet& srcSheet, int dstx, int dsty, int width, int height, int srcx, int srcy, bool useStencil) {
	if (IS_NULL(sheet) || IS_NULL(sheet->data) || INT_IS_ZERO(sheet->w) || INT_IS_ZERO(sheet->h)) return;
	int x, y, sx, sy, ex, ey, cx, cy;

	if (area) {
		dstx += area->left;
		dsty += area->top;
	}

	if (dstx < 0) {
		width += dstx;
		dstx = 0;
	}
	if (dsty < 0) {
		height += dsty;
		dsty = 0;
	}

	width = Min(dstx + width, dstx + -srcx + srcSheet.w) - dstx + Min(0, srcx);
	height = Min(dsty + height, dsty + -srcy + srcSheet.h) - dsty + Min(0, srcy);

	if (srcx < 0) {
		dstx += -srcx;
		srcx = 0;
	}
	else srcx += -Min(0, dstx);

	if (srcy < 0) {
		dsty += -srcy;
		srcy = 0;
	}
	else srcy += -Min(0, dsty);

	x = dstx;
	y = dsty;
	cx = width;
	cy = height;

	ex = CLAMP2(cx + x, sheet->w);
	ey = CLAMP2(cy + y, sheet->h);
	sx = CLAMP3(0, x, sheet->w - 1);
	sy = CLAMP3(0, y, sheet->h - 1);

	if (ex - sx < 1 || ey - sy < 1 || srcx >= srcSheet.w || srcy >= srcSheet.h || srcx + srcSheet.w < 1 || srcy + srcSheet.h < 1)
		return;

	int dx, dy, count;
	byte cla, clp, a, p, * d, * s;
	byte* __restrict src, * __restrict dst;

	int
		dlenx = sheet->nbpp * (ex - sx),
		slenx = srcSheet.nbpp * (ex - sx),

		dbegx = sheet->nbpp * sx,
		dremx = (sheet->pitch - sheet->nbpp * ex),
		dremp = dbegx + dremx,

		sbegx = srcSheet.nbpp * srcx,
		sremx = (srcSheet.pitch - srcSheet.nbpp * (ex - sx + srcx)),
		sremp = sbegx + sremx;

	src = srcSheet.data + srcy * srcSheet.pitch + sbegx;
	dst = sheet->data + sy * sheet->pitch + dbegx;

	if (sheet->nbpp == OUI_BGR && srcSheet.nbpp == OUI_BGRA) {
		for (y = sy, dy = sy + dsty; y < ey; ++y, ++dy) {
			dx = sx + dstx;
			sheet->clip(dx, dy, &count, &cla);
			if (INT_IS_ZERO(cla)) {
				src += slenx + sremp;
				dst += dlenx + dremp;
				continue;
			}
			clp = 0xff - cla;
			d = dst;
			s = src;

			for (x = sx; x < ex; ++x, ++dx, d += 3, s += 4, --count) {
				if (count < 1) {
					sheet->clip(dx, &count, &cla);
					if (INT_IS_ZERO(cla)) {
						int rem = Max(count - 1, 0);
						x += rem;
						dx += rem;
						s += 4 * rem;
						d += 3 * rem;
						count = 1;
						continue;
					}
					clp = 0xff - cla;
				}

				a = DIV255(s[3] * cla);
				if (INT_IS_ZERO(a)) continue;
				if (BYTE_OPAQUE(a)) {
					d[0] = s[0];
					d[1] = s[1];
					d[2] = s[2];
				}
				else {
					p = 0xff - a;
					d[0] = CLAMP255(DIV255(s[0] * a + (d[0] * p)));
					d[1] = CLAMP255(DIV255(s[1] * a + (d[1] * p)));
					d[2] = CLAMP255(DIV255(s[2] * a + (d[2] * p)));
				}
			}

			src += slenx + sremp;
			dst += dlenx + dremp;
		}
	}
	else if (sheet->nbpp == OUI_BGR && srcSheet.nbpp == OUI_BGR) {
		if (useStencil && IS_NOT_NULL(sheet->curr)) {
			int dy, dx, count;
			byte cla, clp, * d, * s;

			for (y = sy, dy = sy; y < ey; ++y, ++dy) {
				dx = sx;
				sheet->clip(dx, dy, &count, &cla);
				if (INT_IS_ZERO(cla)) {
					src += slenx + sremp;
					dst += dlenx + dremp;
					continue;
				}
				clp = 0xff - cla;
				d = dst;
				s = src;

				for (x = sx; x < ex; ++x, ++dx, d += 3, s += 3, --count) {
					if (count < 1) {
						sheet->clip(dx, &count, &cla);
						if (INT_IS_ZERO(cla)) {
							int rem = Max(count - 1, 0);
							x += rem;
							dx += rem;
							s += 3 * rem;
							d += 3 * rem;
							count = 1;
							continue;
						}
						clp = 0xff - cla;
					}

					if (BYTE_OPAQUE(cla)) {
						*(short*)d = *(short*)s;
						d[2] = s[2];
					}
					else {
						d[0] = CLAMP255(DIV255(s[0] * cla + (d[0] * clp)));
						d[1] = CLAMP255(DIV255(s[1] * cla + (d[1] * clp)));
						d[2] = CLAMP255(DIV255(s[2] * cla + (d[2] * clp)));
					}
				}

				src += slenx + sremp;
				dst += dlenx + dremp;
			}
		}
		else {
			for (y = sy; y < ey; ++y) {
				memcpy(dst, src, dlenx);
				src += slenx + sremp;
				dst += dlenx + dremp;
			}
		}
	}
}

int Canvas::draw_tag8(char* text, size_t len, int x, int y, int dir, bool invert, int arrowW, int arrowH, BorderRadius& borderRadius, Spacing& padd, Color& fill) {
	Size textSize;
	get_size8(text, len, &textSize, true);
	if (!text || !len) return 0;
	int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight);
	int w = textSize.width + padd.left + padd.right;
	int h = textSize.height + padd.top + padd.bottom;
	double w2 = w / 2.0;
	double h2 = h / 2.0;
	Rect rc;
	dir %= 8;
	if (invert) {
		if (dir == 0) rc.set(int(x - w2), y, w, h);
		else if (dir == 1) rc.set(int(x - w), y, w, h);
		else if (dir == 2) rc.set(int(x - w), int(y - h2), w, h);
		else if (dir == 3) rc.set(int(x - w), int(y - h), w, h);
		else if (dir == 4) rc.set(int(x - w2), int(y - h), w, h);
		else if (dir == 5) rc.set(int(x), int(y - h), w, h);
		else if (dir == 6) rc.set(int(x), int(y - h2), w, h);
		else if (dir == 7) rc.set(int(x), y, w, h);
	}
	else {
		if (dir == 0) rc.set(int(x - w2), int(y - arrowH - h), w, h);
		else if (dir == 1) rc.set(int(x + arrowW), int(y - arrowH - h), w, h);
		else if (dir == 2) rc.set(int(x + arrowW), int(y - h2), w, h);
		else if (dir == 3) rc.set(int(x + arrowW), int(+ arrowH), w, h);
		else if (dir == 4) rc.set(int(x - w2), int(y + arrowH), w, h);
		else if (dir == 5) rc.set(int(x - arrowW - w), int(y + arrowH), w, h);
		else if (dir == 6) rc.set(int(x - arrowW - w), int(y - h2), w, h);
		else if (dir == 7) rc.set(int(x - arrowW - w), int(y - arrowH - h), w, h);
	}
	
	ShapeStorage roundedBox;
	roundedBox.rounded_rect(rc.left, rc.top, rc.width, rc.height, borderRadius);
	roundedBox.normalize_radius();
	reset();
	agg::path_storage arrow;
	int mnW = Min(w, arrowW);
	int mnH = Min(h, arrowH);
	double sn = fabs(sin(PI / 4));
	double cs = fabs(cos(PI / 4));

	if (invert) {
		if (dir == 0) {
			arrow.move_to(x, y + (arrowH + h));
			arrow.line_to(x + arrowW / 2.0, (double)y + h);
			arrow.line_to(x - arrowW / 2.0, (double)y + h);
		}
		else if (dir == 1) {
			arrow.move_to(x - arrowW - w, y + arrowH + h);
			arrow.line_to((double)x - w, (double)y + h - Max(borderRadius.lb, mnH * sn));
			arrow.line_to((double)x - w + Max(borderRadius.lb, mnW * cs), (double)y + h );
		}
		else if (dir == 2) {
			arrow.move_to(x - arrowW - w, y);
			arrow.line_to((double)x - w, (double)y - arrowH / 2);
			arrow.line_to((double)x - w, (double)y + arrowH / 2);
		}
		else if (dir == 3) {
			arrow.move_to(x - w - arrowW, y - h - arrowH);
			arrow.line_to((double)x - w + Max(borderRadius.lt, mnW * cs), (double)y - h);
			arrow.line_to((double)x - w, (double)y - h + Max(borderRadius.lt, mnH * sn));
		}
		else if (dir == 4) {
			arrow.move_to(x, y - h - arrowH);
			arrow.line_to((double)x + arrowW / 2, (double)y - h);
			arrow.line_to((double)x - arrowW / 2, (double)y - h);
		}
		else if (dir == 5) {
			arrow.move_to(x + w + arrowW, y - h - arrowH);
			arrow.line_to(x + w, y - h + Max(borderRadius.rt, mnH * sn));
			arrow.line_to(x + w - Max(borderRadius.rt, mnW * cs), y - h);
		}
		else if (dir == 6) {
			arrow.move_to(x + arrowW + w, y);
			arrow.line_to(x + w, y - arrowH / 2.0);
			arrow.line_to(x + w, y + arrowH / 2.0);
		}
		else if (dir == 7) {
			arrow.move_to(x + w + arrowW, y + h + arrowH);
			arrow.line_to(x + w - Max(borderRadius.rb, mnW * cs), y + h);
			arrow.line_to(x + w, y + h - Max(borderRadius.rb, mnH * sn));
		}
	} 
	else {
		arrow.move_to(x, y);
		if (dir == 0) {
			arrow.line_to(x + arrowW / 2.0, (double)y - arrowH);
			arrow.line_to(x - arrowW / 2.0, (double)y - arrowH);
		}
		else if (dir == 1) {
			arrow.line_to((double)x + arrowW, (double)y - arrowH - Max(borderRadius.lb, mnH * sn));
			arrow.line_to((double)x + arrowW + Max(borderRadius.lb, mnW * cs), (double)y - arrowH);
		}
		else if (dir == 2) {
			arrow.line_to((double)x + arrowW, (double)y - arrowH / 2);
			arrow.line_to((double)x + arrowW, (double)y + arrowH / 2);
		}
		else if (dir == 3) {
			arrow.line_to((double)x + arrowW + Max(borderRadius.lt, mnW * cs), (double)y + arrowH);
			arrow.line_to((double)x + arrowW, (double)y + arrowH + Max(borderRadius.lt, mnH * sn));
		}
		else if (dir == 4) {
			arrow.line_to((double)x + arrowW / 2, (double)y + arrowH);
			arrow.line_to((double)x - arrowW / 2, (double)y + arrowH);
		}
		else if (dir == 5) {
			arrow.line_to(x - arrowW, y + arrowH + Max(borderRadius.rt, mnH * sn));
			arrow.line_to(x - arrowW - Max(borderRadius.rt, mnW * cs), y + arrowH);
		}
		else if (dir == 6) {
			arrow.line_to(x - arrowW, y - arrowH / 2.0);
			arrow.line_to(x - arrowW, y + arrowH / 2.0);
		}
		else if (dir == 7) {
			arrow.line_to(x - arrowW - Max(borderRadius.rb, mnW * cs), y - arrowH);
			arrow.line_to(x - arrowW, y - arrowH - Max(borderRadius.rb, mnH * sn));
		}
	}
	

	arrow.close_polygon();
	add_path(roundedBox);
	add_path(arrow);
	render(fill);

	Rect rcx(rc);
	rcx.shrink(padd);
	int penLeft, penTop;
	penTop = rcx.top + maxH;
	size_t i = 0;
	char* b, * e;
	while (true) {
		b = text + i;
		e = (char*)memchr(b, '\n', len - i);
		size_t lineLen = e ? size_t(e - b - 1) : len - i;

		if (lineLen > 0) {
			SlotCacher::load_box8(b, lineLen, faceHandle);
			auto& box = faceHandle.box;
			int textW = box.get_width();

			penLeft = rcx.left;
			if (art.alignX == Align::CENTER)
				penLeft += (int)((double)(rcx.width - textW) / 2.0);
			else if (art.alignX == Align::RIGHT)
				penLeft += rcx.width - textW;

			_draw_glyphs(penLeft, penTop, &rc);
		}

		penTop += absLineHeight;
		if (!e) break;
		i = i + e - b + 1;
	}

	return textSize.width;
}

int Canvas::draw_tag16(wchar_t* text, size_t len, int x, int y, int dir, bool invert, int arrowW, int arrowH, BorderRadius& borderRadius, Spacing& padd, Color& fill) {
	Size textSize;
	get_size16(text, len, &textSize, true);
	if (!text || !len) return 0;
	int maxH = (int)get_max_height(), absLineHeight = int(maxH * this->lineHeight);
	int w = textSize.width + padd.left + padd.right;
	int h = textSize.height + padd.top + padd.bottom;
	double w2 = w / 2.0;
	double h2 = h / 2.0;
	Rect rc;
	dir %= 8;
	if (invert) {
		if (dir == 0) rc.set(int(x - w2), y, w, h);
		else if (dir == 1) rc.set(int(x - w), y, w, h);
		else if (dir == 2) rc.set(int(x - w), int(y - h2), w, h);
		else if (dir == 3) rc.set(int(x - w), int(y - h), w, h);
		else if (dir == 4) rc.set(int(x - w2), int(y - h), w, h);
		else if (dir == 5) rc.set(x, int(y - h), w, h);
		else if (dir == 6) rc.set(x, int(y - h2), w, h);
		else if (dir == 7) rc.set(x, y, w, h);
	}
	else {
		if (dir == 0) rc.set(int(x - w2), int(y - arrowH - h), w, h);
		else if (dir == 1) rc.set(int(x + arrowW), int(y - arrowH - h), w, h);
		else if (dir == 2) rc.set(int(x + arrowW), int(y - h2), w, h);
		else if (dir == 3) rc.set(int(x + arrowW), int(y + arrowH), w, h);
		else if (dir == 4) rc.set(int(x - w2), int(y + arrowH), w, h);
		else if (dir == 5) rc.set(int(x - arrowW - w), int(y + arrowH), w, h);
		else if (dir == 6) rc.set(int(x - arrowW - w), int(y - h2), w, h);
		else if (dir == 7) rc.set(int(x - arrowW - w), int(y - arrowH - h), w, h);
	}

	ShapeStorage roundedBox;
	roundedBox.rounded_rect(rc.left, rc.top, rc.width, rc.height, borderRadius);
	roundedBox.normalize_radius();
	reset();
	agg::path_storage arrow;
	int mnW = Min(w, arrowW);
	int mnH = Min(h, arrowH);
	double sn = fabs(sin(PI / 4));
	double cs = fabs(cos(PI / 4));

	if (invert) {
		if (dir == 0) {
			arrow.move_to(x, y + (arrowH + h));
			arrow.line_to(x + arrowW / 2.0, (double)y + h);
			arrow.line_to(x - arrowW / 2.0, (double)y + h);
		}
		else if (dir == 1) {
			arrow.move_to(x - arrowW - w, y + arrowH + h);
			arrow.line_to((double)x - w, (double)y + h - Max(borderRadius.lb, mnH * sn));
			arrow.line_to((double)x - w + Max(borderRadius.lb, mnW * cs), (double)y + h);
		}
		else if (dir == 2) {
			arrow.move_to(x - arrowW - w, y);
			arrow.line_to((double)x - w, (double)y - arrowH / 2);
			arrow.line_to((double)x - w, (double)y + arrowH / 2);
		}
		else if (dir == 3) {
			arrow.move_to(x - w - arrowW, y - h - arrowH);
			arrow.line_to((double)x - w + Max(borderRadius.lt, mnW * cs), (double)y - h);
			arrow.line_to((double)x - w, (double)y - h + Max(borderRadius.lt, mnH * sn));
		}
		else if (dir == 4) {
			arrow.move_to(x, y - h - arrowH);
			arrow.line_to((double)x + arrowW / 2, (double)y - h);
			arrow.line_to((double)x - arrowW / 2, (double)y - h);
		}
		else if (dir == 5) {
			arrow.move_to(x + w + arrowW, y - h - arrowH);
			arrow.line_to(x + w, y - h + Max(borderRadius.rt, mnH * sn));
			arrow.line_to(x + w - Max(borderRadius.rt, mnW * cs), y - h);
		}
		else if (dir == 6) {
			arrow.move_to(x + arrowW + w, y);
			arrow.line_to(x + w, y - arrowH / 2.0);
			arrow.line_to(x + w, y + arrowH / 2.0);
		}
		else if (dir == 7) {
			arrow.move_to(x + w + arrowW, y + h + arrowH);
			arrow.line_to(x + w - Max(borderRadius.rb, mnW * cs), y + h);
			arrow.line_to(x + w, y + h - Max(borderRadius.rb, mnH * sn));
		}
	}
	else {
		arrow.move_to(x, y);
		if (dir == 0) {
			arrow.line_to(x + arrowW / 2.0, (double)y - arrowH);
			arrow.line_to(x - arrowW / 2.0, (double)y - arrowH);
		}
		else if (dir == 1) {
			arrow.line_to((double)x + arrowW, (double)y - arrowH - Max(borderRadius.lb, mnH * sn));
			arrow.line_to((double)x + arrowW + Max(borderRadius.lb, mnW * cs), (double)y - arrowH);
		}
		else if (dir == 2) {
			arrow.line_to((double)x + arrowW, (double)y - arrowH / 2);
			arrow.line_to((double)x + arrowW, (double)y + arrowH / 2);
		}
		else if (dir == 3) {
			arrow.line_to((double)x + arrowW + Max(borderRadius.lt, mnW * cs), (double)y + arrowH);
			arrow.line_to((double)x + arrowW, (double)y + arrowH + Max(borderRadius.lt, mnH * sn));
		}
		else if (dir == 4) {
			arrow.line_to((double)x + arrowW / 2, (double)y + arrowH);
			arrow.line_to((double)x - arrowW / 2, (double)y + arrowH);
		}
		else if (dir == 5) {
			arrow.line_to(x - arrowW, y + arrowH + Max(borderRadius.rt, mnH * sn));
			arrow.line_to(x - arrowW - Max(borderRadius.rt, mnW * cs), y + arrowH);
		}
		else if (dir == 6) {
			arrow.line_to(x - arrowW, y - arrowH / 2.0);
			arrow.line_to(x - arrowW, y + arrowH / 2.0);
		}
		else if (dir == 7) {
			arrow.line_to(x - arrowW - Max(borderRadius.rb, mnW * cs), y - arrowH);
			arrow.line_to(x - arrowW, y - arrowH - Max(borderRadius.rb, mnH * sn));
		}
	}


	arrow.close_polygon();
	add_path(roundedBox);
	add_path(arrow);
	render(fill);

	Rect rcx(rc);
	rcx.shrink(padd);
	int penLeft, penTop;
	penTop = rcx.top + maxH;
	size_t i = 0;
	wchar_t* b, * e;
	while (true) {
		b = text + i;
		e = wmemchr(b, L'\n', len - i);
		size_t lineLen = e ? size_t(e - b - 1) : len - i;

		if (lineLen > 0) {
			SlotCacher::load_box16(b, lineLen, faceHandle);
			auto& box = faceHandle.box;
			int textW = box.get_width();

			penLeft = rcx.left;
			if (art.alignX == Align::CENTER)
				penLeft += (int)((double)(rc.width - textW) / 2.0);
			else if (art.alignX == Align::RIGHT)
				penLeft += rcx.width - textW;

			_draw_glyphs(penLeft, penTop, &rc);
		}

		penTop += absLineHeight;
		if (!e) break;
		i = i + e - b + 1;
	}

	return textSize.width;
}









































//#include "agg_scanline_boolean_algebra.h"
//
//
//template<class ScanlineGen1,
//	class ScanlineGen2,
//	class Scanline1,
//	class Scanline2,
//	class Scanline,
//	class Renderer,
//	class CombineSpansFunctor>
//	void omid_sbool_intersect_shapes(ScanlineGen1& sg1, ScanlineGen2& sg2,
//		Scanline1& sl1, Scanline2& sl2,
//		Scanline& sl, Renderer& ren,
//		CombineSpansFunctor combine_spans)
//{
//	// Prepare the scanline generators.
//	// If anyone of them doesn't contain 
//	// any scanlines, then return.
//	//-----------------
//	if (!sg1.rewind_scanlines()) return;
//	if (!sg2.rewind_scanlines()) return;
//
//	// Get the bounding boxes
//	//----------------
//	agg::rect_i r1(sg1.min_x(), sg1.min_y(), sg1.max_x(), sg1.max_y());
//	agg::rect_i r2(sg2.min_x(), sg2.min_y(), sg2.max_x(), sg2.max_y());
//
//	// Calculate the intersection of the bounding 
//	// boxes and return if they don't intersect.
//	//-----------------
//	agg::rect_i ir = intersect_rectangles(r1, r2);
//	if (!ir.is_valid()) return;
//
//	// Reset the scanlines and get two first ones
//	//-----------------
//	sl.reset(ir.x1, ir.x2);
//	sl1.reset(sg1.min_x(), sg1.max_x());
//	sl2.reset(sg2.min_x(), sg2.max_x());
//	if (!sg1.sweep_scanline(sl1)) return;
//	if (!sg2.sweep_scanline(sl2)) return;
//
//	ren.prepare();
//
//	// The main loop
//	// Here we synchronize the scanlines with 
//	// the same Y coordinate, ignoring all other ones.
//	// Only scanlines having the same Y-coordinate 
//	// are to be combined.
//	//-----------------
//	for (;;)
//	{
//		while (sl1.y() < sl2.y())
//		{
//			if (!sg1.sweep_scanline(sl1)) return;
//		}
//		while (sl2.y() < sl1.y())
//		{
//			if (!sg2.sweep_scanline(sl2)) return;
//		}
//
//		if (sl1.y() == sl2.y())
//		{
//			// The Y coordinates are the same.
//			// Combine the scanlines, render if they contain any spans,
//			// and advance both generators to the next scanlines
//			//----------------------
//			sbool_intersect_scanlines(sl1, sl2, sl, combine_spans);
//			if (sl.num_spans())
//			{
//				sl.finalize(sl1.y());
//				ren.render(sl);
//			}
//			if (!sg1.sweep_scanline(sl1)) return;
//			if (!sg2.sweep_scanline(sl2)) return;
//		}
//	}
//}
//
//template<class Scanline1,
//	class Scanline2,
//	class Scanline,
//	unsigned CoverShift = agg::cover_shift>
//	struct omid_sbool_intersect_spans_aa
//{
//	enum cover_scale_e
//	{
//		cover_shift = CoverShift,
//		cover_size = 1 << cover_shift,
//		cover_mask = cover_size - 1,
//		cover_full = cover_mask
//	};
//
//
//	void operator () (const typename Scanline1::const_iterator& span1,
//		const typename Scanline2::const_iterator& span2,
//		int x, unsigned len,
//		Scanline& sl) const
//	{
//		unsigned cover;
//		const typename Scanline1::cover_type* covers1;
//		const typename Scanline2::cover_type* covers2;
//
//		// Calculate the operation code and choose the 
//		// proper combination algorithm.
//		// 0 = Both spans are of AA type
//		// 1 = span1 is solid, span2 is AA
//		// 2 = span1 is AA, span2 is solid
//		// 3 = Both spans are of solid type
//		//-----------------
//		switch ((span1->len < 0) | ((span2->len < 0) << 1))
//		{
//		case 0:      // Both are AA spans
//			covers1 = span1->covers;
//			covers2 = span2->covers;
//			if (span1->x < x) covers1 += x - span1->x;
//			if (span2->x < x) covers2 += x - span2->x;
//			do
//			{
//				cover = *covers1++ * *covers2++;
//				sl.add_cell(x++,
//					(cover == cover_full * cover_full) ?
//					cover_full :
//					(cover >> cover_shift));
//			} while (--len);
//			break;
//
//		case 1:      // span1 is solid, span2 is AA
//			covers2 = span2->covers;
//			if (span2->x < x) covers2 += x - span2->x;
//			if (*(span1->covers) == cover_full)
//			{
//				sl.add_cells(x, len, covers2);
//			}
//			else
//			{
//				do
//				{
//					cover = *(span1->covers) * *covers2++;
//					sl.add_cell(x++,
//						(cover == cover_full * cover_full) ?
//						cover_full :
//						(cover >> cover_shift));
//				} while (--len);
//			}
//			break;
//
//		case 2:      // span1 is AA, span2 is solid
//			covers1 = span1->covers;
//			if (span1->x < x) covers1 += x - span1->x;
//			if (*(span2->covers) == cover_full)
//			{
//				sl.add_cells(x, len, covers1);
//			}
//			else
//			{
//				do
//				{
//					cover = *covers1++ * *(span2->covers);
//					sl.add_cell(x++,
//						(cover == cover_full * cover_full) ?
//						cover_full :
//						(cover >> cover_shift));
//				} while (--len);
//			}
//			break;
//
//		case 3:      // Both are solid spans
//			cover = *(span1->covers) * *(span2->covers);
//			sl.add_span(x, len,
//				(cover == cover_full * cover_full) ?
//				cover_full :
//				(cover >> cover_shift));
//			break;
//		}
//	}
//};
//
//void polygon(agg::path_storage& path, double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3) {
//	path.move_to(x0, y0);
//	path.line_to(x1, y1);
//	path.line_to(x2, y2);
//	path.line_to(x3, y3);
//	path.line_to(x0, y0);
//}
//
//void rect(agg::path_storage& path, double x0, double y0, double w, double h) {
//	path.move_to(x0, y0);
//	path.hline_rel(w);
//	path.vline_rel(h);
//	path.hline_rel(-w);
//	path.vline_rel(-h);
//}
//
//bool intersect(double& x1, double& y1, double& x2, double& y2, double lx1, double ly1, double lx2, double ly2, bool trimA) {
//	double d21x, d21y, d43x, d43y, d13x, d13y, d, u, v;
//
//	d21x = x2 - x1;
//	d21y = y2 - y1;
//	d43x = lx2 - lx1;
//	d43y = ly2 - ly1;
//	d13x = x1 - lx1;
//	d13y = y1 - ly1;
//	d = d43y * d21x - d43x * d21y;
//	//rays are paralell, no intersection possible
//	if (fabs(d) < 0.00000001) return false;
//	u = (d13y * d21x - d13x * d21y) / d;
//	v = (d13y * d43x - d13x * d43y) / d;
//
//	//is position on segment?
//	bool ints = v >= 0 && v <= 1;
//	//is position on line?
//	//bool intl = u >= 0 && u <= 1;
//
//	if (ints) {
//		if (trimA) {
//			x1 = x1 + v * d21x;
//			y1 = y1 + v * d21y;
//		}
//		else {
//			x2 = x1 + v * d21x;
//			y2 = y1 + v * d21y;
//		}
//	}
//
//	return ints;
//}
//
//void calcCoef(double& c1, double& c2, int side, bool start) {
//	if (side == 0) c2 = start ? -1 : +1, c1 = +1;
//	else if (side == 1) c1 = start ? +1 : -1, c2 = +1;
//	else if (side == 2) c2 = start ? +1 : -1, c1 = -1;
//	else if (side == 3) c1 = start ? -1 : +1, c2 = -1;
//}
//
//void curve(agg::path_storage& path, double x0, double y0, double x1, double y1, double r1, double r2, double bor1, double bor2, double bor3, int side) {
//	agg::arc arc;
//
//	double xx0, yy0, xx1, yy1, rotate, c1, c2;
//
//	if (side == 0) {
//		xx0 = x0 + bor2;
//		xx1 = x1 + bor2;
//		yy0 = y0 - bor1;
//		yy1 = y1 + bor3;
//		rotate = agg::pi + agg::pi / 2;
//	}
//	else if (side == 1) {
//		xx0 = x0 + bor1;
//		xx1 = x1 - bor3;
//		yy0 = y0 + bor2;
//		yy1 = y1 + bor2;
//		rotate = 0;
//	}
//	else if (side == 2) {
//		xx0 = x0 - bor2;
//		xx1 = x1 - bor2;
//		yy0 = y0 + bor1;
//		yy1 = y1 - bor3;
//		rotate = agg::pi / 2;
//	}
//	else if (side == 3) {
//		xx0 = x0 - bor1;
//		xx1 = x1 + bor3;
//		yy0 = y0 - bor2;
//		yy1 = y1 - bor2;
//		rotate = agg::pi;
//	}
//	else return;
//
//	calcCoef(c1, c2, side, true);
//	arc.init(x0 + r1 * c1, y0 + r1 * c2, r1, r1, agg::pi + rotate, agg::pi + agg::pi / 2 + rotate);
//	arc.rewind(0);
//	double x, y, lx, ly;
//	unsigned cmd = arc.vertex(&x, &y);
//	bool bint = false;
//
//	while (!agg::is_stop(cmd)) {
//		if (bint) {
//			if (cmd == agg::path_cmd_line_to)
//				path.line_to(x, y);
//		}
//		else {
//			if (cmd != agg::path_cmd_move_to) {
//				bint = intersect(lx, ly, x, y, x0, y0, xx0, yy0, true);
//				if (bint) {
//					path.move_to(lx, ly);
//					continue;
//				}
//			}
//		}
//		lx = x, ly = y, cmd = arc.vertex(&x, &y);
//	}
//
//	if (!bint) 
//		path.move_to(lx, ly);
//
//	calcCoef(c1, c2, side, false);
//	arc.init(x1 + r2 * c1, y1 + r2 * c2, r2, r2, agg::pi + agg::pi / 2 + rotate, 0.0 + rotate);
//	arc.rewind(0);
//	cmd = arc.vertex(&x, &y);
//	bint = false;
//
//	while (!agg::is_stop(cmd)) {
//		if (cmd != agg::path_cmd_move_to)
//			bint = intersect(lx, ly, x, y, x1, y1, xx1, yy1, false);
//		if (cmd == agg::path_cmd_line_to) path.line_to(x, y);
//		if (bint) break;
//		lx = x, ly = y, cmd = arc.vertex(&x, &y);
//	}
//
//	if (side == 0) 
//		arc.init(x1 + r2 * c1, y1 + r2 * c2, Max(r2 - bor2, 0), Max(r2 - bor3, 0), 0.0 + rotate, agg::pi + agg::pi / 2 + rotate, false);
//	else if (side == 1)
//		arc.init(x1 + r2 * c1, y1 + r2 * c2, Max(r2 - bor3, 0), Max(r2 - bor2, 0), 0.0 + rotate, agg::pi + agg::pi / 2 + rotate, false);
//	else if (side == 2)
//		arc.init(x1 + r2 * c1, y1 + r2 * c2, Max(r2 - bor2, 0), Max(r2 - bor3, 0), 0.0 + rotate, agg::pi + agg::pi / 2 + rotate, false);
//	else if (side == 3) 
//		arc.init(x1 + r2 * c1, y1 + r2 * c2, Max(r2 - bor3, 0), Max(r2 - bor2, 0), 0.0 + rotate, agg::pi + agg::pi / 2 + rotate, false);
//
//	arc.rewind(0);
//	cmd = arc.vertex(&x, &y);
//	bint = false;
//	double px, py;
//	px = lx, py = ly;
//
//	while (!agg::is_stop(cmd)) {
//		if (bint) {
//			if (cmd == agg::path_cmd_line_to)
//				path.line_to(x, y);
//		}
//		else {
//			if (cmd != agg::path_cmd_move_to) {
//				bint = intersect(lx, ly, x, y, x1, y1, xx1, yy1, true);
//				if (bint) {
//					path.line_to(lx, ly);
//					double d = sqrt(pow(px - lx, 2.0) + pow(py - ly, 2.0));
//					continue;
//				}
//			}
//		}
//		lx = x, ly = y, cmd = arc.vertex(&x, &y);
//	}
//
//	if (!bint)
//		path.line_to(lx, ly);
//
//	calcCoef(c1, c2, side, true);
//
//	if (side == 0)
//		arc.init(x0 + r1 * c1, y0 + r1 * c2, Max(r1 - bor2, 0), Max(r1 - bor1, 0), agg::pi + agg::pi * 0.5 + rotate, agg::pi + rotate, false);
//	else if (side == 1)
//		arc.init(x0 + r1 * c1, y0 + r1 * c2, Max(r1 - bor1, 0), Max(r1 - bor2, 0), agg::pi + agg::pi * 0.5 + rotate, agg::pi + rotate, false);
//	else if (side == 2)
//		arc.init(x0 + r1 * c1, y0 + r1 * c2, Max(r1 - bor2, 0), Max(r1 - bor1, 0), agg::pi + agg::pi * 0.5 + rotate, agg::pi + rotate, false);
//	else if (side == 3)
//		arc.init(x0 + r1 * c1, y0 + r1 * c2, Max(r1 - bor1, 0), Max(r1 - bor2, 0), agg::pi + agg::pi * 0.5 + rotate, agg::pi + rotate, false);
//
//	arc.rewind(0);
//	cmd = arc.vertex(&x, &y);
//	bint = false;
//
//	while (!agg::is_stop(cmd)) {
//		if (cmd != agg::path_cmd_move_to)
//			bint = intersect(lx, ly, x, y, x0, y0, xx0, yy0, false);
//		if (cmd == agg::path_cmd_line_to) path.line_to(x, y);
//		if (bint) break;
//		lx = x, ly = y, cmd = arc.vertex(&x, &y);
//	}
//	path.close_polygon();
//}
//
//void Canvas::draw_border(Rect* area, Border& border, BorderRadius& radius) {
	////double x0, xx0, x1, xx1, x2, xx2, x3, xx3, y0, yy0, y1, yy1, y2, yy2, y3, yy3;
	//double x1, x2, x3, x4, y1, y2, y3, y4;
	//
	//agg::path_storage left, top, right, bottom;
	//
	//x1 = 0;
	//x2 = area->width;
	//x3 = area->width;
	//x4 = 0;
	//
	//y1 = 0;
	//y2 = 0;
	//y3 = area->height;
	//y4 = area->height;
	//
	//pixfmt_bgr pf(sheet, area);
	//renderer_base ren_base(pf);
	//renderer_scanline ren(ren_base);
	//scanline sl_back, sl_result, sl1, sl2, sl_fore;
	//rasterizer_scanline ras_back;
	//
	//agg::rounded_rect rect1(x1, y1, x3, y3, 1);
	//agg::rounded_rect rect2(x1 + border.left, y1 + border.top, x3 - border.right, y3 - border.bottom, 1);
	//rect1.radius(radius.lt, radius.rt, radius.rb, radius.lb);
	//rect2.radius(radius.lt, radius.rt, radius.rb, radius.lb);
	//rect1.normalize_radius();
	//rect2.normalize_radius();
	//ras_back.add_path(rect1);
	//ras_back.add_path(rect2);
	//ras_back.filling_rule(agg::filling_rule_e::fill_even_odd);
	//ren.color(Color(0x7f, 0, 0x7f));
	////agg::render_scanlines(ras_back, sl_back, ren);
	//agg::rasterizer_scanline_aa<> ras_fore;
	//ras_fore.filling_rule(agg::fill_even_odd);
	//omid_sbool_intersect_spans_aa<scanline, scanline, scanline> combine_functor;
	//
	//ras_fore.reset();
	//
	//double rx1, ry1, rx2, ry2, rx3, ry3, rx4, ry4;
	//
	//rx1 = radius.lt;
	//ry1 = radius.lt;
	//rx2 = radius.rt;
	//ry2 = radius.rt;
	//rx3 = radius.rb;
	//ry3 = radius.rb;
	//rx4 = radius.lb;
	//ry4 = radius.lb;
	//
	//double dx, dy, k, t;
	//dx = std::fabs(y3 - y1);
	//dy = std::fabs(x3 - x1);
	//k = 1.0;
	//
	//t = dx / (rx1 + rx2); if (t < k) k = t;
	//t = dx / (rx3 + rx4); if (t < k) k = t;
	//t = dy / (ry1 + ry2); if (t < k) k = t;
	//t = dy / (ry3 + ry4); if (t < k) k = t;
	//
	//if (k < 1.0)
	//{
	//	rx1 *= k; ry1 *= k; rx2 *= k; ry2 *= k;
	//	rx3 *= k; ry3 *= k; rx4 *= k; ry4 *= k;
	//}
	//
	//curve(top, x1, y1, x2, y2, rx1, rx2, border.left, border.top, border.right, 1);
	//curve(right, x2, y2, x3, y3, rx2, rx3, border.top, border.right, border.bottom, 2);
	//curve(bottom, x3, y3, x4, y4, rx3, rx4, border.right, border.bottom, border.left, 3);
	//curve(left, x4, y4, x1, y1, rx4, rx1, border.bottom, border.left, border.top, 0);
	//
	//double wd = 1;
	//agg::conv_stroke<agg::path_storage> cvl(left);
	//cvl.width(wd);
	//ras_fore.add_path(cvl);
	//render_scanlines(ras_fore, sl, pf, border.leftColor);
	//
	//agg::conv_stroke<agg::path_storage> cvt(top);
	//cvt.width(wd);
	//ras_fore.add_path(cvt);
	//render_scanlines(ras_fore, sl, pf, border.topColor);
	//
	//agg::conv_stroke<agg::path_storage> cvr(right);
	//cvr.width(wd);
	//ras_fore.add_path(cvr);
	//render_scanlines(ras_fore, sl, pf, border.rightColor);
	//
	//agg::conv_stroke<agg::path_storage> cvb(bottom);
	//cvb.width(wd);
	//ras_fore.add_path(cvb);
	//render_scanlines(ras_fore, sl, pf, border.botColor);
	//
	//omid_sbool_intersect_shapes(ras_back, ras_fore, sl1, sl2, sl_result, ren, combine_functor);
	//
	//ras_fore.reset();
	//ras_fore.add_path(top);
	//ren.color(border.topColor);
	//omid_sbool_intersect_shapes(ras_back, ras_fore, sl1, sl2, sl_result, ren, combine_functor);
	//
	//ras_fore.reset();
	//ras_fore.add_path(right);
	//ren.color(border.rightColor);
	//omid_sbool_intersect_shapes(ras_back, ras_fore, sl1, sl2, sl_result, ren, combine_functor);
	//
	//ras_fore.reset();
	//ras_fore.add_path(bottom);
	//ren.color(border.botColor);
	//omid_sbool_intersect_shapes(ras_back, ras_fore, sl1, sl2, sl_result, ren, combine_functor);
//}
