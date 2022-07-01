#include "oui_window.h"
#include "oui.h"
#include "oui_uix.h"
#include "oui_label.h"

void draw_X_icon(Canvas& canvas, Rect& rect, Color color, double width, byte opacity) {
	pixfmt_bgr pf(canvas.sheet, canvas.area);
	base_ren_type r(pf);
	agg::line_profile_aa profile;
	renderer_type renInd(r, profile);
	rasterizer_type rasInd(renInd);
	renInd.color(color.to_rgba8());
	profile.width(width);

	int sx = (rect.width - rect.height) / 2;
	rasInd.move_to_d(rect.left + sx, rect.top);
	rasInd.line_to_d((double)rect.left + sx + rect.height, (double)rect.top + rect.height);
	rasInd.render(false);
	rasInd.move_to_d(rect.left + sx + rect.height, rect.top);
	rasInd.line_to_d(rect.left + sx, (double)rect.top + rect.height);
	rasInd.render(false);
}

void UIWindow::apply_theme(bool bInvalidate) {
	set_background_color(OUITheme::primary);
	set_color(OUITheme::text);
	btnCloseColor = backgroundColor.get_contrast_color();
	crHover.brightness(&backgroundColor, -20);
	crDown.brightness(&backgroundColor, -40);
}

OUI* UIWindow::create(int left, int top, int width, int height, OUI* caller, WindowType type) {
	border.set(1, OUITheme::windowBorder);
	icw = int(InitialValues::titleBarHeight * 1.4), ich = InitialValues::titleBarHeight;
	int brd = 8;
	borderRadius.set(brd, brd, brd, brd);
	add_box_shadow(false, 0, 7, 12, 0, OUITheme::windowShadow);
	svgApp = parse_svg(R"(<svg width="48" height="48"><path fill="#1976d2" d="M6 6h17v17H6V6zM25.042 22.958V6H42v16.958H25.042zM6 25h17v17H6V25zM25 42V25h17v17H25z"/></svg>)");
	bDraggable = true;
	btnCloseHover = btnCloseDown = btnMaxHover = btnMaxDown = btnMinHover = btnMinDown = false;
	uix = caller->uix;
	parent = caller;
	godSheet = caller->godSheet;
	width = Max(width, 4 * icw);
	OUI::create(left, top, width, height, NULL, false);
	bVisible = false;
	this->type = type;
	this->caller = caller;
	uix->add_window(this);

	return this;
}

void UIWindow::get_content_area(Rect& rc) {
	OUI::get_content_area(rc);
	rc.height -= InitialValues::titleBarHeight;
}

void UIWindow::get_abs_content_area(Rect& rc) {
	rc.left = contentArea.left;
	rc.top = contentArea.top + InitialValues::titleBarHeight;
	rc.width = contentArea.width;
	rc.height = contentArea.height - InitialValues::titleBarHeight;
	rc.shift(area.left, area.top);
}

void UIWindow::on_update() {
	OUI::on_update();
	int L = 1, T = 1, w = area.width - 2, h = InitialValues::titleBarHeight;
	Rect rc(L, T, w, h);
	Spacing pad;
	Rect rcc;
	Border bord;
	Color cr2 = btnCloseColor;

	pad.set(11);
	rcc.set(L + w - icw, T, icw, ich);
	
	if (!btnCloseHover && btnCloseDown)
		int t = 0;

	if ((btnCloseHover && !btnCloseDown) || (btnCloseDown && btnCloseHover)) {
		canvas.sheet->setclip(&absContentShape, 0xff);
		Color crCloseDown, crCloseHover(0xff, 0, 0);
		crCloseDown.brightness(&crCloseHover, +100);
		canvas.clear(&rcc, (btnCloseHover && !btnCloseDown) ? &crCloseHover : &crCloseDown);
		canvas.sheet->unclip();
	}

	rcc.shrink(pad);
	draw_X_icon(canvas, rcc, ((btnCloseHover && !btnCloseDown) || (btnCloseDown && btnCloseHover)) ?
		Color(0xff, 0xff, 0xff) : cr2, btnCloseHover && !btnCloseDown ? 1 : .8, opacity);

	if (type != WindowType::Dialog) {
		canvas.art.strokeColor = cr2;
		if ((btnMaxHover && !btnMaxDown) || (btnMaxDown && btnMaxHover)) {
			rcc.set(L + w - 2 * icw, T, icw, ich);
			canvas.clear(&rcc, btnMaxDown && btnMaxHover ? &crDown : &crHover);
		}
		bord.set(1, cr2);
		rcc.set(L + w - 2 * icw + (icw - ich) / 2, T, ich, ich);
		rcc.shrink(pad);
		canvas.draw_box(rcc, bord, opacity);

		rcc.set(L + w - 3 * icw, T, icw, ich);
		if ((btnMinHover && !btnMinDown) || (btnMinDown && btnMinHover))
			canvas.clear(&rcc, btnMinDown && btnMinHover ? &crDown : &crHover);
		int minBW = int(.3 * ich);
		int l = (rcc.left + (icw - minBW) / 2);
		canvas.draw_horizontal_line(T + h / 2, l, l + minBW, 1);

		pad.set(8);
		canvas.render_svg(svgApp, L, T, icw, ich, opacity, &pad);

		rcc.set(L + ich, T, w - 4 * icw, h);
	}
	else rcc.set(L + 16, T, w - 4 * icw, h);

	canvas.art.alignX = Align::LEFT;
	canvas.art.alignY = Align::CENTER;
	canvas.set_color(cr2);
	canvas.draw_text16(title, rcc);
}

void UIWindow::on_mouse_move(int x, int y, uint32_t flags) {
	int L = 1, T = 1, w = area.width - 2, h = InitialValues::titleBarHeight;
	Rect rcClose(L + w - icw, T, icw, ich);
	btnCloseHover = rcClose.is_inside(x, y);
	Rect rcMax(L + w - 2 * icw, T, icw, ich);
	btnMaxHover = rcMax.is_inside(x, y);
	Rect rcMin(L + w - 3 * icw, T, icw, ich);
	btnMinHover = rcMin.is_inside(x, y);
	invalidate();
	OUI::on_mouse_move(x, y, flags);
}

void UIWindow::on_mouse_down(int x, int y, uint32_t flags) {
	btnCloseDown = btnCloseHover;
	btnMaxDown = btnMaxHover;
	btnMinDown = btnMinHover;
	invalidate();
	OUI::on_mouse_down(x, y, flags);
}

void UIWindow::on_mouse_up(int x, int y, uint32_t flags) {
	if (caller) {
		if (btnCloseDown && btnCloseHover) {
			close(DialogButtonId::Cancel);
		}
	}
	btnCloseDown = false;
	btnMaxDown = false;
	btnMinDown = false;
	invalidate();
	OUI::on_mouse_up(x, y, flags);
}

void UIWindow::on_dbl_click(int x, int y, uint32_t flags) {
	on_mouse_down(x, y, flags);
	OUI::on_dbl_click(x, y, flags);
}

OUI* UIWindow::get_draggable(int x, int y, uint32_t flags) {
	if (type == WindowType::Dialog)
		return (y < InitialValues::titleBarHeight) && x < (area.width - 1 - icw) ? this : 0;
	return (y < InitialValues::titleBarHeight) && x < (area.width - 1 - 3 * icw) ? this : 0;
}

void UIWindow::on_mouse_leave(OUI* next) {
	OUI::on_mouse_leave(next);
	btnCloseHover = false;
	btnMaxHover = false;
	btnMinHover = false;
	invalidate();
}

void UIWindow::close(uint32_t wmsg) {
	show_window(false);
	if (caller) caller->on_window_closed(this, wmsg);
}

void UIWindow::show_window(bool show) {
	if (!uix) return;
	uix->show_window(this, show);
	if (type == WindowType::Dialog) {
		bVisible = uix->pop_up_dialog(this) ? show : false;
	}
}

bool UIWindow::is_child_visible(OUI* child) {
	return bVisible;//OUI::is_child_visible(child);
}

void UIWindow::set_title(std::wstring newTitle) {
	title = newTitle;
	invalidate();
}

void UIDialog::process_event(OUI* element, uint32_t message, uint64_t param, bool bubbleUp) {
	if (message == Event::Click) {
		if (buttonIds.count((UIButton*)element)) {
			close(buttonIds[(UIButton*)element]);
			return;
		}
	}
	OUI::process_event(element, message, param, bubbleUp);
}

OUI* UIDialog::create(int width, int height, OUI* caller, size_t buttonCount) {
	height += InitialValues::titleBarHeight;
	int brd = 4, fontSize = 15, w = 100, h = 34;
	int l, t, mar = 15, midmar = 8;
	int totalW = int(buttonCount) * w + 2 * mar + Max(int(buttonCount) - 1, 0) * midmar;
	height = Max(height, 2 * InitialValues::titleBarHeight + 2 * mar);

	height = Max(height, 2 * InitialValues::titleBarHeight + 2 * mar);
	width = Max(totalW, width);
	UIWindow::create(0, 0, width, height, caller, WindowType::Dialog);
	set_background_color(Color("#2c2c2c"));

	Color normalBack("#393939"), normalColor(0xff, 0xff, 0xff);
	l = t = 0;
	w = h = 10;

	iterateI(buttonCount) {
		UIButton* btn = new UIButton();
		btn->borderRadius.set(brd, brd, brd, brd);
		btn->create(l, t, w, h, this);
		btn->set_text(L"Unset");
		btn->set_background_color(normalBack);
		btn->set_color(normalColor);
		btn->set_font_size(fontSize);
		buttonIds[btn] = 0;
		buttons.push_back(btn);
	}

	return this;
}

void UIDialog::on_resize(int width, int height) {
	int w = 100, h = 34;
	int l, t, midmar = 8;
	auto buttonCount = (int)buttons.size();
	int totalW = int(buttonCount) * w + Max(int(buttonCount) - 1, 0) * midmar + padding.left + padding.right;
	l = contentArea.width - int(buttonCount) * w - (int(buttonCount) - 1) * midmar;
	t = contentArea.height - h;

	iterateV(buttons) {
		auto btn = *it;
		btn->move(l, t, w, h);
		l += w + midmar;
	}
}

OUI* UIDialog::create(int width, int height, OUI* caller, std::initializer_list<ButtonName> list) {
	create(width, height, caller, list.size());
	size_t cur = 0, sz = buttons.size();
	for (auto& x : list) {
		auto* btn = buttons[cur++];
		btn->set_text(x.name);
		buttonIds[btn] = x.id;
	}
	return this;
}

OUI* UIDialog::create(int width, int height, OUI* caller, DialogButtonSet buttonSet) {
	int buttonCount =
		buttonSet == DialogButtonSet::Yes_No_Cancel ? 3 :
		buttonSet == DialogButtonSet::OK ? 1 : 2;

	create(width, height, caller, buttonCount);

	UIButton* ok, * cancel, * yes, * no;
	switch (buttonSet) {
	case DialogButtonSet::OK:
		ok = buttons[0];
		buttonIds[ok] = DialogButtonId::OK;
		ok->set_text(L"OK");
		break;
	case DialogButtonSet::OK_Cancel:
		ok = buttons[0];
		cancel = buttons[1];
		buttonIds[ok] = DialogButtonId::OK;
		buttonIds[cancel] = DialogButtonId::Cancel;
		ok->set_text(L"OK");
		cancel->set_text(L"Cancel");
		break;
	case DialogButtonSet::Yes_No:
		yes = buttons[0];
		no = buttons[1];
		buttonIds[yes] = DialogButtonId::Yes;
		buttonIds[no] = DialogButtonId::No;
		yes->set_text(L"Yes");
		no->set_text(L"No");
		break;
	case DialogButtonSet::Yes_No_Cancel:
		yes = buttons[0];
		no = buttons[1];
		cancel = buttons[2];
		buttonIds[yes] = DialogButtonId::Yes;
		buttonIds[no] = DialogButtonId::No;
		buttonIds[cancel] = DialogButtonId::Cancel;
		yes->set_text(L"Yes");
		no->set_text(L"No");
		cancel->set_text(L"Cancel");
		break;
	}

	return this;
}

void UIDialog::set_default_button(uint32_t buttonId) {
	Color defaultBtnBack("#60cdff"), defaultBtnColor("#1a1a1a");
	auto* button = find(buttonId);
	if (!button) return;
	button->set_background_color(defaultBtnBack);
	button->set_color(defaultBtnColor);
}

UIButton* UIDialog::find(uint32_t buttonId) {
	for (auto it = buttonIds.begin(); it != buttonIds.end(); ++it)
		if (it->second == buttonId)
			return it->first;
	return NULL;
}

UIButton* UIDialog::get_button(uint32_t id) {
	return find(id);
}
