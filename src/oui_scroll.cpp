#include "oui_scroll.h"
#include "oui_uix.h"

UIScroll::UIScroll(): UIButton() {
	min = viewport = page = handlePos = lastHandlePos = lastX = lastY = handleLength = 0;
	mode = ScrollMode::Vertical;
	scrollX = NULL;
	scrollY = NULL;
	bVisible = false;
}

OUI* UIScroll::create(int left, int top, int width, int height, OUI* parent, bool bAddToParent) {
	if (parent) {
		this->parent = parent;
		uix = parent->uix;
		godSheet = parent->godSheet;
		//uix->add_element(this);
	}

	set_background_color(Color("#999999"));

	//normalBackgroundColor.inherit();
	//normalColor.set(OUITheme::scroll);
	//hoverColor.brightness(&normalColor, +25);
	//downColor.brightness(&normalColor, -25);
	//change_curr_color();
	boxModel.left = left;
	boxModel.top = top;
	boxModel.width = width;
	boxModel.height = height;
	set_page(1, 1);
	canvas.set(&area, godSheet);
	move(left, top, width, height);
	return this;
}

void UIScroll::on_update_frame() {
	if (uix == NULL) return;
	if (bInvalidated) {
		uix->lock_screen(id);
		on_update();
		uix->unlock_screen(id);
		uix->update_section(&area);
		bInvalidated = false;
	}
}

void UIScroll::on_update() {
	if (parent) {
		backgroundColor.save();
		backgroundColor.set(parent->backgroundColor);
	}
	UILabel::on_update();
	backgroundColor.restore();
	canvas.render(handle.ras, handle.sl, isPressed ? downBackColor : isHover ? hoverBackColor : backgroundColor);
}

void UIScroll::calc_shape() {
	OUI::calc_shape();
}

void UIScroll::calc_handle() {
	BorderRadius rad;
	rad.lt = rad.lb = rad.rt = rad.rb = max(max(handleArea.width, handleArea.height) / 2, 0);
	handle.rounded_rect(handleArea.left, handleArea.top, 
		CLAMP3(0, handleArea.width, handleArea.width), 
		CLAMP3(0, handleArea.height, handleArea.height), rad);
}

void UIScroll::on_resize(int width, int height) {
	UIButton::on_resize(width, height);
	int total = get_total();
	handleLength = CLAMP3(40, int((double)viewport / page * total), total);
	calc_handle();
}

void UIScroll::set_page(int page, int viewport) {
	this->page = page;
	this->viewport = CLAMP3(0, viewport, page);
	int total = get_total();
	handleLength = page > 0 ? CLAMP3(40, int((double)viewport / page * total), total) : total;
	set_handle_pos(CLAMP3(0, handlePos, total - handleLength));
	show_window(viewport < page);
}

void UIScroll::on_mouse_move(int x, int y, uint32_t param) {
	//if (!bVisible) return;
	UIButton::on_mouse_move(x, y, param);

	if (isPressed) {
		int newPos;

		newPos = 0;
		if (mode == ScrollMode::Vertical) {
			newPos = lastHandlePos + y - lastY;
			newPos = CLAMP3(0, newPos, contentArea.height - handleLength);
		}
		else {
			newPos = lastHandlePos + x - lastX;
			newPos = CLAMP3(0, newPos, contentArea.width - handleLength);
		}

		if (parent) {
			parent->process_event(this, Event::Scroll,
				TOUINT2((mode == ScrollMode::Vertical ? 1 : 0), newPos), false);
		}
	}
}

void UIScroll::set_handle_pos(int pos) {
	calc_handle_pos(pos);
	int d = get_total() - handleLength;
	if (!d) page_pos = 0;
	else page_pos = (int)round((double)handlePos / double(d) * (page - viewport));
}

void UIScroll::calc_handle_pos(int pos) {
	int total = get_total();
	handleLength = page > 0 ? CLAMP3(40, int((double)viewport / page * total), total) : total;
	if (mode == ScrollMode::Vertical)
		handlePos = CLAMP3(0, pos, contentArea.height - handleLength);
	else
		handlePos = CLAMP3(0, pos, contentArea.width - handleLength);
	if (mode == ScrollMode::Vertical)
		handleArea.set(contentArea.left, contentArea.top + handlePos, contentArea.width, handleLength);
	else
		handleArea.set(contentArea.left + handlePos, contentArea.top, handleLength, contentArea.height);
	calc_handle();
	invalidate();
}

void UIScroll::set_pos(int pos) {
	if (!bVisible) return;
	int total = get_total();
	int d = (total - handleLength);
	int v = (page - viewport);
	if (d == 0 || v == 0) return;
	page_pos = pos;
	pos = (int)round((double)page_pos / double(v) * double(d));
	calc_handle_pos(pos);
}

int UIScroll::get_pos() const {
	if (!bVisible) return 0;
	return page_pos;
}

int UIScroll::get_total() const {
	return max((mode == ScrollMode::Vertical ? contentArea.height : contentArea.width), 0);
}

void UIScroll::on_mouse_down(int x, int y, uint32_t param) {
	UIButton::on_mouse_down(x, y, param);
	if (isPressed) {
		lastX = x;
		lastY = y;
		lastHandlePos = handlePos;
	}
}

void UIScroll::on_click(int x, int y, uint32_t flags) {
}

void UIScroll::scroll_rel(int delta) {
	delta *= Max(viewport / 2, 20);
	delta = int(((double)delta / page) * get_total());
	invalidate();
	if (parent)
		parent->process_event(this, Event::Scroll,
			TOUINT2((mode == ScrollMode::Vertical ? 1 : 0), handlePos + delta), false);
}