#pragma once
#include "oui_basics.h"
#include "oui_color.h"

template<typename T>
struct vec {
	T x, y, z, w;
	void reset(T defaultX) {
		x = defaultX;
	}
	void reset(T defaultX, T defaultY) {
		x = defaultX;
		y = defaultY;
	}
	void reset(T defaultX, T defaultY, T defaultZ) {
		x = defaultX;
		y = defaultY;
		z = defaultZ;
	}
	void reset(T defaultX, T defaultY, T defaultZ, T defaultW) {
		x = defaultX;
		y = defaultY;
		z = defaultZ;
		w = defaultW;
	}
	bool operator==(const vec& rhs) {
		return lhs.x == rhs.x &&
			lhs.y == rhs.y &&
			lhs.z == rhs.z &&
			lhs.w == rhs.w;
	}
	bool operator!= (const vec& rhs) {
		return lhs.x != rhs.x ||
			lhs.y != rhs.y ||
			lhs.z != rhs.z ||
			lhs.w != rhs.w;
	}
};
typedef vec<int> veci;
typedef vec<double> vecf;
typedef vec<__int64> vecll;

struct OUI_API Rect {
	int left, top, width, height;
	veci offset;
	std::vector<Rect> stack;

	Rect();

	Rect(int x, int y, int w, int h);

	void shift(int shiftX, int shiftY);

	void set(const Rect& rect);

	void set(int x, int y, int w, int h);

	bool is_inside(int x, int y);

	bool compare(int l, int t, int cx, int cy);

	void push(int x, int y, int w, int h);

	void pop();

	void shrink(Spacing& spacing);

	void expand(Spacing& spacing);

	int right();

	int bottom();
};

struct OUI_API Spacing {
	int top, right, bottom, left;
	Spacing();
	virtual void reset();
	void set(int all);
	void set(int leftRight, int topBottom);
	void set(int top, int leftRight, int bottom);
	void set(int top, int right, int bottom, int left);
	bool is_empty();
};

struct OUI_API BorderRadius {
	double lt, rt, rb, lb;
	void reset();
	void set(double lt, double rt, double rb, double lb);
	void set(int topLeft, int topRightBottomLeft, int bottomRight);
	void set(int all);
	void set(int topLeftBottomRight, int topRightBottomLeft);
	bool is_empty();
};

struct OUI_API Border : public Spacing {
	Color leftColor, topColor, rightColor, botColor;
	Border();
	void set(int width, Color color);
	void set(int leftRight, int topBottom, Color leftRightColor, Color topBottomColor);
	void set(int top, int leftRight, int bottom, Color topColor, Color leftRightColor, Color bottomColor);
	void set(int top, int right, int bottom, int left, Color topColor, Color rightColor, Color bottomColor, Color leftColor);
	void set_top(int top, Color color);
	void set_right(int right, Color color);
	void set_bottom(int bottom, Color color);
	void set_left(int left, Color color);
	void set_top_color(Color color);
	void set_right_color(Color color);
	void set_bottom_color(Color color);
	void set_left_color(Color color);
	void set_color(Color all);
};
