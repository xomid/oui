#include "UIRangeSlide.h"

UIRangeSlide::UIRangeSlide()
	: boundRange("0.", "na", "0.", "1.") {
	set_value(0, 0);
	set_value(1, .5);
	set_value(2, 10);
}

void UIRangeSlide::on_init() {
	UIDoubleSlide::on_init();
	fill3.set(fill);
	rcHandle3.set(0, 0, handleWidth, handleWidth);
	set_handle_color(1, Color("#888"));
	set_handle_color(2, Color("#eee"));
}

void UIRangeSlide::set_handle_color(int handleId, Color fillColor) {
	if (handleId == 0) fill.set(fillColor);
	else if (handleId == 1) fill2.set(fillColor);
	else fill3.set(fillColor);
}

void UIRangeSlide::on_update() {
	UIDoubleSlide::on_update();
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

	cx = contentArea.left + rcHandle3.width / 2.0 + rcHandle3.left;
	cy = contentArea.top + rcHandle3.height / 2.0 + rcHandle3.top;
	c = stroke;
	hover = chosenHandle == &rcHandle3;
	if (bPressed && hover)
		c = c.bright(-20);
	else if (hover)
		c = c.bright(20);
	canvas.draw_circle(cx, cy, rad, strokeWidth, 2, back, c, fill2);

	cx = contentArea.left + rcHandle2.width / 2.0 + rcHandle2.left;
	cy = contentArea.top + rcHandle2.height / 2.0 + rcHandle2.top;
	c = stroke;
	hover = chosenHandle == &rcHandle2;
	if (bPressed && hover)
		c = c.bright(-20);
	else if (hover)
		c = c.bright(20);
	canvas.draw_circle(cx, cy, rad, strokeWidth, 2, back, c, fill3);
}

void* UIRangeSlide::get_handle(int x, int y, uint32_t flags) {
	if (dir == SlideDirection::HORIZONTAL) {
		if (x <= rcHandle.left + rcHandle.width)
			return &rcHandle;
		if (x >= rcHandle2.left)
			return &rcHandle2;
		return &rcHandle3;
	}
	return 0;
}

double UIRangeSlide::calc_value(void* handle, int x, int y, uint32_t flags) {
	double t = 0;
	if (dir == SlideDirection::HORIZONTAL) {
		if (handle != &rcHandle3) {
			double v = double(CLAMP3(minPos, x - (rcHandle.width >> 1), maxPos));
			v = v / double(maxPos - minPos);

			if (handle == &rcHandle) {
				Float maxT = (range2.value - range2.minValue) / (range2.maxValue - range2.minValue);
				return fmin(v, atof(maxT.str().c_str()));
			}
			else {
				Float minT = (range.value - range.minValue) / (range.maxValue - range.minValue);
				return fmax(v, atof(minT.str().c_str()));
			}
		}
		return double(CLAMP3(rcHandle.left, x - (rcHandle3.width >> 1), rcHandle2.left) - rcHandle.left)
			/ double(rcHandle2.left - rcHandle.left);
	}
	return t;
}

void UIRangeSlide::calc_handle_pos(void* handle, double t) {
	if (handle) {
		int left, top;
		auto rc = (Rect*)handle;

		if (dir == SlideDirection::HORIZONTAL) {
			top = (contentArea.height - handleWidth) >> 1;
			minPos = 0;
			maxPos = contentArea.width - handleWidth;
			left = t * (maxPos - minPos);

			if (rc != &rcHandle3) {
				if (rc == &rcHandle)
					left = CLAMP3(minPos, left, rcHandle2.left);
				else
					left = CLAMP3(rcHandle.left, left, maxPos);
			}
			else {
				Float res = (boundRange.value - boundRange.minValue)
					/ (boundRange.maxValue - boundRange.minValue);

				left = t * (rcHandle2.left - rcHandle.left) + rcHandle.left;
				left = CLAMP3(rcHandle.left, left, rcHandle2.left);
				t = fmin(fmax(t, 0.), 1.);
				boundRange.value = t * (boundRange.maxValue - boundRange.minValue) + boundRange.minValue;
				//boundRange.correct();

				mid = atof(res.str().c_str());
			}
		}
		else {
			left = top = 0;
		}

		rc->set(left, top, handleWidth, handleWidth);
		if (rc != &rcHandle3)
			rcHandle3.left = mid * (rcHandle2.left - rcHandle.left) + rcHandle.left;
	}
}

void UIRangeSlide::on_range_update() {
	UIDoubleSlide::on_range_update();

	UISlide::calc_handle_pos(range, &rcHandle);
	UISlide::calc_handle_pos(range2, &rcHandle2);
	UISlide::calc_handle_pos(boundRange, &rcHandle3);
}

RangedFloat& UIRangeSlide::get_range(void* handle) {
	if (handle == &rcHandle3) {
		return boundRange;
	}
	if (handle == &rcHandle) {
		return range;
	}
	return range2;
}

void UIRangeSlide::set_bound_range(double low, double high, double step) {
	if (!isnan(low) && !isnan(high))
	{
		boundRange.set_min(low);
		boundRange.set_max(high);
		boundRange.set_step(step);
		on_range_update();
	}
}

void UIRangeSlide::set_bound_range(std::string low, std::string high, std::string step) {
	boundRange.set_min(low);
	boundRange.set_max(high);
	boundRange.set_step(step);
	on_range_update();
}


void UIRangeSlide::set_value(int handleId, double value) {
	if (handleId == 0)
		range.set_value(value);
	else if (handleId == 1)
		boundRange.set_value(value);
	else
		range2.set_value(value);

	on_range_update();
}

double UIRangeSlide::get_value(int handleId) const {
	if (handleId == 0)
		return atof(range.value.str().c_str());
	if (handleId == 1)
		return atof(boundRange.value.str().c_str());
	return atof(range2.value.str().c_str());
}

void UIRangeSlide::alert_parent(void* handle) {
	if (parent)
		parent->process_event(this, Event::Update, handle == &rcHandle ? 0 :
			handle == &rcHandle3 ? 1 : 2, true);
}