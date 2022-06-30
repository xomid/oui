#include "oui_color.h"

#pragma region

struct named_color
{
	char name[22];
	byte r, g, b, a;
};

static named_color colors[] = {
	{ "aliceblue",240,248,255, 255 },
	{ "antiquewhite",250,235,215, 255 },
	{ "aqua",0,255,255, 255 },
	{ "aquamarine",127,255,212, 255 },
	{ "azure",240,255,255, 255 },
	{ "beige",245,245,220, 255 },
	{ "bisque",255,228,196, 255 },
	{ "black",0,0,0, 255 },
	{ "blanchedalmond",255,235,205, 255 },
	{ "blue",0,0,255, 255 },
	{ "blueviolet",138,43,226, 255 },
	{ "brown",165,42,42, 255 },
	{ "burlywood",222,184,135, 255 },
	{ "cadetblue",95,158,160, 255 },
	{ "chartreuse",127,255,0, 255 },
	{ "chocolate",210,105,30, 255 },
	{ "coral",255,127,80, 255 },
	{ "cornflowerblue",100,149,237, 255 },
	{ "cornsilk",255,248,220, 255 },
	{ "crimson",220,20,60, 255 },
	{ "cyan",0,255,255, 255 },
	{ "darkblue",0,0,139, 255 },
	{ "darkcyan",0,139,139, 255 },
	{ "darkgoldenrod",184,134,11, 255 },
	{ "darkgray",169,169,169, 255 },
	{ "darkgreen",0,100,0, 255 },
	{ "darkgrey",169,169,169, 255 },
	{ "darkkhaki",189,183,107, 255 },
	{ "darkmagenta",139,0,139, 255 },
	{ "darkolivegreen",85,107,47, 255 },
	{ "darkorange",255,140,0, 255 },
	{ "darkorchid",153,50,204, 255 },
	{ "darkred",139,0,0, 255 },
	{ "darksalmon",233,150,122, 255 },
	{ "darkseagreen",143,188,143, 255 },
	{ "darkslateblue",72,61,139, 255 },
	{ "darkslategray",47,79,79, 255 },
	{ "darkslategrey",47,79,79, 255 },
	{ "darkturquoise",0,206,209, 255 },
	{ "darkviolet",148,0,211, 255 },
	{ "deeppink",255,20,147, 255 },
	{ "deepskyblue",0,191,255, 255 },
	{ "dimgray",105,105,105, 255 },
	{ "dimgrey",105,105,105, 255 },
	{ "dodgerblue",30,144,255, 255 },
	{ "firebrick",178,34,34, 255 },
	{ "floralwhite",255,250,240, 255 },
	{ "forestgreen",34,139,34, 255 },
	{ "fuchsia",255,0,255, 255 },
	{ "gainsboro",220,220,220, 255 },
	{ "ghostwhite",248,248,255, 255 },
	{ "gold",255,215,0, 255 },
	{ "goldenrod",218,165,32, 255 },
	{ "gray",128,128,128, 255 },
	{ "green",0,128,0, 255 },
	{ "greenyellow",173,255,47, 255 },
	{ "grey",128,128,128, 255 },
	{ "honeydew",240,255,240, 255 },
	{ "hotpink",255,105,180, 255 },
	{ "indianred",205,92,92, 255 },
	{ "indigo",75,0,130, 255 },
	{ "ivory",255,255,240, 255 },
	{ "khaki",240,230,140, 255 },
	{ "lavender",230,230,250, 255 },
	{ "lavenderblush",255,240,245, 255 },
	{ "lawngreen",124,252,0, 255 },
	{ "lemonchiffon",255,250,205, 255 },
	{ "lightblue",173,216,230, 255 },
	{ "lightcoral",240,128,128, 255 },
	{ "lightcyan",224,255,255, 255 },
	{ "lightgoldenrodyellow",250,250,210, 255 },
	{ "lightgray",211,211,211, 255 },
	{ "lightgreen",144,238,144, 255 },
	{ "lightgrey",211,211,211, 255 },
	{ "lightpink",255,182,193, 255 },
	{ "lightsalmon",255,160,122, 255 },
	{ "lightseagreen",32,178,170, 255 },
	{ "lightskyblue",135,206,250, 255 },
	{ "lightslategray",119,136,153, 255 },
	{ "lightslategrey",119,136,153, 255 },
	{ "lightsteelblue",176,196,222, 255 },
	{ "lightyellow",255,255,224, 255 },
	{ "lime",0,255,0, 255 },
	{ "limegreen",50,205,50, 255 },
	{ "linen",250,240,230, 255 },
	{ "magenta",255,0,255, 255 },
	{ "maroon",128,0,0, 255 },
	{ "mediumaquamarine",102,205,170, 255 },
	{ "mediumblue",0,0,205, 255 },
	{ "mediumorchid",186,85,211, 255 },
	{ "mediumpurple",147,112,219, 255 },
	{ "mediumseagreen",60,179,113, 255 },
	{ "mediumslateblue",123,104,238, 255 },
	{ "mediumspringgreen",0,250,154, 255 },
	{ "mediumturquoise",72,209,204, 255 },
	{ "mediumvioletred",199,21,133, 255 },
	{ "midnightblue",25,25,112, 255 },
	{ "mintcream",245,255,250, 255 },
	{ "mistyrose",255,228,225, 255 },
	{ "moccasin",255,228,181, 255 },
	{ "navajowhite",255,222,173, 255 },
	{ "navy",0,0,128, 255 },
	{ "oldlace",253,245,230, 255 },
	{ "olive",128,128,0, 255 },
	{ "olivedrab",107,142,35, 255 },
	{ "orange",255,165,0, 255 },
	{ "orangered",255,69,0, 255 },
	{ "orchid",218,112,214, 255 },
	{ "palegoldenrod",238,232,170, 255 },
	{ "palegreen",152,251,152, 255 },
	{ "paleturquoise",175,238,238, 255 },
	{ "palevioletred",219,112,147, 255 },
	{ "papayawhip",255,239,213, 255 },
	{ "peachpuff",255,218,185, 255 },
	{ "peru",205,133,63, 255 },
	{ "pink",255,192,203, 255 },
	{ "plum",221,160,221, 255 },
	{ "powderblue",176,224,230, 255 },
	{ "purple",128,0,128, 255 },
	{ "red",255,0,0, 255 },
	{ "rosybrown",188,143,143, 255 },
	{ "royalblue",65,105,225, 255 },
	{ "saddlebrown",139,69,19, 255 },
	{ "salmon",250,128,114, 255 },
	{ "sandybrown",244,164,96, 255 },
	{ "seagreen",46,139,87, 255 },
	{ "seashell",255,245,238, 255 },
	{ "sienna",160,82,45, 255 },
	{ "silver",192,192,192, 255 },
	{ "skyblue",135,206,235, 255 },
	{ "slateblue",106,90,205, 255 },
	{ "slategray",112,128,144, 255 },
	{ "slategrey",112,128,144, 255 },
	{ "snow",255,250,250, 255 },
	{ "springgreen",0,255,127, 255 },
	{ "steelblue",70,130,180, 255 },
	{ "tan",210,180,140, 255 },
	{ "teal",0,128,128, 255 },
	{ "thistle",216,191,216, 255 },
	{ "tomato",255,99,71, 255 },
	{ "turquoise",64,224,208, 255 },
	{ "violet",238,130,238, 255 },
	{ "wheat",245,222,179, 255 },
	{ "white",255,255,255, 255 },
	{ "whitesmoke",245,245,245, 255 },
	{ "yellow",255,255,0, 255 },
	{ "yellowgreen",154,205,50, 255 },
	{ "zzzzzzzzzzz",0,0,0, 0 }
};

#pragma endregion

int __cmp_color(const void* p1, const void* p2) {
	return strcmp(((named_color*)p1)->name, ((named_color*)p2)->name);
}

OUI_API bool is_valid_color(std::string text) {
	const char* str = text.c_str();
	named_color clr;
	std::string param;
	size_t c;
	auto len = text.length();
	auto i = ocom::find(str, len, "#", 1);
	if (i != std::string::npos && ((i + len) == 9 || (i + len == 7) || (i + len == 4)))
		return true;

	/*if (!compare(str, "transparent", true) ||
		!compare(str, "currentColor", true)) return true;*/

#define bfind2(str, len, q, lenq) (find(str, len, q, lenq) > -1)

	if (ocom::begin(str, "rgb(", false) && len >= 5) {
		param = text.substr(4, len - 5);
		c = ocom::count_char_outside_paranthesis(param, ',');
		return (param[len - 1] == ')' && ocom::is_in_range(double(c), 2, 3, true, true));
	}

	if (ocom::begin(str, "rgba(", false) && len >= 6) {
		param = text.substr(5, text.length() - 6);
		c = ocom::count_char_outside_paranthesis(param, ',');
		return (param[len - 1] == ')' && ocom::is_in_range(double(c), 2, 3, true, true));
	}

	if (ocom::begin(str, "hsl(", false) && len >= 5) {
		param = text.substr(4, text.length() - 5);
		c = ocom::count_char_outside_paranthesis(param, ',');
		return (param[len - 1] == ')' && ocom::is_in_range(double(c), 2, 3, true, true));
	}

	if (ocom::begin(str, "hsla(", false) && len >= 6) {
		param = text.substr(5, text.length() - 6);
		c = ocom::count_char_outside_paranthesis(param, ',');
		return (param[len - 1] == ')' && ocom::is_in_range(double(c), 2, 3, true, true));
	}

	if (ocom::begin(str, "brightness(", false) && len >= 12) {
		param = text.substr(11, text.length() - 12);
		c = ocom::count_char_outside_paranthesis(param, ',');
		return (param[len - 1] == ')' && c == 0);
	}

	if (len > sizeof(clr.name) - 1)
		return false;
	strcpy_s(clr.name, sizeof(clr.name), str);
	const void* p = bsearch(&clr,
		colors,
		sizeof(colors) / sizeof(colors[0]),
		sizeof(colors[0]),
		__cmp_color);
	if (p == 0)
		return false;
	return true;
}

inline byte doubleHex(byte value) {
	byte c = value & 0xf;
	return c << 4 | c;
}

Color::Color() {
	r = g = b = 0;
	a = 0xff;
	f = add = 0.0f;
	clear();
}
Color::Color(const SimpleColor& color) {
	set(color.r, color.g, color.b, color.a);
}
Color::Color(const Color& color) : Color() {
	set(color.r, color.g, color.b, color.a);
}
Color::Color(const Color& color, byte opacity) : Color() {
	set(color.r, color.g, color.b, DIV255(color.a * opacity));
}
Color::Color(const char* color, Color* base) : Color() {
	set(std::string(color), base);
}
Color::Color(const wchar_t* color, Color* base) : Color() {
	set(std::wstring(color), base);
}
Color::Color(std::string color, Color* base) : Color() {
	set(color, base);
}
Color::Color(std::wstring color, Color* base) : Color() {
	set(color, base);
}
Color::Color(byte red, byte green, byte blue, byte alpha) : Color() {
	set(red, green, blue, alpha);
}

void Color::operator=(const Color& color) {
	set(color);
}

void Color::save() {
	store.push_back(*this);
}

void Color::restore() {
	if (!store.size()) throw;
	set(store.back());
	store.pop_back();
}

std::string Color::to_string(const Color* color) const {
	if (!color) return "";
	else if (color->a == 0xff) return ocom::to_hex(color->r, color->g, color->b, color->a);
	auto alp = std::to_string(color->a / 255.0);
	return "rgba(" + std::to_string(color->r) + ", " + std::to_string(color->g) +
		", " + std::to_string(color->b) + ", " + alp.substr(0, alp.find('.') + 3) + ")";
}

float Color::hue_to_rgb(float m1, float m2, float h) {
	if (h < 0.0f) {
		h += 1.0f;
	}
	else if (h > 1.0f) {
		h -= 1.0f;
	}

	if (h * 6.0f < 1.0f) {
		return m1 + (m2 - m1) * h * 6.0f;
	}
	if (h * 2.0f < 1.0f) {
		return m2;
	}
	if (h * 3.0f < 2.0f) {
		return m1 + (m2 - m1) * (2.0f / 3.0f - h) * 6.0f;
	}
	return m1;
}

inline bool __has_char(std::string& str, char c) {
	return str.find(c);
}

inline bool __has_word(std::string& str, std::string word) {
	return str.find(word);
}

bool Color::parseColor(std::string& str, Color* base) {
	auto len = str.length();

	if (ocom::begin(str.c_str(), "brightness", false) && len >= 12) {
		auto loc = str.find(')');
		double d;
		if (loc == std::string::npos || !base) return false;
		str = str.substr(11, loc - 12);
		d = atof(str.c_str());
		float b = float(d * 0xff + 0.5);
		brightness(base, b); // no change in alpha
		return true;
	}

	if (len > 3 && str[0] == '#')
	{
		unsigned c = 0;
		sscanf_s(str.c_str() + 1, "%x", &c);
		if (len == 9) set((c >> 24) & 0xFF, (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
		else if (len == 5) set(doubleHex((c >> 12) & 0xf), doubleHex((c >> 8) & 0xf), doubleHex((c >> 4) & 0xf), doubleHex(c & 0xf));
		else if (len == 4) set(doubleHex((c >> 8) & 0xf), doubleHex((c >> 4) & 0xf), doubleHex(c & 0xf));
		else set((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF, 255);
		return true;
	}

	size_t i, j, t;
	if (ocom::begin(str.c_str(), "rgb", false) && len >= 5)
	{
		i = str.find('(');
		j = str.find(')');

		if (i != std::string::npos && j != std::string::npos) {
			auto color = str.substr(i + 1, j - i - 1);
			double a, b, g, r;

			bool p, brgba = str.find('a') != std::string::npos;
			t = 0;
			a = 255;
			b = g = r = 0;

			while (color.length()) {
				std::string part;
				i = color.find(',');
				part = color.substr(0, i);
				color = i == std::string::npos ? "" : color.substr(i + 1);

				switch (t++) {
				case 0:
					r = atof(part.c_str());
					p = __has_char(part, '%');
					r = p ? r * 2.55 : r;
					break;
				case 1:
					g = atof(part.c_str());
					p = __has_char(part, '%');
					g = p ? g * 2.55 : g;
					break;
				case 2:
					b = atof(part.c_str());
					p = __has_char(part, '%');
					b = p ? b * 2.55 : b;
					break;
				case 3:
					a = atof(part.c_str()) * 255;
					break;
				}
			}

			if (t < 3) return false;

			set((byte)CLAMP255(r),
				(byte)CLAMP255(g),
				(byte)CLAMP255(b),
				(byte)CLAMP255(a));
		}
		return true;
	}

	if (ocom::begin(str.c_str(), "hsl", false) && len >= 5)
	{
		size_t j, t;
		i = str.find('(');
		j = str.find(')');

		if (i != std::string::npos && j != std::string::npos) {
			std::string color = str.substr(i + 1, j - i - 1);
			double h, s, l, a;

			bool p, brgba = __has_char(str, 'a');
			t = 0;
			a = 255;
			h = s = l = 0;

			while (color.length()) {
				std::string part;
				i = color.find(',');
				part = color.substr(0, i);
				color = i < 0 ? "" : color.substr(i + 1);

				switch (t++) {
				case 0:
					h = atof(part.c_str()) / 360.0;
					break;
				case 1:
					s = atof(part.c_str());
					p = __has_char(part, '%');
					if (p) s = s / 100;
					break;
				case 2:
					l = atof(part.c_str());
					p = __has_char(part, '%');
					if (p) l = l / 100;
					break;
				case 3:
					a = atof(part.c_str()) * 255;
					break;
				}
			}

			if (t < 3) return false;

			float m2 = (float)l <= 0.5f ? (float)(l * (s + 1.0)) : (float)(l + s - l * s);
			float m1 = (float)l * 2.0f - m2;

			byte r, g, b;
			r = CLAMP255(hue_to_rgb(m1, m2, (float)h + 1.0f / 3.0f) * 255.0f);
			g = CLAMP255(hue_to_rgb(m1, m2, (float)h) * 255.0f);
			b = CLAMP255(hue_to_rgb(m1, m2, (float)h - 1.0f / 3.0f) * 255.0f);

			set(r, g, b, (byte)a);
		}
		return true;
	}

	named_color c;
	if (len > sizeof(c.name) - 1)
		return false;

	strcpy_s(c.name, sizeof(c.name), str.c_str());
	const void* p = bsearch(&c,
		colors,
		sizeof(colors) / sizeof(colors[0]),
		sizeof(colors[0]),
		__cmp_color);
	if (p == 0)
		return false;

	const named_color* pc = (const named_color*)p;
	set(pc->r, pc->g, pc->b, pc->a);

	return true;
}

void Color::opacity(byte op) {
	a = CLAMP255(op);
}

double Color::opacity() const {
	return a;
}

void Color::brightness(Color* base, float add) {
	if (!base) return;
	type = ColorType::color_type_brightness;
	this->add = add;
	set(CLAMP255(base->r + add), CLAMP255(base->g + add), CLAMP255(base->b + add), base->a);
}

Color Color::bright(float add) const {
	return Color(CLAMP255(r + add), CLAMP255(g + add), CLAMP255(b + add), a);
}

Color Color::transparent(float alpha) const {
	Color res;
	int clamped_a = int(alpha * 0xff);
	clamped_a = clamped_a > 255 ? 255 : clamped_a < 0 ? 0 : clamped_a;
	res.set(r, g, b, byte(clamped_a));
	return res;
}

bool Color::set(std::string color, Color* base) {
	return parseColor(color, base);
}

bool Color::set(std::wstring color, Color* base) {
	auto scolor = ocom::to_string(color);
	return parseColor(scolor, base);
}

bool Color::set(const char* color, Color* base) {
	auto str = std::string(color);
	return parseColor(str, base);
}

bool Color::set(const wchar_t* color, Color* base) {
	auto wstr = std::wstring(color);
	auto scolor = ocom::to_string(wstr);
	return parseColor(scolor, base);
}

void Color::set(const Color& src) {
	set(src.r, src.g, src.b, src.a);
}

agg::rgba8 Color::to_rgba8() const {
	return agg::rgba8(r, g, b, a);
}

void Color::set(byte red, byte green, byte blue, byte alpha) {
	r = red;
	g = green;
	b = blue;
	a = alpha;
	type = ColorType::color_type_value;
}

void Color::clear() {
	type = ColorType::color_type_none;
}

void Color::inherit() {
	type = ColorType::color_type_inherit;
}

bool Color::has_value() {
	return type == ColorType::color_type_value;
}

bool Color::has_brightness() {
	return type == ColorType::color_type_brightness;
}

bool Color::is_inherited() {
	return type == ColorType::color_type_inherit;
}

bool Color::is_none() {
	return type == ColorType::color_type_none;
}

Color Color::get_contrast_color() const {
	double luminance = (0.299 * r + 0.587 * g + 0.114 * b) / 255;
	byte d = 0;
	if (luminance <= 0.5) d = 255;
	return Color(d, d, d);
}

// STATIC GLOBAL COLOR FUNCTIONS

OUI_API Color rand_color(size_t index) {
	index %= 148;
	auto& c = colors[index];
	return Color(c.r, c.g, c.b, c.a);
}


OUI_API void rgb_to_hsb(int red, int green, int blue, int& hue, int& sat, int& bri)
{
	double maxRGB, minRGB, Delta;
	double h, s, b;

	//hue = 100;
	minRGB = Min(Min(red, green), blue);
	maxRGB = Max(Max(red, green), blue);
	Delta = maxRGB - minRGB;

	b = maxRGB;
	if (maxRGB > 0)
		s = 255 * Delta / maxRGB;
	else
		s = 0;

	if (s > 0)
	{
		if (red == maxRGB)
			h = (green - blue) / Delta;
		else
			if (green == maxRGB)
				h = 2 + (blue - red) / Delta;
			else
				if (blue == maxRGB)
					h = 4 + (red - green) / Delta;
	}
	else
		h = -1;

	h = h * 60;
	h = (h < 0) ? h + 360 : h;

	if (red == green && green == blue)
		h = 0;

	hue = (int)(h + 0.5);
	sat = (int)(((s * 100.000) / 255.000) + 0.5);
	bri = (int)(((b * 100.000 / 255.00)) + 0.5);
}

OUI_API void hsb_to_rgb(int hue, int sat, int bri, byte& red, byte& green, byte& blue) {
	double      hh, p, q, t, ff, b, s;
	long        i;
	double ir, ig, ib;

	if (sat <= 0) {// < is bogus, just shuts up warnings
		ir = bri;
		ig = bri;
		ib = bri;
	}
	else {
		if (hue >= 360) hue = 0;

		hh = double(hue) / 60.0;
		s = double(sat) / 100.0;
		b = double(bri) / 100.0;

		i = (long)hh;
		ff = hh - i;
		p = b * (1.0 - s);
		q = b * (1.0 - (s * ff));
		t = b * (1.0 - (s * (1.0 - ff)));

		switch (i) {
		case 0:
			ir = b;
			ig = t;
			ib = p;
			break;
		case 1:
			ir = q;
			ig = b;
			ib = p;
			break;
		case 2:
			ir = p;
			ig = b;
			ib = t;
			break;

		case 3:
			ir = p;
			ig = q;
			ib = b;
			break;
		case 4:
			ir = t;
			ig = p;
			ib = b;
			break;
		case 5:
		default:
			ir = b;
			ig = p;
			ib = q;
			break;
		}
	}

	red = CLAMP255((int)(ir * 255 + 0.5));
	green = CLAMP255((int)(ig * 255 + 0.5));
	blue = CLAMP255((int)(ib * 255 + 0.5));
}
