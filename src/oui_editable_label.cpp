#include "oui_editable_label.h"
#include "oui_uix.h"

UIEditableLabel::UIEditableLabel() : UIText() {
	bEditable = false;
}

void UIEditableLabel::set(bool editable) {
	//if (editable) uix->add_focusable(this);
	//else uix->remove_focusable(this);
	bEditable = editable;
}

void UIEditableLabel::on_init() {
	UIText::on_init();
	canvas.art.alignX = defaultAlignX;
}

void UIEditableLabel::blur() {
	set(false);
	UIText::blur();
}

void UIEditableLabel::focus() {
	if (bEditable) UIText::focus();
}

void UIEditableLabel::on_dbl_click(int x, int y, uint32_t flags) {
	set(true);
	UIText::focus();
	UIText::on_dbl_click(x, y, flags);
}

void UIEditableLabel::on_mouse_down(int x, int y, uint32_t flags) {
	if (bEditable)
		UIText::on_mouse_down(x, y, flags);
	else
		UILabel::on_mouse_down(x, y, flags);
}

void UIEditableLabel::on_update() {
	if (bEditable) {
		UIText::on_update();
	}
	else {
		canvas.art.alignX = Align::LEFT;
		UILabel::on_update();
	}
}

bool UIEditableLabel::is_editable() {
	return bEditable;
}

OUI* UIEditableLabel::get_draggable(int x, int y, uint32_t flags) {
	if (!parent || bEditable) return 0;
	x = TORELX(TOABSX(x), parent->area);
	y = TORELY(TOABSY(y), parent->area);
	return parent->get_draggable(x, y, flags);
}

bool UIEditableLabel::pre_append(wchar_t c) {
	return bEditable;
}

bool UIEditableLabel::pre_append(std::wstring str) {
	return bEditable;
}

bool UIEditableLabel::focusable() {
	return isFocusable && is_editable();
}

void UIEditableLabel::on_key_down(uint32_t key, uint32_t nrep, uint32_t flags) {
	UIText::on_key_down(key, nrep, flags);
	switch (key) {
	case VK_RETURN:
		uix->focus_next();
		break;
	}
}