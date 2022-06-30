#include "oui_sheet.h"

ShapeStorage::ShapeStorage() :
	x1(0), y1(0), x2(0), y2(0),
	rx1(0), ry1(0), rx2(0), ry2(0),
	rx3(0), ry3(0), rx4(0), ry4(0),
	opacity(255),
	curve(poly)
{
	type = ShapeType::shape_type_rect;
	currIndex = 0;
	spreadX = 0;
	spreadY = 0;
	applyMat = false;
	negate = false;
}

void ShapeStorage::rounded_rect(double x1, double y1, double w, double h, BorderRadius& borderRadius) {
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x1 + w;
	this->y2 = y1 + h;

	srx1 = rx1 = borderRadius.lt;
	sry1 = ry1 = borderRadius.lt;
	srx2 = rx2 = borderRadius.rt;
	sry2 = ry2 = borderRadius.rt;
	srx3 = rx3 = borderRadius.rb;
	sry3 = ry3 = borderRadius.rb;
	srx4 = rx4 = borderRadius.lb;
	sry4 = ry4 = borderRadius.lb;

	type = ShapeType::shape_type_rounded_rect;
	normalize_radius();
	finish();
}

void ShapeStorage::rounded_rect(double x1, double y1, double w, double h, BorderRadius& borderRadius, Border& border) {
	this->x1 = x1 + border.left;
	this->y1 = y1 + border.top;
	this->x2 = x1 + w - border.right;
	this->y2 = y1 + h - border.bottom;

	srx1 = rx1 = Max(borderRadius.lt - border.left, 0);
	sry1 = ry1 = Max(borderRadius.lt - border.top, 0);
	srx2 = rx2 = Max(borderRadius.rt - border.right, 0);
	sry2 = ry2 = Max(borderRadius.rt - border.top, 0);
	srx3 = rx3 = Max(borderRadius.rb - border.right, 0);
	sry3 = ry3 = Max(borderRadius.rb - border.bottom, 0);
	srx4 = rx4 = Max(borderRadius.lb - border.left, 0);
	sry4 = ry4 = Max(borderRadius.lb - border.bottom, 0);

	type = ShapeType::shape_type_rounded_rect;
	normalize_radius();
	finish();
}

void ShapeStorage::recti(int x, int y, int w, int h) {
	this->x1 = x;
	this->y1 = y;
	this->x2 = double(x + w);
	this->y2 = double(y + h);
	type = ShapeType::shape_type_rect;
	finish();
}

void ShapeStorage::finish() {
	ras.reset();
	ras.add_path(*this);
}

// rounded rect section
void ShapeStorage::rectf(double x1, double y1, double x2, double y2) {
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
	type = ShapeType::shape_type_rect;
	finish();
}

void ShapeStorage::radius(double r) {
	rx1 = ry1 = rx2 = ry2 = rx3 = ry3 = rx4 = ry4 = r;
	type = ShapeType::shape_type_rounded_rect;
	normalize_radius();
	finish();
}

void ShapeStorage::radius(double rx, double ry) {
	rx1 = rx2 = rx3 = rx4 = rx;
	ry1 = ry2 = ry3 = ry4 = ry;
	type = ShapeType::shape_type_rounded_rect;
	normalize_radius();
	finish();
}

void ShapeStorage::radius(double rx_bottom, double ry_bottom, double rx_top, double ry_top) {
	rx1 = rx2 = rx_bottom;
	rx3 = rx4 = rx_top;
	ry1 = ry2 = ry_bottom;
	ry3 = ry4 = ry_top;
	type = ShapeType::shape_type_rounded_rect;
	normalize_radius();
	finish();
}

void ShapeStorage::radius(double rx1, double ry1, double rx2, double ry2, double rx3, double ry3, double rx4, double ry4) {
	this->rx1 = rx1; this->ry1 = ry1; this->rx2 = rx2; this->ry2 = ry2;
	this->rx3 = rx3; this->ry3 = ry3; this->rx4 = rx4; this->ry4 = ry4;
	type = ShapeType::shape_type_rounded_rect;
	normalize_radius();
	finish();
}

void ShapeStorage::normalize_radius() {
	double dx = std::fabs((y2 + spreadY) - (y1 - spreadY));
	double dy = std::fabs((x2 + spreadX) - (x1 - spreadX));

	rx1 = srx1 + spreadX;
	ry1 = sry1 + spreadY;
	rx2 = srx2 + spreadX;
	ry2 = sry2 + spreadY;
	rx3 = srx3 + spreadX;
	ry3 = sry3 + spreadY;
	rx4 = srx4 + spreadX;
	ry4 = sry4 + spreadY;

	double k = 1.0;
	double t;
	t = dx / (rx1 + rx2); if (t < k) k = t;
	t = dx / (rx3 + rx4); if (t < k) k = t;
	t = dy / (ry1 + ry2); if (t < k) k = t;
	t = dy / (ry3 + ry4); if (t < k) k = t;

	if (k < 1.0)
	{
		rx1 *= k; ry1 *= k; rx2 *= k; ry2 *= k;
		rx3 *= k; ry3 *= k; rx4 *= k; ry4 *= k;
	}
}

void ShapeStorage::rewind(unsigned r) {
	if (type == ShapeType::shape_type_rect) currIndex = 0;
	else if (type == ShapeType::shape_type_rounded_rect) status = 0;
	else curve.rewind(r);
}

unsigned ShapeStorage::vertex(unsigned index, double* x, double* y) {
	if (type == ShapeType::shape_type_polygon) return poly.vertex(index, x, y);
	// default is shape_type_rect
	switch (index) {
	case 0:
		*x = x1;
		*y = y1;
		if (applyMat) mtx.transform(x, y);
		return agg::path_cmd_move_to;
	case 1:
		*x = x2;
		*y = y1;
		if (applyMat) mtx.transform(x, y);
		return agg::path_cmd_line_to;
	case 2:
		*x = x2;
		*y = y2;
		if (applyMat) mtx.transform(x, y);
		return agg::path_cmd_line_to;
	case 3:
		*x = x1;
		*y = y2;
		if (applyMat) mtx.transform(x, y);
		return agg::path_cmd_line_to;
	default:
		*x = 0;
		*y = 0;
		return agg::path_cmd_stop;
	}
}

unsigned ShapeStorage::vertex(double* x, double* y) {
	if (type == ShapeType::shape_type_polygon) return curve.vertex(x, y);
	if (type == ShapeType::shape_type_rect) {
		switch (currIndex++) {
		case 0:
			*x = x1 - spreadX;
			*y = y1 - spreadY;
			if (applyMat) mtx.transform(x, y);
			return agg::path_cmd_move_to;
		case 1:
			*x = x2 + spreadX;
			*y = y1 - spreadY;
			if (applyMat) mtx.transform(x, y);
			return agg::path_cmd_line_to;
		case 2:
			*x = x2 + spreadX;
			*y = y2 + spreadY;
			if (applyMat) mtx.transform(x, y);
			return agg::path_cmd_line_to;
		case 3:
			*x = x1 - spreadX;
			*y = y2 + spreadY;
			if (applyMat) mtx.transform(x, y);
			return agg::path_cmd_line_to;
		default:
			*x = 0;
			*y = 0;
			return agg::path_cmd_stop;
		}
	}

	unsigned cmd = agg::path_cmd_stop;
	switch (status)
	{
	case 0:
		arc.init(x1 + rx1 - spreadX, y1 + ry1 - spreadY, rx1, ry1,
			agg::pi, agg::pi + agg::pi * 0.5);
		arc.rewind(0);
		status++;

	case 1:
		cmd = arc.vertex(x, y);
		if (applyMat && agg::is_vertex(cmd)) mtx.transform(x, y);
		if (agg::is_stop(cmd)) status++;
		else return cmd;

	case 2:
		arc.init(x2 - rx2 + spreadX, y1 + ry2 - spreadY, rx2, ry2,
			agg::pi + agg::pi * 0.5, 0.0);
		arc.rewind(0);
		status++;

	case 3:
		cmd = arc.vertex(x, y);
		if (applyMat && agg::is_vertex(cmd)) mtx.transform(x, y);
		if (agg::is_stop(cmd)) status++;
		else return agg::path_cmd_line_to;

	case 4:
		arc.init(x2 - rx3 + spreadX, y2 - ry3 + spreadY, rx3, ry3,
			0.0, agg::pi * 0.5);
		arc.rewind(0);
		status++;

	case 5:
		cmd = arc.vertex(x, y);
		if (applyMat && agg::is_vertex(cmd)) mtx.transform(x, y);
		if (agg::is_stop(cmd)) status++;
		else return agg::path_cmd_line_to;

	case 6:
		arc.init(x1 + rx4 - spreadX, y2 - ry4 + spreadY, rx4, ry4,
			agg::pi * 0.5, agg::pi);
		arc.rewind(0);
		status++;

	case 7:
		cmd = arc.vertex(x, y);
		if (applyMat && agg::is_vertex(cmd)) mtx.transform(x, y);
		if (agg::is_stop(cmd)) status++;
		else return agg::path_cmd_line_to;

	case 8:
		cmd = agg::path_cmd_end_poly | agg::path_flags_close | agg::path_flags_ccw;
		status++;
		break;
	}
	return cmd;

}

void ShapeStorage::set_matrix(agg::trans_affine mtx) {
	this->mtx = mtx;
	applyMat = true;
	finish();
}

void ShapeStorage::remove_matrix() {
	applyMat = false;
	finish();
}

void ShapeStorage::spread(double spreadX, double spreadY, bool brender) {
	this->spreadX = spreadX;
	this->spreadY = spreadY;
	if (brender) {
		normalize_radius();
		finish();
	}
}

bool ShapeStorage::is_rect() {
	return (type == ShapeType::shape_type_rect);
}

bool ShapeStorage::is_rounded_rect() {
	return (type == ShapeType::shape_type_rounded_rect);
}



Sheet::Sheet()
	: data(0), w(0), sw(0), h(0), sh(0), pitch(0), nbpp(0), pitexcess(0), bAttached(false) {
	offset.reset(0, 0);
}

Sheet::~Sheet() {
	free();
}

void Sheet::free() {
	if (data && !bAttached) ::free(data);
	data = NULL;
	bAttached = false;
	pitch = nbpp = w = h = sw = sh = pitexcess = 0;
}

void Sheet::attach(pyte data, int width, int height, int pitch, int nbpp) {
	if (!data || width < 1 || height < 1 || pitch == 0 || nbpp < 1 || nbpp > 4)
		return;
	bAttached = true;
	this->data = data;
	this->pitch = pitch;
	this->nbpp = nbpp;
	w = sw = width;
	h = sh = height;
	pitexcess = pitch - sw * nbpp;
}

void Sheet::destroy() {
	free();
}

void Sheet::setclip(ShapeStorage* shape, byte opacity, bool reverse) {
	shape->opacity = opacity;
	clipArea.push_back(shape);
	curr = clipArea.back();
	curr->negate = reverse;

	if (curr) {
		if (curr->type == ShapeType::shape_type_rect) {
			x1 = curr->x1;
			y1 = curr->y1;
			x2 = curr->x2;
			y2 = curr->y2;
			curr->mtx.transform(&x1, &y1);
			curr->mtx.transform(&x2, &y2);
			curr->maxx = int(x2 + 1);
			curr->minx = int(x1);
			curr->maxy = int(y2 + 1);
			curr->miny = int(y1);
		}
		else {
			curr->minx = curr->ras.min_x();
			curr->maxx = curr->ras.max_x();
			curr->miny = curr->ras.min_y();
			curr->maxy = curr->ras.max_y();
		}
	}
}

void Sheet::unclip() {
	if (clipArea.size()) clipArea.pop_back();
	curr = clipArea.size() > 0 ? clipArea.back() : NULL;
}

int Sheet::create(int width, int height, int NBPP /*  Number of bytes per pixel */) {
	if (width < 1 || height < 1 || NBPP < 1) return 1;
	free();

	sw = w = width;
	sh = h = height;
	nbpp = NBPP > 0 && NBPP < 5 ? NBPP : OUI_BGR;
	temp = 4 - nbpp;
	pitch = w * nbpp + temp;
	pitexcess = 0;
	data = (pyte)calloc(h * pitch, 1);
	curr = NULL;
	clipArea.clear();

	return data && w == width && height == h && nbpp == NBPP ? 0 : 2;
}

int Sheet::clone(const Sheet* source) {
	if (!source || source->is_useless())
		return 1;

	sw = w = source->w;
	sh = h = source->h;
	nbpp = source->nbpp;
	temp = source->temp;
	pitch = source->pitch;
	pitexcess = source->pitexcess;
	data = (pyte)calloc(h * pitch, 1);
	curr = source->curr;
	clipArea = source->clipArea;

	if (!data) return 2;

	memcpy(data, source->data, h * pitch);
	return 0;
}

int Sheet::copy(const Sheet* source) {
	if (!source || source->is_useless() || source == this)
		return 1;

	curr = source->curr;
	clipArea = source->clipArea;

	if (w == source->w && h == source->h && nbpp == source->nbpp
		&& pitch == source->pitch && source->data && data)
		memcpy(data, source->data, h * pitch);
	else 1;

	return 0;
}


void Sheet::clear_solid(Color& color) {
	if (!data || !w || !h || !nbpp) return;

	byte* d, r = color.r, g = color.g, b = color.b, a = color.a;
	int x, y;

	if (nbpp == OUI_GRAY) {
		memset(data, color.r, h * pitch);
	}
	else if (nbpp == OUI_BGRA) {
		for (y = 0; y < h; ++y) {
			d = data + y * pitch;
			for (x = 0; x < w; ++x, d += 4) {
				d[0] = b;
				d[1] = g;
				d[2] = r;
				d[3] = a;
			}
		}
	}
}

void Sheet::clear(byte value) {
	if (data) memset(data, value, h * pitch);
}

void Sheet::clear(byte red, byte green, byte blue) {
	if (!data || !w || !h || !nbpp) return;

	byte* d;
	int x, y;

	if (nbpp == OUI_GRAY) {
		clear((red + green + blue) / 3);
	}
	else if (nbpp == OUI_BGR) {
		for (y = 0; y < h; ++y) {
			d = data + y * pitch;
			for (x = 0; x < w; ++x, d += 3)
			{
				d[0] = blue;
				d[1] = green;
				d[2] = red;
			}
		}
	}
}

bool Sheet::getPixel(int x, int y, byte* r, byte* g, byte* b) const {
	if (x > -1 && x < w && y > -1 && y < h) {
		if (nbpp == OUI_GRAY) {
			byte v = data[y * pitch + x];
			if (r) *r = v;
			if (g) *g = v;
			if (g) *b = v;

			return true;
		}
		else if (nbpp == OUI_BGR) {
			pyte d = data + y * pitch + 3 * x;
			if (r) *r = d[2];
			if (g) *g = d[1];
			if (g) *b = d[0];
		}
	}

	return false;
}

bool Sheet::getPixel(int x, int y, byte* res) const {
	if (x > -1 && x < w && y > -1 && y < h) {
		if (nbpp == OUI_GRAY) {
			*res = data[y * pitch + x];
			return true;
		}
	}

	*res = 0;
	return false;
}

void Sheet::resize(int cx, int cy) {
	if (cx < 1 || cy < 1) return;

	if (cx > w || cy > h) {
		int tnbpp = nbpp;
		if (data) free();
		create(cx, cy, tnbpp);
	}
	else {
		w = Min(cx, sw);
		h = Min(cy, sh);
	}
}








