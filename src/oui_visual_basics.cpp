#include "oui_visual_basics.h"

Rect::Rect() {
	left = top = width = height = 0;
	offset.reset(0, 0);
}

Rect::Rect(int x, int y, int w, int h) {
	set(x, y, w, h);
}

void Rect::shift(int shiftX, int shiftY) {
	left += shiftX;
	top += shiftY;
	offset.reset(left, top);
}

void Rect::set(const Rect& rect) {
	left = rect.left;
	top = rect.top;
	width = rect.width;
	height = rect.height;
	offset.reset(left, top);
}

void Rect::set(int x, int y, int w, int h) {
	left = x;
	top = y;
	width = w;
	height = h;
	offset.reset(left, top);
}

bool Rect::is_inside(int x, int y) {
	return (x >= left && x < left + width) && (y >= top && y < top + height);
}

bool Rect::compare(int l, int t, int cx, int cy) {
	return (left == l && top == t && width == cx && height == cy);
}

void Rect::push(int x, int y, int w, int h) {
	stack.push_back(*this);
	set(x, y, w, h);
}

void Rect::pop() {
	if (stack.size()) {
		set(stack.back());
		stack.pop_back();
	}
}

void Rect::shrink(Spacing& spacing) {
	left += spacing.left;
	top += spacing.top;
	width -= spacing.left + spacing.right;
	height -= spacing.top + spacing.bottom;
}

void Rect::expand(Spacing& spacing) {
	left -= spacing.left;
	top -= spacing.top;
	width += spacing.left + spacing.right;
	height += spacing.top + spacing.bottom;
}

int Rect::right() {
	return left + width;
}

int Rect::bottom() {
	return top + height;
}



Spacing::Spacing() {
	reset();
}

void Spacing::reset() {
	top = right = bottom = left = 0;
}

bool Spacing::is_empty() {
	return top <= 0 && left <= 0 && right <= 0 && bottom <= 0;
}

void Spacing::set(int all) {
	this->top = all;
	this->right = all;
	this->bottom = all;
	this->left = all;
}

void Spacing::set(int leftRight, int topBottom) {
	this->top = topBottom;
	this->right = leftRight;
	this->bottom = topBottom;
	this->left = leftRight;
}

void Spacing::set(int top, int leftRight, int bottom) {
	this->top = top;
	this->right = leftRight;
	this->bottom = bottom;
	this->left = leftRight;
}

void Spacing::set(int top, int right, int bottom, int left) {
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->left = left;
}

bool BorderRadius::is_empty() {
	return ocom::is_zerod(lt) && ocom::is_zerod(lt) && ocom::is_zerod(lt) && ocom::is_zerod(lt);
}

void BorderRadius::reset() {
	lt = rt = rb = lb = 0.0;
}

void BorderRadius::set(double lt, double rt, double rb, double lb) {
	this->lt = lt;
	this->rt = rt;
	this->rb = rb;
	this->lb = lb;
}

void BorderRadius::set(int topLeft, int topRightBottomLeft, int bottomRight) {
	this->lt = topLeft;
	this->rt = topRightBottomLeft;
	this->lb = topRightBottomLeft;
	this->rb = bottomRight;
}

void BorderRadius::set(int all) {
	this->lt = all;
	this->rt = all;
	this->rb = all;
	this->lb = all;
}

void BorderRadius::set(int topLeftBottomRight, int topRightBottomLeft) {
	this->lt = topLeftBottomRight;
	this->rb = topLeftBottomRight;
	this->rt = topRightBottomLeft;
	this->lb = topRightBottomLeft;
}

Border::Border() {
	reset();
}

void Border::set(int width, Color color) {
	set_top(width, color);
	set_right(width, color);
	set_bottom(width, color);
	set_left(width, color);
}

void Border::set(int leftRight, int topBottom, Color leftRightColor, Color topBottomColor) {
	set_top(topBottom, topBottomColor);
	set_right(leftRight, leftRightColor);
	set_bottom(topBottom, topBottomColor);
	set_left(leftRight, leftRightColor);
}

void Border::set(int top, int leftRight, int bottom, Color topColor, Color leftRightColor, Color bottomColor) {
	set_top(top, topColor);
	set_right(leftRight, leftRightColor);
	set_bottom(bottom, bottomColor);
	set_left(leftRight, leftRightColor);
}

void Border::set(int top, int right, int bottom, int left, Color topColor, Color rightColor, Color bottomColor, Color leftColor) {
	set_top(top, topColor);
	set_right(right, rightColor);
	set_bottom(bottom, bottomColor);
	set_left(left, leftColor);
}

void Border::set_top(int top, Color color) {
	this->top = top;
	this->topColor = color;
}

void Border::set_right(int right, Color color) {
	this->right = right;
	this->rightColor = color;
}

void Border::set_bottom(int bottom, Color color) {
	this->bottom = bottom;
	this->botColor = color;
}

void Border::set_left(int left, Color color) {
	this->left = left;
	this->leftColor = color;
}

void Border::set_top_color(Color color) {
	this->topColor = color;
}

void Border::set_right_color(Color color) {
	this->rightColor = color;
}

void Border::set_bottom_color(Color color) {
	this->botColor = color;
}

void Border::set_left_color(Color color) {
	this->leftColor = color;
}

void Border::set_color(Color all) {
	this->topColor = all;
	this->rightColor = all;
	this->botColor = all;
	this->leftColor = all;
}
