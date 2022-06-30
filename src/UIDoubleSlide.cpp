#include "UIDoubleSlide.h"

UIDoubleSlide::UIDoubleSlide()
	: range2("0", "na", "0", "1.0") {
	range2.set_value(range.maxValue.str());
	on_range_update();
}

void UIDoubleSlide::on_init() {
	UILinearSlide::on_init();
	fill2.set(fill);
	rcHandle2.set(0, 0, handleWidth, handleWidth);
	set_handle_color(0, Color("#111"));
	set_handle_color(1, Color("#eee"));
}

void UIDoubleSlide::set_value(int handleId, double value) {
	if (handleId == 0)
		range.set_value(value);
	else 
		range2.set_value(value);

	on_range_update();
}

double UIDoubleSlide::get_value(int handleId) const {
	if (handleId == 0)
		return atof(range.value.str().c_str());
	return atof(range2.value.str().c_str());
}

void UIDoubleSlide::set_handle_color(int handleId, Color fillColor) {
	if (handleId == 0) fill.set(fillColor);
	else fill2.set(fillColor);
}

void UIDoubleSlide::on_update() {
	OUI::on_update();
	int l, t;
	if (dir == SlideDirection::HORIZONTAL) {
		l = contentArea.left + pad;
		t = contentArea.top + Max(0, (contentArea.height - slideImg.h) >> 1);
	}
	else {
		l = contentArea.left + Max(0, (contentArea.width - slideImg.w) >> 1);
		t = contentArea.top + pad;
	}

	canvas.bit_blt(slideImg, l, t, slideImg.w, slideImg.h, 0, 0, true);

	double cx, cy;
	cx = contentArea.left + rcHandle.width / 2.0 + rcHandle.left;
	cy = contentArea.top + rcHandle.height / 2.0 + rcHandle.top;
	Color c = stroke;
	bool hover = chosenHandle == &rcHandle;
	if (bPressed && hover)
		c = c.bright(-20);
	else if (hover)
		c = c.bright(20);
	canvas.draw_circle(cx, cy, rad, strokeWidth, 2, back, c, fill);


	cx = contentArea.left + rcHandle2.width / 2.0 + rcHandle2.left;
	cy = contentArea.top + rcHandle2.height / 2.0 + rcHandle2.top;
	c = stroke;
	hover = chosenHandle == &rcHandle2;
	if (bPressed && hover)
		c = c.bright(-20);
	else if (hover)
		c = c.bright(20);
	canvas.draw_circle(cx, cy, rad, strokeWidth, 2, back, c, fill2);
}

void* UIDoubleSlide::get_handle(int x, int y, uint32_t flags) {
	if (dir == SlideDirection::HORIZONTAL) {
		if (ABS(x - rcHandle.left - (rcHandle.width >> 1)) < ABS(x - rcHandle2.left - (rcHandle2.width >> 1)))
			return &rcHandle;
		else
			return &rcHandle2;
	}
	return 0;
}

double UIDoubleSlide::calc_value(void* handle, int x, int y, uint32_t flags) {
	if (handle) {
		auto pressedHandleRect = (Rect*)handle;
		if (dir == SlideDirection::HORIZONTAL) {
			double v = double(CLAMP3(minPos, x - (pressedHandleRect->width >> 1), maxPos));
			return v / double(maxPos - minPos);
		}
	}

	return 0.;
}

void UIDoubleSlide::on_range_update() {
	if (range.hasMin) range2.minValue = range.minValue;
	if (range.hasMax) range2.maxValue = range.maxValue;
	if (range.hasStep) range2.stepValue = range.stepValue;
	UISlide::calc_handle_pos(range, &rcHandle);
	UISlide::calc_handle_pos(range2, &rcHandle2);
}

void UIDoubleSlide::alert_parent(void* handle) {
	if (parent) parent->process_event(this, Event::Update, handle == &rcHandle ? 0 : 1, true);
}

RangedFloat& UIDoubleSlide::get_range(void* handle) {
	if (handle == &rcHandle)
		return range;
	return range2;
}