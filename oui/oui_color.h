#pragma once
#include "oui_basics.h"
#include "agg_color_rgba.h"

OUI_API bool is_valid_color(std::string text);

inline byte doubleHex(byte value);

struct OUI_API Color {
private:
	std::vector<Color> store;
public:
	enum class ColorType {
		color_type_none,
		color_type_inherit,
		color_type_value,
		color_type_brightness
	};
	byte r, g, b, a;
	ColorType type;
	float f, add;

	Color();
	Color(const Color& color);
	Color(const Color& color, byte opacity);
	Color(const char* color, Color* base = NULL);
	Color(std::string color, Color* base = NULL);
	Color(byte red, byte green, byte blue, byte alpha = 0xff);
	void operator=(const Color& color);
	void save();
	void restore();
	std::string to_string(const Color* color) const;
	Color bright(float add) const;
	Color transparent(float alpha) const;
	double opacity() const;
	float hue_to_rgb(float m1, float m2, float h);
	bool parseColor(std::string& str, Color* base = NULL);
	void opacity(const byte op);
	void brightness(Color* base, float add);
	bool set(std::string color, Color* base = NULL);
	bool set(const char* color, Color* base = NULL);
	void set(const Color& src);
	agg::rgba8 to_rgba8() const;
	inline int to_int() const {
		return ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
	}
	void set(byte red, byte green, byte blue, byte alpha = 0xff);
	void clear();
	void inherit();
	bool has_value();
	bool has_brightness();
	bool is_inherited();
	bool is_none();
};

class Colors {
public:
	OUI_API static const Color aliceblue, antiquewhite, aqua, aquamarine, azure, beige, bisque, black, blanchedalmond,
		blue, blueviolet, brown, burlywood, cadetblue, chartreuse, chocolate, coral, cornflowerblue, cornsilk, crimson, cyan,
		darkblue, darkcyan, darkgoldenrod, darkgray, darkgreen, darkgrey, darkkhaki, darkmagenta, darkolivegreen, darkorange,
		darkorchid, darkred, darksalmon, darkseagreen, darkslateblue, darkslategray, darkslategrey, darkturquoise, darkviolet,
		deeppink, deepskyblue, dimgray, dimgrey, dodgerblue, firebrick, floralwhite, forestgreen, fuchsia, gainsboro, ghostwhite,
		gold, goldenrod, gray, green, greenyellow, grey, honeydew, hotpink, indianred, indigo, ivory, khaki, lavender, lavenderblush,
		lawngreen, lemonchiffon, lightblue, lightcoral, lightcyan, lightgoldenrodyellow, lightgray, lightgreen, lightgrey, lightpink,
		lightsalmon, lightseagreen, lightskyblue, lightslategray, lightslategrey, lightsteelblue, lightyellow, lime, limegreen,
		linen, magenta, maroon, mediumaquamarine, mediumblue, mediumorchid, mediumpurple, mediumseagreen, mediumslateblue,
		mediumspringgreen, mediumturquoise, mediumvioletred, midnightblue, mintcream, mistyrose, moccasin, navajowhite, navy,
		oldlace, olive, olivedrab, orange, orangered, orchid, palegoldenrod, palegreen, paleturquoise, palevioletred, papayawhip,
		peachpuff, peru, pink, plum, powderblue, purple, red, rosybrown, royalblue, saddlebrown, salmon, sandybrown, seagreen,
		seashell, sienna, silver, skyblue, slateblue, slategray, slategrey, snow, springgreen, steelblue, tan, teal, thistle,
		tomato, turquoise, violet, wheat, white, whitesmoke, yellow, yellowgreen;
};

OUI_API Color rand_color(size_t index);
