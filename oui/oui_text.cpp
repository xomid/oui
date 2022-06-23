#include "oui_text.h"
#include "oui_uix.h"
#include <precise_float.h>

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
	set_index(currIndex);
	blink = true;
	set_timer(0, blinkerSpeed);
	//process_event(this, Event::Focus, 0, true);
}

void UIText::blur() {
	UILabel::blur();
	reset_selection();
	blink = false;
	kill_timer(0);
	//process_event(this, Event::Blur, 0, true);
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
	bool bIconIconDown = m_path ? x < iconWidth : x < padding.left;
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
			trigger_update();
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
						trigger_update();
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
							trigger_update();
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
				trigger_update();
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
		cursorX, cursorY, cursorLineWidth, penLeft, penTop;
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
	cursorLineHeight = rect.height - padding.top;
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
		cursorX = rect.left + posx;

		for (i = 0; i < gcount; i++) {
			auto* slot = SlotCacher::load_glyph(ginfo[i].codepoint, faceHandle);
			if (!slot) break;
			ix = posx + __SL(gpos[i].x_offset + slot->metrics.horiBearingX);
			iy = posy + __SL(gpos[i].y_offset + slot->metrics.horiBearingY);
			posx += __SL(gpos[i].x_advance);
			posy += __SL(gpos[i].y_advance);
			if (i == currIndex && blink)
				cursorX = rect.left + ix;

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

		if (blink) {
			if (i == currIndex)
				cursorX = rect.left + posx;
			canvas.draw_vertical_line(cursorX, cursorY, rect.top + cursorY + cursorLineHeight, cursorLineWidth);
		}
	}
	canvas.sheet->unclip();

	UILabel::get_content_area(rc);
	int l = rc.left, t = rc.top;
	Spacing pad;

	if (m_path) {
		l = rc.left = padding.top;
		auto ds = calc_scale(rc.width, rc.height, m_path);

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
		int closeHeight = Min(rc.height, fontSize);
		auto ds = calc_scale(rc.width, closeHeight, closeIcon);

		l = int(area.width - padding.right - 1 - closeIcon->pl - closeIcon->pr - closeIcon->width * ds);
		int tt = rc.top;
		if (canvas.art.alignY == Align::CENTER)
			tt += int((rc.height - closeIcon->height * ds - closeIcon->pt - closeIcon->pb) / 2);
		if (canvas.art.alignY == Align::BOTTOM)
			tt += int(rc.height - closeIcon->height * ds - closeIcon->pt - closeIcon->pb);

		pad.set(Max(1, 1));
		closeIcon->m_colors = &colors;
		canvas.render_svg(closeIcon, l, tt, rc.width, closeHeight, opacity, &pad);
	}
}

void UIText::on_resize(int width, int height) {
	padding.set(4, 4, 4, 8);
	padding.bottom = padding.top = CLAMP3(1, int(height * .2), 4);
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

