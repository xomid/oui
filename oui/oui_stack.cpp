#include <algorithm> 
#include <cctype>
#include <locale>
#include "oui_stack.h"
#include "oui_label.h"
#include "oui_scroll.h"

UIStack::UIStack(): OUI() {
	mode = UIStackMode::STACKHOR;
	bScrollable = true;
	bStretch = false;
	overflow = Overflow::hidden;
}

void UIStack::set_direction(UIStackMode mode) {
	this->mode = mode;
	reset_size();
}

void UIStack::stretch() {
	bStretch = true;
	reset_size();
}

void UIStack::on_update() {
	OUI::on_update();
}

void UIStack::measure_size(int* width, int* height) {
	OUI::measure_size(width, height);
	return;

	/*int cw = 0, ch = 0;
	measure_content(&cw, &ch);*/

	/*if (scrollX && scrollX->bVisible) {
		r = scrollX->area.left + scrollX->area.width - contentArea.left;
		b = scrollX->area.top + scrollX->area.height - contentArea.top;
		cw = Max(r, cw);
		ch = Max(b, ch);
	}
	if (scrollY && scrollY->bVisible) {
		r = scrollY->area.left + scrollY->area.width - contentArea.left;
		b = scrollY->area.top + scrollY->area.height - contentArea.top;
		cw = Max(r, cw);
		ch = Max(b, ch);
	}*/

	/*if (boxModel.widthUnit == Unit::WRAP && w) *w = cw +
		(boxModel.padding.left + boxModel.padding.right + boxModel.border.left + boxModel.border.right);
	if (boxModel.heightUnit == Unit::WRAP && h) *h = ch +
		(boxModel.padding.top + boxModel.padding.bottom + boxModel.border.top + boxModel.border.bottom);*/
}

void UIStack::measure_content(int* w, int* h) {
	int l, t, cw, ch, mxw = -1, mxh = -1, top = 0;
	l = t = 0;

	Rect rc;
	OUI::get_content_area(rc);

	if (mode == UIStackMode::STACKHOR) {
		iterateV(elements) {
			auto elem = *it;
			if (!elem->bVisible)
				continue;
			cw = ch = 0;
			elem->measure_size(&cw, &ch);
			if (bStretch) ch = rc.height;

			if (l + cw + elem->margin.left +
				elem->margin.right > rc.width) {
				top = t;
				l = 0;
				t = 0;
			}

			l += cw + elem->margin.left +
				elem->margin.right;
			t = Max(t + top, ch + top);
			mxw = Max(l, mxw);
		}

		l = mxw;
	}
	else {
		iterateV(elements) {
			auto elem = *it;
			if (!elem->bVisible)
				continue;
			cw = ch = 0;
			elem->measure_size(&cw, &ch);
			if (bStretch) cw = rc.width;
			t += ch + elem->margin.top +
				elem->margin.bottom;
			l = Max(l, cw);
		}
	}

	if (w) *w = l;
	if (h) *h = t;
}

void UIStack::on_resize(int width, int height) {
	int cw, ch, l, t, i, top, left, r, b, h, w;
	OUI* elem;
	Rect rc;

	OUI::on_resize(width, height);
	OUI::get_content_area(rc);
	measure_content(&cw, &ch);
	
	if (rc.height > 1000) {
		OUI::get_content_area(rc);
		measure_content(&cw, &ch);
	}

	if (ch > rc.height) {
		if (scrollY) scrollY->move(rc.width - 10, 0, 10, rc.height);
	}

	if (scrollY) scrollY->set_page(ch, rc.height);

	get_content_area(rc);

	l = 0;
	t = scrollY ? -scrollY->get_pos() : 0;
	top = 0;
	left = 0;
	r = rc.width;
	b = rc.height;

	auto sz = elements.size();
	if (sz == 0) return;

	if (mode == UIStackMode::STACKHOR) {
		t = top;

		for (i = 0; i < sz; ++i) {
			elem = elements[i];
			if (!elem->bVisible)
				continue;

			elem->measure_size(&w, &h);

			if (l + elem->margin.left + elem->margin.right + w > rc.width) {
				l = 0;
				t += elem->margin.top + elem->margin.bottom + h;
			}

			if (elem->floatType == UIFloatType::Left) {
				l += elem->margin.left;
				elem->move(l, t + margin.top, w, h);
				l += w + elem->margin.right;
			}
		}

		for (i = (int)elements.size() - 1; i > -1; --i) {
			elem = elements[i];
			if (!elem->bVisible)
				continue;

			t = top + elem->margin.top;
			elem->measure_size(&w, &h);

			if (elem->floatType == UIFloatType::Right) {
				r -= elem->margin.right;
				elem->move(r - w, t, w, h);
				r -= w + elem->margin.left;
			}
		}

		iterateV(elements) {
			auto elem = *it;
			if (!elem->bVisible)
				continue;

			t = top + elem->margin.top;
			elem->measure_size(&w, &h);

			if (elem->floatType == UIFloatType::Fill) {
				l += elem->margin.left;
				elem->move(l, t, r - l, h);
				break;
			}
		}
	}
	else if (mode == UIStackMode::STACKVER) {
		if (bStretch) cw = rc.width;

		iterateV(elements) {
			auto elem = *it;
			if (!elem->bVisible)
				continue;
			l = left + elem->margin.left;

			elem->measure_size(&w, &h);
			if (bStretch) w = cw;

			if (elem->floatType == UIFloatType::Bottom) {
				b -= elem->margin.bottom;
				elem->move(r - w, t, w, h);
				b -= w + elem->margin.bottom;
			}
			else {
				t += elem->margin.top;
				elem->move(l, t, w, h);
				t += h + elem->margin.bottom;
			}
		}
	}

	invalidate();
}	

void UIStack::move_page(int left, int top) {
	int l, t, r, b, h, w;
	Rect rc;
	get_content_area(rc);
	OUI::move_page(left, top);

	l = left;
	t = scrollY ? -scrollY->get_pos() : 0;
	top = 0;
	left = 0;
	r = rc.width;
	b = rc.height;
	if (elements.size() == 0) return;

	if (mode == UIStackMode::STACKHOR) {
		
	}
	else if (mode == UIStackMode::STACKVER) {
		iterateV(elements) {
			auto elem = *it;
			if (!elem->bVisible)
				continue;
			l = left + elem->margin.left;

			w = elem->boxModel.width;
			h = elem->boxModel.height;

			if (elem->floatType == UIFloatType::Bottom) {
				b -= elem->margin.bottom;
				elem->move(r - w, t);
				b -= w + elem->margin.bottom;
			}
			else {
				t += elem->margin.top;
				elem->move(l, t);
				t += h + elem->margin.bottom;
			}
		}
	}

	invalidate();
}

void UIStack::get_content_area(Rect& rc) {
	OUI::get_content_area(rc);
	if (scrollY && scrollY->bVisible)
		rc.width = Max(rc.width - 10, 0);
}