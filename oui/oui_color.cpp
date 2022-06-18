#include "oui_color.h"

#pragma region

struct named_color
{
	char name[22];
	byte r, g, b, a;
};

const Color Colors::aliceblue = Color(240, 248, 255, 255);
const Color Colors::antiquewhite = Color(250, 235, 215, 255);
const Color Colors::aqua = Color(0, 255, 255, 255);
const Color Colors::aquamarine = Color(127, 255, 212, 255);
const Color Colors::azure = Color(240, 255, 255, 255);
const Color Colors::beige = Color(245, 245, 220, 255);
const Color Colors::bisque = Color(255, 228, 196, 255);
const Color Colors::black = Color(0, 0, 0, 255);
const Color Colors::blanchedalmond = Color(255, 235, 205, 255);
const Color Colors::blue = Color(0, 0, 255, 255);
const Color Colors::blueviolet = Color(138, 43, 226, 255);
const Color Colors::brown = Color(165, 42, 42, 255);
const Color Colors::burlywood = Color(222, 184, 135, 255);
const Color Colors::cadetblue = Color(95, 158, 160, 255);
const Color Colors::chartreuse = Color(127, 255, 0, 255);
const Color Colors::chocolate = Color(210, 105, 30, 255);
const Color Colors::coral = Color(255, 127, 80, 255);
const Color Colors::cornflowerblue = Color(100, 149, 237, 255);
const Color Colors::cornsilk = Color(255, 248, 220, 255);
const Color Colors::crimson = Color(220, 20, 60, 255);
const Color Colors::cyan = Color(0, 255, 255, 255);
const Color Colors::darkblue = Color(0, 0, 139, 255);
const Color Colors::darkcyan = Color(0, 139, 139, 255);
const Color Colors::darkgoldenrod = Color(184, 134, 11, 255);
const Color Colors::darkgray = Color(169, 169, 169, 255);
const Color Colors::darkgreen = Color(0, 100, 0, 255);
const Color Colors::darkgrey = Color(169, 169, 169, 255);
const Color Colors::darkkhaki = Color(189, 183, 107, 255);
const Color Colors::darkmagenta = Color(139, 0, 139, 255);
const Color Colors::darkolivegreen = Color(85, 107, 47, 255);
const Color Colors::darkorange = Color(255, 140, 0, 255);
const Color Colors::darkorchid = Color(153, 50, 204, 255);
const Color Colors::darkred = Color(139, 0, 0, 255);
const Color Colors::darksalmon = Color(233, 150, 122, 255);
const Color Colors::darkseagreen = Color(143, 188, 143, 255);
const Color Colors::darkslateblue = Color(72, 61, 139, 255);
const Color Colors::darkslategray = Color(47, 79, 79, 255);
const Color Colors::darkslategrey = Color(47, 79, 79, 255);
const Color Colors::darkturquoise = Color(0, 206, 209, 255);
const Color Colors::darkviolet = Color(148, 0, 211, 255);
const Color Colors::deeppink = Color(255, 20, 147, 255);
const Color Colors::deepskyblue = Color(0, 191, 255, 255);
const Color Colors::dimgray = Color(105, 105, 105, 255);
const Color Colors::dimgrey = Color(105, 105, 105, 255);
const Color Colors::dodgerblue = Color(30, 144, 255, 255);
const Color Colors::firebrick = Color(178, 34, 34, 255);
const Color Colors::floralwhite = Color(255, 250, 240, 255);
const Color Colors::forestgreen = Color(34, 139, 34, 255);
const Color Colors::fuchsia = Color(255, 0, 255, 255);
const Color Colors::gainsboro = Color(220, 220, 220, 255);
const Color Colors::ghostwhite = Color(248, 248, 255, 255);
const Color Colors::gold = Color(255, 215, 0, 255);
const Color Colors::goldenrod = Color(218, 165, 32, 255);
const Color Colors::gray = Color(128, 128, 128, 255);
const Color Colors::green = Color(0, 128, 0, 255);
const Color Colors::greenyellow = Color(173, 255, 47, 255);
const Color Colors::grey = Color(128, 128, 128, 255);
const Color Colors::honeydew = Color(240, 255, 240, 255);
const Color Colors::hotpink = Color(255, 105, 180, 255);
const Color Colors::indianred = Color(205, 92, 92, 255);
const Color Colors::indigo = Color(75, 0, 130, 255);
const Color Colors::ivory = Color(255, 255, 240, 255);
const Color Colors::khaki = Color(240, 230, 140, 255);
const Color Colors::lavender = Color(230, 230, 250, 255);
const Color Colors::lavenderblush = Color(255, 240, 245, 255);
const Color Colors::lawngreen = Color(124, 252, 0, 255);
const Color Colors::lemonchiffon = Color(255, 250, 205, 255);
const Color Colors::lightblue = Color(173, 216, 230, 255);
const Color Colors::lightcoral = Color(240, 128, 128, 255);
const Color Colors::lightcyan = Color(224, 255, 255, 255);
const Color Colors::lightgoldenrodyellow = Color(250, 250, 210, 255);
const Color Colors::lightgray = Color(211, 211, 211, 255);
const Color Colors::lightgreen = Color(144, 238, 144, 255);
const Color Colors::lightgrey = Color(211, 211, 211, 255);
const Color Colors::lightpink = Color(255, 182, 193, 255);
const Color Colors::lightsalmon = Color(255, 160, 122, 255);
const Color Colors::lightseagreen = Color(32, 178, 170, 255);
const Color Colors::lightskyblue = Color(135, 206, 250, 255);
const Color Colors::lightslategray = Color(119, 136, 153, 255);
const Color Colors::lightslategrey = Color(119, 136, 153, 255);
const Color Colors::lightsteelblue = Color(176, 196, 222, 255);
const Color Colors::lightyellow = Color(255, 255, 224, 255);
const Color Colors::lime = Color(0, 255, 0, 255);
const Color Colors::limegreen = Color(50, 205, 50, 255);
const Color Colors::linen = Color(250, 240, 230, 255);
const Color Colors::magenta = Color(255, 0, 255, 255);
const Color Colors::maroon = Color(128, 0, 0, 255);
const Color Colors::mediumaquamarine = Color(102, 205, 170, 255);
const Color Colors::mediumblue = Color(0, 0, 205, 255);
const Color Colors::mediumorchid = Color(186, 85, 211, 255);
const Color Colors::mediumpurple = Color(147, 112, 219, 255);
const Color Colors::mediumseagreen = Color(60, 179, 113, 255);
const Color Colors::mediumslateblue = Color(123, 104, 238, 255);
const Color Colors::mediumspringgreen = Color(0, 250, 154, 255);
const Color Colors::mediumturquoise = Color(72, 209, 204, 255);
const Color Colors::mediumvioletred = Color(199, 21, 133, 255);
const Color Colors::midnightblue = Color(25, 25, 112, 255);
const Color Colors::mintcream = Color(245, 255, 250, 255);
const Color Colors::mistyrose = Color(255, 228, 225, 255);
const Color Colors::moccasin = Color(255, 228, 181, 255);
const Color Colors::navajowhite = Color(255, 222, 173, 255);
const Color Colors::navy = Color(0, 0, 128, 255);
const Color Colors::oldlace = Color(253, 245, 230, 255);
const Color Colors::olive = Color(128, 128, 0, 255);
const Color Colors::olivedrab = Color(107, 142, 35, 255);
const Color Colors::orange = Color(255, 165, 0, 255);
const Color Colors::orangered = Color(255, 69, 0, 255);
const Color Colors::orchid = Color(218, 112, 214, 255);
const Color Colors::palegoldenrod = Color(238, 232, 170, 255);
const Color Colors::palegreen = Color(152, 251, 152, 255);
const Color Colors::paleturquoise = Color(175, 238, 238, 255);
const Color Colors::palevioletred = Color(219, 112, 147, 255);
const Color Colors::papayawhip = Color(255, 239, 213, 255);
const Color Colors::peachpuff = Color(255, 218, 185, 255);
const Color Colors::peru = Color(205, 133, 63, 255);
const Color Colors::pink = Color(255, 192, 203, 255);
const Color Colors::plum = Color(221, 160, 221, 255);
const Color Colors::powderblue = Color(176, 224, 230, 255);
const Color Colors::purple = Color(128, 0, 128, 255);
const Color Colors::red = Color(255, 0, 0, 255);
const Color Colors::rosybrown = Color(188, 143, 143, 255);
const Color Colors::royalblue = Color(65, 105, 225, 255);
const Color Colors::saddlebrown = Color(139, 69, 19, 255);
const Color Colors::salmon = Color(250, 128, 114, 255);
const Color Colors::sandybrown = Color(244, 164, 96, 255);
const Color Colors::seagreen = Color(46, 139, 87, 255);
const Color Colors::seashell = Color(255, 245, 238, 255);
const Color Colors::sienna = Color(160, 82, 45, 255);
const Color Colors::silver = Color(192, 192, 192, 255);
const Color Colors::skyblue = Color(135, 206, 235, 255);
const Color Colors::slateblue = Color(106, 90, 205, 255);
const Color Colors::slategray = Color(112, 128, 144, 255);
const Color Colors::slategrey = Color(112, 128, 144, 255);
const Color Colors::snow = Color(255, 250, 250, 255);
const Color Colors::springgreen = Color(0, 255, 127, 255);
const Color Colors::steelblue = Color(70, 130, 180, 255);
const Color Colors::tan = Color(210, 180, 140, 255);
const Color Colors::teal = Color(0, 128, 128, 255);
const Color Colors::thistle = Color(216, 191, 216, 255);
const Color Colors::tomato = Color(255, 99, 71, 255);
const Color Colors::turquoise = Color(64, 224, 208, 255);
const Color Colors::violet = Color(238, 130, 238, 255);
const Color Colors::wheat = Color(245, 222, 179, 255);
const Color Colors::white = Color(255, 255, 255, 255);
const Color Colors::whitesmoke = Color(245, 245, 245, 255);
const Color Colors::yellow = Color(255, 255, 0, 255);
const Color Colors::yellowgreen = Color(154, 205, 50, 255);

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
Color::Color(const Color& color) : Color() {
	set(color.r, color.g, color.b, color.a);
}
Color::Color(const Color& color, byte opacity) : Color() {
	set(color.r, color.g, color.b, DIV255(color.a * opacity));
}
Color::Color(const char* color, Color* base) : Color() {
	set(std::string(color), base);
}
Color::Color(std::string color, Color* base) : Color() {
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
				color = i < 0 ? "" : color.substr(i + 1);

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

bool Color::set(const char* color, Color* base) {
	auto str = std::string(color);
	return parseColor(str, base);
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

OUI_API Color rand_color(size_t index) {
	index %= 148;
	auto& c = colors[index];
	return Color(c.r, c.g, c.b, c.a);
}
