#include "oui_text.h"
#include "oui_uix.h"
#include <precise_float.h>

#pragma region UIText

bool is_key_on(uint32_t key) {
	return ::GetKeyState(key) & 0x8000;
}

UIText::UIText() : UILabel() {
	showPlaceHolder = true;
	showClearIcon = true;
	currIndex = 0;
	bCloseIconDown = false;
	bShowIcon = false;
	bIconDown = false;
	closeIcon = NULL;
	m_path = NULL;
	selectionBeginIndex = 0;
	shiftX = 0;
	shiftY = 0;
	blink = false;
	bFocusable = true;
	blinkerSpeed = 500;
	textW = textH = 0;
	//add_box_shadow(false, 0, 1, 3, 0, Color("#0001"));
	//border.set(1, Color("#6a6d78"));
	set_place_holder_color(OUITheme::text.bright(-40));
	padding.set(4, 4, 4, 8);
	borderRadius.set(4, 4, 4, 4);
	set_direction(Align::LEFT);
}

void UIText::set_text(std::wstring text) {
	OUI::set_text(text);
	//value = srcValue;
	set_index((int)this->text.length());
	reset_selection();
	invalidate();
	blink = false;
}

void UIText::reset_selection() {
	selectionBeginIndex = currIndex;
}

OUI* UIText::get_draggable(int x, int y, uint32_t flags) {
	return bDraggable && !contentArea.is_inside(x, y) ? this : 0;
}

Point UIText::get_cursor_pos(bool absolute) {
	if (absolute) {
		auto res = cursorPos;
		res.shift(area.left, area.top);
		return res;
	}
	return cursorPos;
}

void UIText::focus() {
	UILabel::focus();
	//border.set_color(Color("#2962ff"));

	//auto& s = outsetBoxShadows[0];
	//s.offsetY = 2;
	//s.blur = 7;
	//s.spread = 2;
	//s.color.set("#0002");
	//calc_shape();
	set_index(currIndex);
	blink = true;
	set_timer(0, blinkerSpeed);
	process_event(this, Event::Focus, 0, true);
}

void UIText::blur() {
	UILabel::blur();
	//border.set_color(Color("#6a6d78"));

	/*auto& s = outsetBoxShadows[0];
	s.offsetY = 1;
	s.blur = 3;
	s.spread = 0;
	s.color.set("#0001");
	calc_shape();*/

	reset_selection();
	blink = false;
	kill_timer(0);
	process_event(this, Event::Blur, 0, true);
}

void UIText::on_timer(uint32_t nTimer) {
	if (nTimer == 0) {
		blink = !blink;
		if (blink)
			int t = 0;
		invalidate();
	}
}

void UIText::on_mouse_down(int x, int y, uint32_t flags) {

	get_content_area(contentArea);
	int iconWidth = contentArea.height;
	bCloseIconDown = (x >= contentArea.width + contentArea.left);
	bIconDown = x < contentArea.left;

	if (!bCloseIconDown && !bIconDown) {
		OUI::on_mouse_down(x, y, flags);
		release_capture();

		x -= contentArea.left;
		y -= contentArea.top;
		set_index(get_index(x, y));

		blink = true;
		invalidate();
		reset_selection();
	}

	invalidate();
}

void UIText::on_dbl_click(int x, int y, uint32_t flags) {
	Rect rc;
	UILabel::get_content_area(rc);
	int iconWidth = rc.height;
	bCloseIconDown = (x >= rc.width + rc.left - iconWidth);
	bool bIconIconDown = x < iconWidth;
	if (!bCloseIconDown && !bIconIconDown) {
		selectionBeginIndex = 0;
		set_index((int)text.length());
		invalidate();
	}
	invalidate();
}

void UIText::on_mouse_up(int x, int y, uint32_t flags) {
	Rect rc;
	UILabel::get_content_area(rc);
	int iconWidth = rc.height;
	bool bIconIconDown = x < iconWidth;

	if (bCloseIconDown && (x >= rc.width + rc.left - iconWidth) && showClearIcon) {
		clear();
		process_event(this, Event::Update, 0, true);
	}
	else if (!bIconIconDown) {
		OUI::on_mouse_up(x, y, flags);
	}
}

void UIText::on_mouse_move(int x, int y, uint32_t flags) {
	if (!bCloseIconDown && !bIconDown) {
		if (flags & MK_LBUTTON) {
			x -= contentArea.left;
			y -= contentArea.top;
			x = Max(x, -1);
			//shiftX++;
			if (x < 0) {

				int t = 0;
			}

			set_index(get_index(x, y));
			invalidate();
		}
	}
	bHover = true;
	invalidate();
}

void UIText::on_mouse_enter(OUI* prev) {
	UILabel::on_mouse_enter(prev);
	bHover = true;
	//if (!bActive) border.set_color(Color("#868993"));
	invalidate();
}

void UIText::on_mouse_leave(OUI* next) {
	UILabel::on_mouse_leave(next);
	bHover = false;
	//if (!bActive) border.set_color(Color("#6a6d78"));
	invalidate();
}

void UIText::set_index(int index) {
	int i, sz, prev, textW, textH, ix, iy, posx, posy;
	bool bSingleLine;
	PGlyphSlot slot;
	Rect rect;
	auto faceHandle = canvas.faceHandle;
	bSingleLine = true;
	prev = currIndex;
	currIndex = index;
	currIndex = CLAMP2(currIndex, text.length());

	SlotCacher::load_box16((wchar_t*)text.c_str(), text.length(), faceHandle);
	auto& box = faceHandle.box;
	hb_glyph_position_t* gpos = faceHandle.gpos;
	hb_glyph_info_t* ginfo = faceHandle.ginfo;
	textW = box.get_width();
	textH = box.get_height();
	posx = shiftX;
	posy = 0;

	cursorPos.set(0, 0);
	get_content_area(rect);
	int cursorLineHeight = rect.height - 4;
	int cursorY = (rect.height - cursorLineHeight) / 2;
	sz = int(faceHandle.gcount);
	if (canvas.art.alignX != Align::RIGHT) {
		for (i = 0; i < sz; ++i) {
			slot = SlotCacher::load_glyph(ginfo[i].codepoint, faceHandle);
			if (!slot) break;

			ix = posx + __SL(gpos[i].x_offset + slot->metrics.horiBearingX);
			iy = textH - (posy + __SL(gpos[i].y_offset + slot->metrics.horiBearingY));

			posx += __SL(gpos[i].x_advance);
			posy += __SL(gpos[i].y_advance);

			if (int(i) == currIndex) {
				if (ix >= rect.width) {
					posx = ix;
					break;
				}

				if (ix < 0)
					shiftX += -ix;
			}
		}

		if (int(i) == currIndex && posx >= rect.width) {
			shiftX += rect.width - posx - 2;
			posx = rect.width;
		}
		if (posx < rect.width)
			shiftX += rect.width - posx - 2;

		shiftX = Min(0, shiftX);
		cursorPos.set(rect.left + posx, rect.top + cursorY);
	}
	else if (canvas.art.alignX == Align::RIGHT) {
		if (textW <= rect.width) {
			shiftX = rect.width - textW - 2;
			return;
		}

		for (i = 0; i < sz; ++i) {
			slot = SlotCacher::load_glyph(ginfo[i].codepoint, faceHandle);
			if (!slot) break;

			ix = posx + __SL(gpos[i].x_offset + slot->metrics.horiBearingX);
			iy = textH - (posy + __SL(gpos[i].y_offset + slot->metrics.horiBearingY));

			posx += __SL(gpos[i].x_advance);
			posy += __SL(gpos[i].y_advance);

			if (int(i) == currIndex) {
				if (ix >= rect.width) {
					posx = ix;
					break;
				}

				if (ix < 0)
					shiftX += -ix;
			}
		}

		if (int(i) == currIndex && posx >= rect.width)
			shiftX += rect.width - posx - 2;
		if (posx < rect.width)
			shiftX += rect.width - posx - 2;

		shiftX = Min(rect.width - 2, shiftX);
		cursorPos.set(rect.left + shiftX + posx, rect.top + cursorY);
	}
}

int UIText::get_index(int x, int y) {
	bool bSingleLine = true;
	int i, textW, textH, ix, iy, posx, posy;
	PGlyphSlot slot;
	auto& faceHandle = canvas.faceHandle;

	SlotCacher::load_box16((wchar_t*)text.c_str(), text.length(), faceHandle);
	auto& box = faceHandle.box;
	hb_glyph_position_t* gpos = faceHandle.gpos;
	hb_glyph_info_t* ginfo = faceHandle.ginfo;
	textW = box.get_width();
	textH = box.get_height();
	posx = shiftX;
	posy = 0;

	if (x < shiftX)
		return 0;

	i = 0;
	for (int sz = int(faceHandle.gcount); i < sz; ++i) {
		slot = SlotCacher::load_glyph(ginfo[i].codepoint, faceHandle);
		if (!slot) break;

		ix = posx + __SL(gpos[i].x_offset + slot->metrics.horiBearingX);
		iy = textH - (posy + __SL(gpos[i].y_offset + slot->metrics.horiBearingY));

		if (x >= posx && x <= (posx + __SL(gpos[i].x_advance))) {
			if (x >= 0) {
				if (x < (posx + __SL(gpos[i].x_advance) / 2))
					return i;
				else
					return i + 1;
			}
			else {
				if (x < (posx + __SL(gpos[i].x_advance) / 2))
					return i - 1;
				else
					return i;
			}

		}

		posx += __SL(gpos[i].x_advance);
		posy += __SL(gpos[i].y_advance);
	}

	return i;
}

void UIText::on_key_down(uint32_t key, uint32_t nrep, uint32_t flags) {
	if (!bActive) return;
	if (key != VK_SHIFT && key != VK_CAPITAL) {

		invalidate();
		blink = true;

		switch (key) {
		case VK_TAB:
		case VK_RETURN:
		case VK_ACCEPT:
		case VK_DOWN:
		case VK_UP:
		case VK_ESCAPE:
			break;
		case VK_END:
			set_index((int)text.length());
			if (!uix->is_key_down(VK_SHIFT)) reset_selection();
			break;

		case VK_HOME:
			set_index(0);
			if (!uix->is_key_down(VK_SHIFT)) reset_selection();
			break;

		case VK_LEFT:
		case VK_RIGHT: {
			int t = currIndex + (key == VK_RIGHT ? 1 : -1);
			set_index(Min(Max(t, 0), int(text.length())));
			if (!uix->is_key_down(VK_SHIFT)) reset_selection();
		}
					 break;

		case VK_BACK:
		{
			int mn = Min(currIndex, selectionBeginIndex);
			int mx = Max(currIndex, selectionBeginIndex);
			if (mn == mx) {
				mn = Max(mn - 1, 0);
				text = text.substr(0, mn) + text.substr(mx);
				set_index(currIndex - 1);
			}
			else {
				text = text.substr(0, mn) + text.substr(mx);
				set_index(mn);
			}
			reset_selection();
			process_event(this, Event::Update, 0, true);
		}
		break;

		default:
			if (uix->is_key_down(VK_CONTROL)) {
				switch (tolower(key)) {
				case 'a':
					selectionBeginIndex = 0;
					set_index((int)text.length());
					break;

				case 'c':
				case 'x':
				{
					int mn = Min(currIndex, selectionBeginIndex);
					int mx = Max(currIndex, selectionBeginIndex);
					uix->copy_to_clipboard(text.substr(mn, mx - mn));

					if (tolower(key) == 'x') {
						int mn = Min(currIndex, selectionBeginIndex);
						int mx = Max(currIndex, selectionBeginIndex);
						if (mn != mx) {
							text = text.substr(0, mn) + text.substr(mx);
							set_index(mn);
						}
						reset_selection();
						process_event(this, Event::Update, 0, true);
					}
				}
				break;

				case 'v':
				{
					std::wstring v = uix->copy_from_clipboard();
					if (v.length()) {
						int mn = Min(currIndex, selectionBeginIndex);
						int mx = Max(currIndex, selectionBeginIndex);
						if (mn != mx) {
							text = text.substr(0, mn) + text.substr(mx);
							currIndex = mn;
							selectionBeginIndex = mn;
						}
						if (pre_append(v)) {
							text.insert(currIndex, v);
							set_index(currIndex + (int)v.length());
							reset_selection();
							process_event(this, Event::Update, 0, true);
						}
					}
				}
				break;
				}
			}
			else {
				wchar_t ch = uix->convert_char(key);
				if (!ch || !pre_append(ch)) return;

				int mn = Min(currIndex, selectionBeginIndex);
				int mx = Max(currIndex, selectionBeginIndex);
				if (mn != mx) {
					text = text.substr(0, mn) + text.substr(mx);
					currIndex = mn;
				}

				blink = true;
				text.insert(currIndex, 1, ch);
				set_index(currIndex + 1);
				reset_selection();
				process_event(this, Event::Update, 0, true);
			}
		}
	}

	if (parent) parent->on_key_down(key, nrep, flags);
}

void UIText::clear() {
	text = L"";
	currIndex = 0;
	reset_selection();
	invalidate();
}

void UIText::set_direction(Align align) {
	defaultAlignX = align;
}

void UIText::on_update() {
	if (bActive) {
		Color c = backgroundColor;
		backgroundColor.brightness(&c, -1);
		OUI::on_update();
		backgroundColor = c;
	}
	else
		OUI::on_update();

	Sheet* sheet = canvas.sheet;
	if (IS_NULL(sheet)) return;

	int ix, iy, dy, posx, posy, count,
		i, glyphw, glyphh, glyphPitch, cursorLineHeight,
		cursorY, cursorLineWidth, penLeft, penTop;
	Rect rect;
	Art& art = canvas.art;
	wchar_t* text = (wchar_t*)this->text.c_str();
	auto len = this->text.length();
	size_t selStart, selEnd;

	get_content_area(rect);
	Rect rcc(0, 0, rect.width, rect.height);
	Rect sel(0, 0, 0, rect.height);

	Rect rc = rect;
	rc.shift(area.left, area.top);
	ShapeStorage sha;
	sha.recti(rc.left, rc.top, rc.width, rc.height);
	canvas.sheet->setclip(&sha, 0xff);

	if (!len && showPlaceHolder) {
		canvas.art.textColor.set(placeholderColor);
		canvas.art.alignX = defaultAlignX;
		canvas.draw_text16((wchar_t*)placeholder.c_str(), rect);
	}
	else {
		canvas.art.textColor.set(color);
	}

	auto faceHandle = canvas.faceHandle;
	SlotCacher::load_box16(text, len, faceHandle);
	auto gpos = faceHandle.gpos;
	auto ginfo = faceHandle.ginfo;
	PGlyphSlot slot;

	penLeft = 0;
	penTop = (int)canvas.get_max_height();
	len = wcslen(text);

	if (art.alignY == Align::CENTER) {
		int textH = (int)canvas.get_max_height();
		penTop += (rect.height - textH) / 2;
	}
	if (art.alignY == Align::BOTTOM)
		penTop += rect.height;

	cursorLineWidth = 1;
	selStart = size_t(Min(selectionBeginIndex, currIndex));
	selEnd = size_t(CLAMP3(selectionBeginIndex, currIndex, int(faceHandle.gcount)));
	posx = shiftX;
	posy = 0;

	iterateI(selEnd) {
		slot = SlotCacher::load_glyph(ginfo[i].codepoint, faceHandle);
		if (!slot) break;

		ix = posx + __SL(gpos[i].x_offset + slot->metrics.horiBearingX);
		iy = posy + __SL(gpos[i].y_offset + slot->metrics.horiBearingY);

		posx += __SL(gpos[i].x_advance);
		posy += __SL(gpos[i].y_advance);
		if (i < selStart) continue;

		if (i == selStart) {
			sel.left = ix;
		}
		sel.width = posx - sel.left;
	}

	if (sel.width) {
		sel.shift(rect.left, rect.top);
		canvas.fill_rounded_rect(&sel, 1, &OUITheme::textSelection);
	}

	posx = shiftX;
	posy = 0;
	cursorLineHeight = rect.height - 4;
	cursorY = (rect.height - cursorLineHeight) / 2;
	canvas.art.strokeColor.set(OUITheme::text);

	{
		int x, y, ix, iy, w, h, pitch, nbpp, dix, diy;
		byte a, p, red, grn, blu, alp, cla, clp;
		pyte d, s, data, glyphData;
		short px;
		w = sheet->w;
		h = sheet->h;
		pitch = sheet->pitch;
		nbpp = sheet->nbpp;
		data = sheet->data;
		red = art.textColor.r;
		grn = art.textColor.g;
		blu = art.textColor.b;
		alp = art.textColor.a;
		px = (art.textColor.b | art.textColor.g << 8);

		auto gpos = faceHandle.gpos;
		auto ginfo = faceHandle.ginfo;
		int gcount = faceHandle.gcount;
		int sx, sy, ex, ey;

		/*sx = rc.left + area.left;
		sy = rc.top + area.top;
		ex = rc.width - 1;
		ey = rc.height - 1;
		ex = ex + sx;
		ey = ey + sy;*/
		sx = 0;
		sy = 0;
		ex = sheet->w - 1;
		ey = sheet->h - 1;
		penTop += rc.top;
		penLeft += rc.left;

		for (i = 0; i < gcount; i++) {
			auto* slot = SlotCacher::load_glyph(ginfo[i].codepoint, faceHandle);
			if (!slot) break;
			ix = posx + __SL(gpos[i].x_offset + slot->metrics.horiBearingX);
			iy = posy + __SL(gpos[i].y_offset + slot->metrics.horiBearingY);
			posx += __SL(gpos[i].x_advance);
			posy += __SL(gpos[i].y_advance);
			if (i == currIndex && blink) {
				canvas.draw_vertical_line(rect.left + ix, rect.top + cursorY, rect.top + cursorY + cursorLineHeight, cursorLineWidth);
			}

			if ((posx + penLeft) < sx || (ix + penLeft) > ex) continue;

			glyphw = slot->bitmap.width;
			glyphh = slot->bitmap.rows;
			glyphPitch = slot->bitmap.pitch;
			glyphData = slot->bitmap.buffer;
			y = 0, dy = (penTop - iy);

			if (dy < sy) {
				int ddy = sy - dy;
				y += ddy;
				dy += ddy;
			}

			for (; y < glyphh; y++, dy++) {
				dix = ix + penLeft;
				diy = dy;
				x = 0;

				if (diy > ey) break;

				if (dix < sx) {
					int dx = sx - dix;
					dix += dx;
					dx = Min(dx, glyphPitch);
					x += dx;
				}
				if (dix > ex) break;

				d = data + diy * pitch + 3 * dix;
				s = glyphData + y * glyphPitch + x;
				sheet->clip(dix, diy, &count, &cla);
				clp = 0xff - cla;

				for (; x < glyphw; x++, s++, dix++, d += 3, --count) {
					if (dix < sx) continue;
					if (dix > ex) break;

					if (count < 1) {
						sheet->clip(dix, &count, &cla);
						clp = 0xff - cla;
					}

					if (cla == 0 || *s == 0) {
						continue;
					}
					else {
						a = DIV255(DIV255(cla * *s) * alp);
						p = 0xff - a;

						d[0] = DIV255(a * blu) + DIV255(p * d[0]);
						d[1] = DIV255(a * grn) + DIV255(p * d[1]);
						d[2] = DIV255(a * red) + DIV255(p * d[2]);
					}
				}
			}
		}

		if (i == currIndex && blink) {
			canvas.draw_vertical_line(rect.left + posx, rect.top + cursorY, rect.top + cursorY + cursorLineHeight, cursorLineWidth);
		}
	}
	canvas.sheet->unclip();

	UILabel::get_content_area(rc);
	int l = rc.left, t = rc.top;
	Spacing pad;

	if (m_path) {
		l = rc.left = padding.top;
		double ds = fmin(double(rc.width - 1) / fabs(m_path->width), double(rc.height - 1) / fabs(m_path->height));
		if (m_path->width <= 0 || m_path->height <= 0) ds = 0;

		int tt = rc.top;
		if (canvas.art.alignY == Align::CENTER)
			tt += int((rc.height - m_path->height * ds - m_path->pt - m_path->pb) / 2);
		if (canvas.art.alignY == Align::BOTTOM)
			tt += int(rc.height - m_path->height * ds - m_path->pt - m_path->pb);

		pad.set(4);
		m_path->m_colors = &colors;
		canvas.render_svg(m_path, l, tt, rc.width, rc.height, opacity, &pad);
	}
	if (showClearIcon && len && bHover) {
		double ds = fmin(double(rc.width - 1) / fabs(closeIcon->width), double(rc.height - 1) / fabs(closeIcon->height));
		if (closeIcon->width <= 0 || closeIcon->height <= 0) ds = 0;

		l = int(area.width - padding.right - 1 - closeIcon->pl - closeIcon->pr - closeIcon->width * ds);
		int tt = rc.top;
		if (canvas.art.alignY == Align::CENTER)
			tt += int((rc.height - closeIcon->height * ds - closeIcon->pt - closeIcon->pb) / 2);
		if (canvas.art.alignY == Align::BOTTOM)
			tt += int(rc.height - closeIcon->height * ds - closeIcon->pt - closeIcon->pb);

		pad.set(6);
		closeIcon->m_colors = &colors;
		canvas.render_svg(closeIcon, l, tt, rc.width, rc.height, opacity, &pad);
	}
}

void UIText::on_resize(int width, int height) {
	set_index(currIndex);
}

void UIText::set_place_holder_text(std::wstring placeholder) {
	this->placeholder = placeholder;
	invalidate();
}

void UIText::set_place_holder_color(Color color) {
	this->placeholderColor = color;
	invalidate();
}

OUI* UIText::create(int left, int top, int width, int height, OUI* parent, bool bAddToParent) {
	UILabel::create(left, top, width, height, parent, bAddToParent);
	if (parent) set_background_color(parent->backgroundColor.bright(+13));
	closeIcon = parse_svg(R"(
		<svg>
			<path d="M13.4 12l6.6 6.6-1.4 1.4-6.6-6.6L5.4 20 4 18.6l6.6-6.6L4 5.4 5.4 4l6.6 6.6L18.6 4 20 5.4 13.4 12z" fill="currentColor"></path>
		</svg>
	)");
	return this;
}

void UIText::get_content_area(Rect& rc) {
	UILabel::get_content_area(rc);
	int iconWidth = rc.height;
	if (m_path) {
		int dx = rc.left - padding.top;
		rc.left -= dx;
		rc.width += dx;
		rc.left += iconWidth;
		rc.width -= iconWidth + (text.length() ? iconWidth : 0);
	}
	else {
		rc.width -= (text.length() ? iconWidth : 0);
	}
}

void UIText::set_icon(std::string svg) {
	m_path = parse_svg(svg);
}

void UIText::set_color(Color color) {
	UILabel::set_color(color);
	colors["currentColor"] = color;
}

bool UIText::pre_append(wchar_t c) {
	return true;
}

bool UIText::pre_append(std::wstring str) {
	return true;
}

void UIText::trigger_update() {
	if (parent) parent->process_event(this, Event::Update, 0, true);
}

#pragma endregion

// UINumber

#include <regex>

UINumber::UINumber() : UIText(), number("0", "1", "na", "na") {
	num_to_text();
	set_place_holder_text(L"");
	showClearIcon = false;
	btnIncrease.create(
		Rect(0, 0, 20, 20),
		0,
		parse_svg(R"(<svg viewBox="0 0 11 7"><path fill="none" stroke="currentColor" stroke-width="1.3" d="m.5 5.5 5-4 5 4"/></svg>)"));
	btnDecrease.create(
		Rect(0, 0, 20, 20),
		0,
		parse_svg(R"(<svg viewBox="0 0 11 7" width="11" height="7" fill="none"><path stroke="currentColor" stroke-width="1.3" d="m.5 1.5 5 4 5-4"/></svg>)"));

	btnIncrease.set_colors(Colors::lightgray, Colors::white, Colors::darkgray);
	btnDecrease.set_colors(Colors::lightgray, Colors::white, Colors::darkgray);
}

bool UINumber::pre_append(wchar_t c) {
	if (::iswdigit(c) != 0) return true;
	if (c != L'.' && c != L'+' && c != L'-') return false;

	for (auto ch : text) {
		if (ch == c) return false;
	}

	return true;
}

bool UINumber::pre_append(std::wstring str) {
	return std::regex_match(str, std::wregex(L"[(-|+)|][0-9]+"));
}

void UINumber::blur() {
	if (text.length() == 0)
		set_text(L"0");
	text_to_num();
	trigger_update();
	UIText::blur();
}

void UINumber::on_update() {
	UIText::on_update();
	btnIncrease.draw(canvas);
	btnDecrease.draw(canvas);
}

void UINumber::on_resize(int width, int height) {
	UIText::on_resize(width, height);
	Rect rc; UILabel::get_content_area(rc);
	auto sh = (contentArea.height / 2) * .7;
	int sw = int(sh * btnIncrease.get_aratio());
	int l = area.width;
	int mid = (rc.height) / 2;
	btnIncrease.rc.set(l - sw, (int)round((mid - sh) / 2.0) + rc.top, sw, (int)sh);
	btnDecrease.rc.set(l - sw, (int)round((mid - sh) / 2.0) + mid + rc.top, sw, (int)sh);
}

void UINumber::on_key_down(uint32_t key, uint32_t nrep, uint32_t flags) {
	UIText::on_key_down(key, nrep, flags);
	if (!bActive || is_key_on(VK_CONTROL)) return;
	switch (key) {
	case VK_UP:
		increase();
		break;
	case VK_DOWN:
		decrease();
		break;
	case VK_RETURN:
		text_to_num();
		trigger_update();
		invalidate();
		break;
	default:
		auto str = ocom::to_string(text);
		if (is_number(str) && number.is_in_range(str)) {
			text_to_num();
			trigger_update();
			invalidate();
		}
		break;
	}
}

void UINumber::trigger_update() {
	if (parent) parent->process_event(this, Event::Update, 0, true);
}

Float UINumber::get_number() const {
	return number.get_number();
}

void UINumber::increase() {
	++number;
	num_to_text();
	trigger_update();
	invalidate();
}

void UINumber::decrease() {
	--number;
	num_to_text();
	trigger_update();
	invalidate();
}

void UINumber::on_timer(uint32_t nTimer) {
	UIText::on_timer(nTimer);
	if (nTimer != 1 && nTimer != 2) return;

	if ((clock() - lastChange) < waitingElapse)
		return;
	lastChange = clock();
	waitingElapse = Max(1U, (clock_t)(waitingElapse * UINUMBER_CHANGE_ACCELERATION));

	if (nTimer == 1) {
		increase();
	}
	else if (nTimer == 2) {
		decrease();
	}
}

void UINumber::config(std::string number, std::string tick, std::string minPrice, std::string maxPrice) {
	this->number.set(number, tick, minPrice, maxPrice);
	num_to_text();
}

void UINumber::text_to_num() {
	auto str = ocom::to_string(text);
	if (!is_number(str)) str = "0";
	number.set(str);
	str = number.str();
	set_text(ocom::to_wstring(str));
}

void UINumber::num_to_text() {
	auto str = number.str();
	set_text(ocom::to_wstring(str));
}

void UINumber::on_mouse_move(int x, int y, uint32_t flags) {
	if (!btnIncrease.on_mouse_move(x, y, flags).bHover)
		if (!btnDecrease.on_mouse_move(x, y, flags).bHover)
			UIText::on_mouse_move(x, y, flags);
	invalidate();
}

void UINumber::on_mouse_down(int x, int y, uint32_t flags) {
	btnIncrease.on_mouse_down(x, y, flags);
	btnDecrease.on_mouse_down(x, y, flags);
	if (btnIncrease.bDown) {
		kill_timer(2);
		set_timer(1, UINUMBER_CHANGE_TICK_ELAPSE);
	}
	else if (btnDecrease.bDown) {
		kill_timer(1);
		set_timer(2, UINUMBER_CHANGE_TICK_ELAPSE);
	}
	waitingElapse = UINUMBER_CHANGE_ELAPSE;
	lastChange = clock();
	lastDown = clock();
	UIText::on_mouse_down(x, y, flags);
	if (!bActive) focus();
	invalidate();
}

void UINumber::on_mouse_up(int x, int y, uint32_t flags) {
	kill_timer(1);
	kill_timer(2);
	if (waitingElapse == UINUMBER_CHANGE_ELAPSE) {
		if (btnIncrease.on_mouse_up(x, y, flags)) {
			increase();
		}
		else if (btnDecrease.on_mouse_up(x, y, flags)) {
			decrease();
		}
		else {
			UIText::on_mouse_up(x, y, flags);
		}
	}
	else {
		UIText::on_mouse_up(x, y, flags);
	}
	invalidate();
}

void UINumber::on_dbl_click(int x, int y, uint32_t flags) {
	UIText::on_dbl_click(x, y, flags);
	btnIncrease.on_mouse_down(x, y, flags);
	btnDecrease.on_mouse_down(x, y, flags);
	invalidate();
}

void UINumber::on_mouse_enter(OUI* prev) {
	UIText::on_mouse_enter(prev);
}

void UINumber::on_mouse_leave(OUI* next) {
	UIText::on_mouse_leave(next);
	btnIncrease.bHover = false;
	btnDecrease.bHover = false;
	btnIncrease.adj_colors();
	btnDecrease.adj_colors();
	invalidate();
}

// UIEditableLabel 

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
	return bFocusable && is_editable();
}

void UIEditableLabel::on_key_down(uint32_t key, uint32_t nrep, uint32_t flags) {
	UIText::on_key_down(key, nrep, flags);
	switch (key) {
	case VK_RETURN:
		uix->focus_next();
		break;
	}
}