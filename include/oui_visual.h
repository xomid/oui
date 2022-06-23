#pragma once
#include "agg_svg_parser.h"
#include "oui_sheet.h"

enum class OUI_API SortType {
	None = 0,
	Ascending = 1,
	Descending = 2
};


enum class OUI_API MouseDragStatus {
	STRATED = 0,
	STOPPED
};

enum class OUI_API MouseDragDirection {
	VERTICALLY = 0,
	HORIZONTALLY,
	BOTHDIR
};

template<typename T>
struct Drag {
	vec<T> offset, elemPos;
	MouseDragStatus mouseAction;
	MouseDragDirection dragDir;

	Drag() {
		stopDragging();
	}

	void startDragging(int x, int y, T elemX, T elemY, MouseDragDirection dir = MouseDragDirection::BOTHDIR) {
		offset.reset(x, y);
		elemPos.reset(elemX, elemY);
		mouseAction = MouseDragStatus::STRATED;
		dragDir = dir;
	}
	bool drag(int x, int y, T& newX, T& newY) {
		if (mouseAction == MouseDragStatus::STOPPED) return false;

		switch (dragDir) {
		case MouseDragDirection::BOTHDIR:
			newX = (x - offset.x) + elemPos.x;
			newY = (y - offset.y) + elemPos.y;
			break;
		case MouseDragDirection::HORIZONTALLY:
			newX = (x - offset.x) + elemPos.x;
			newY = elemPos.y;
			break;
		case MouseDragDirection::VERTICALLY:
			newX = elemPos.x;
			newY = (y - offset.y) + elemPos.y;
			break;
		}

		return true;
	}

	void stopDragging() {
		offset.reset(0, 0);
		elemPos.reset(0, 0);
		mouseAction = MouseDragStatus::STOPPED;
	}
};


class OUI_API Event {
public:
	static constexpr size_t None = 0;
	static constexpr size_t Focus = 1;
	static constexpr size_t Blur = 2;
	static constexpr size_t Select = 3;
	static constexpr size_t Deselect = 4;
	static constexpr size_t Update = 5;
	static constexpr size_t Scroll = 6;
	static constexpr size_t Click = 7;
	static constexpr size_t Destroy = 8;
	static constexpr size_t PostSelect = 9;
	static constexpr size_t _last = PostSelect + 1;
};

class OUI_API DialogButtonId {
public:
	static constexpr size_t OK = 0;
	static constexpr size_t Cancel = 1;
	static constexpr size_t Yes = 2;
	static constexpr size_t No = 3;
	static constexpr size_t _last = 4;
};

class OUI;
class Menu;
class UIX;
class UIScroll;
class UIWindow;
typedef std::vector<OUI*> aoui;

enum class OUI_API Overflow {
	scroll,
	visible,
	hidden,
	initial,
	inherit
};

class InitialValues {
public:
	OUI_API static int fontSize, titleBarHeight;
	OUI_API static double lineHeight;
	OUI_API static std::wstring fontName;
};

class OUITheme {
public:
	OUI_API static Color primary, border, borderActive, textSelection, text,
		scroll, windowBorder, windowShadow, windowTitleBar;
	OUI_API static int activeBorderWidth;
};

enum class OUI_API UIFloatType {
	Left,
	Right,
	Fill,
	Top,
	Bottom
};

struct OUI_API BoxShadow {
public:
	int offsetX, offsetY, blur, spread;
	bool inset;
	Color color;
	Sheet sheet;

	BoxShadow();
	void reset();
};

struct OUI_API BoxModel {
	int left, right, top, bottom, width, height;
	void reset();
};

enum class OUI_API MenuType {
	Solid,
	Fluid
};

enum class OUI_API MenuActivationMode {
	PointerHover,
	PointerDown,
	PointerUp
};

enum class OUI_API Align {
	BASELINE,
	CENTER,
	RIGHT,
	LEFT,
	TOP,
	BOTTOM
};

typedef agg::svg::path_renderer SVGShape;
typedef SVGShape* PSVGShape;

struct OUI_API Point {
	int x, y;
	Point();
	Point(int x, int y);
	void set(int x, int y);
	void shift(int dx, int dy);
};

enum class OUI_API LineStyle {
	SOLID,
	DASH_5x5
};

struct OUI_API Art {
	int thickness, shiftPatternX, shiftPatternY;
	Color fillColor, strokeColor, textColor;
	LineStyle lineStyle;
	Align alignX, alignY;
	Art();
};

struct OUI_API ColoredSpan {
	Color color;
	size_t start, length;
	ColoredSpan(Color color, int start, int length);
};

struct OUI_API Size {
	int width, height;
};
