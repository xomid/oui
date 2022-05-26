#include "oui_visual.h"

int InitialValues::fontSize = 14;
double InitialValues::lineHeight = 1.5;
std::wstring InitialValues::fontName = L"initial";
int InitialValues::titleBarHeight = 32;

Color OUITheme::primary("#1e2026");
Color OUITheme::border(67, 67, 67);
Color OUITheme::borderActive(0xf, 0xa6, 0xff);
Color OUITheme::textSelection(0xf, 0xa6, 0xff);
Color OUITheme::text(0xaf, 0xaf, 0xaf);
Color OUITheme::scroll(0xaf, 0xaf, 0xaf);
int OUITheme::activeBorderWidth = 2;
Color OUITheme::windowBorder = Color(0x1f, 0x1f, 0x1f, 40);
Color OUITheme::windowShadow = Color(0, 0, 0, 60);
Color OUITheme::windowTitleBar = Color(0xff, 0xff, 0);

BoxShadow::BoxShadow() {
	reset();
}

void BoxShadow::reset() {
	inset = false;
	color.set(0, 0, 0, 0);
	offsetX = offsetY = blur = spread = 0;
}

void BoxModel::reset() {
	left = top = right = bottom = width = height = 0;
}

Point::Point()
	: x(0), y(0)
{
}

Point::Point(int x, int y)
	: x(x), y(y)
{
}

void Point::set(int x, int y) {
	this->x = x;
	this->y = y;
}

void Point::shift(int dx, int dy) {
	x += dx;
	y += dy;
}

Art::Art() {
	thickness = 12;
	lineStyle = LineStyle::SOLID;
	alignX = alignY = Align::LEFT;
	shiftPatternX = shiftPatternY = 0;
}

ColoredSpan::ColoredSpan(Color color, int start, int length)
	: color(color), start(start), length(length) {

}
