#include "agg_scanline_p.h"
#include "agg_scanline_u.h"
#include "agg_scanline_bin.h"
#include "agg_scanline_boolean_algebra.h"
#include "agg_scanline_storage_aa.h"
#include "agg_scanline_storage_bin.h"

#include "oui.h"
#include "oui_canvas.h"
#include "oui_blur.h"
#include "oui_scroll.h"
#include "oui_uix.h"

byte* OUI::blur_stack = NULL;

OUI* OUI::find_element(OUI* curr, int x, int y)
{
	if (!curr || !curr->isVisible) return NULL;
	std::vector<OUI*>* elems = &curr->elements;
	for (size_t i = 0, sz = elems->size(); i < sz; i++) {
		OUI* elem = elems->at(i);
		if (!elem || !elem->isVisible) continue;
		if (elem->area.is_inside(x, y)) {
			return find_element(elem, x, y);
		}
		else if (elem->elements.size() && elem->overflow == Overflow::visible) {
			OUI* e = find_element(elem, x, y);
			if (e != elem) return e;
		}
	}

	if (curr->scrollX && curr->scrollX->area.is_inside(x, y) && curr->scrollX->isVisible)
		return (OUI*)curr->scrollX;
	if (curr->scrollY && curr->scrollY->area.is_inside(x, y) && curr->scrollY->isVisible)
		return (OUI*)curr->scrollY;

	return curr;
}

int OUI::init() {
	auto res = SlotCacher::init();
	if (!blur_stack) blur_stack = new byte[256];
	return res;
}

void OUI::destroy() {
	delete[] blur_stack;
	blur_stack = NULL;
	SlotCacher::destroy();
}

OUI::~OUI() {
	on_destroy();
	if (scrollX) delete scrollX;
	if (scrollY) delete scrollY;
}

OUI::OUI() {
	Color c;
	menu = NULL;
	parent = NULL;
	godSheet = NULL;
	isCreated = false;
	isVisible = false;
	isFocusable = false;
	isActive = false;
	isEnabled = true;
	isSelected = false;
	isHover = false;
	isPressed = false;
	isMenu = false;
	isDraggable = false;
	zIndex = 0;
	isScrollable = false;
	scrollX = NULL;
	scrollY = NULL;
	opacity = 0xff;
	canvas.art.alignX = Align::CENTER;
	canvas.art.alignY = Align::CENTER;
	floatType = UIFloatType::Left;
	overflow = Overflow::visible;
	menuType = MenuType::Fluid;
	menuActivationMode = MenuActivationMode::PointerHover;

	backgroundColor = OUITheme::primary;
	color = OUITheme::text;

	boxModel.reset();
	padding.reset();
	border.reset();
	contextPadding.set(0);
	area.set(0, 0, 0, 0);
	contextArea.set(0, 0, 0, 0);
	borderRadius.reset();
	canvas.set_line_height(InitialValues::lineHeight);
	canvas.set_area(&contextArea);
	canvas.add_font(fontName = InitialValues::fontName, L"C:\\dev\\fonts\\SegoeUIVF.ttf");
	canvas.load_handle(fontName, fontSize = InitialValues::fontSize);
}

void OUI::on_update() {
	canvas.area = &area;

	if (outsetBoxShadows.size()) {
		canvas.bit_blt(outsetBoxShadowSheet,
			outsetBoxShadowSheet.offset.x,
			outsetBoxShadowSheet.offset.y,
			outsetBoxShadowSheet.w, outsetBoxShadowSheet.h, 0, 0, true);
	}

	if (shape.is_rect()) {
		Rect bord(0, 0, area.width, area.height);
		canvas.clear(bord, border, backgroundColor, opacity);
	}
	else {
		Border& bor = border;
		if (!border.is_empty()) {
			//if (isActive) canvas.render(shape.ras, shape.sl, OUITheme::borderActive);
			canvas.render(shape.ras, shape.sl, border.leftColor);
		}
		canvas.render(contentShape.ras, contentShape.sl, backgroundColor, opacity);
	}

	if (insetBoxShadows.size()) {
		Rect rc; get_content_area(rc);
		canvas.bit_blt(insetBoxShadowSheet,
			rc.left, rc.top, insetBoxShadowSheet.w, insetBoxShadowSheet.h, 0, 0, true);
	}

	canvas.area = &contextArea;
}

void OUI::move_shape() {
	int w, h, l, t;
	Rect rc;
	OUI::get_content_area(rc);
	l = rc.left;
	t = rc.top;
	w = area.width;
	h = area.height;

	if (borderRadius.is_empty()) {
		shape.recti(0, 0, w, h);
		contentShape.recti(l, t, rc.width, rc.height);
		absContentShape.recti(area.left + l, area.top + t, rc.width, rc.height);
	}
	else {
		shape.rounded_rect(0.0, 0.0, (double)w, (double)h, borderRadius);
		contentShape.rounded_rect(0.0, 0.0, (double)w, (double)h, borderRadius, border);
		absContentShape.rounded_rect(area.left, area.top, (double)w, (double)h, borderRadius, border);
	}
}

void OUI::calc_shape() {
	int w, h, l, t;
	Rect rc;
	OUI::get_content_area(rc);
	l = rc.left;
	t = rc.top;
	w = area.width;
	h = area.height;

	if (borderRadius.is_empty()) {
		shape.recti(0, 0, w, h);
		contentShape.recti(l, t, rc.width, rc.height);
		absContentShape.recti(area.left + l, area.top + t, rc.width, rc.height);
	}
	else {
		shape.rounded_rect(0.0, 0.0, (double)w, (double)h, borderRadius);
		contentShape.rounded_rect(0.0, 0.0, (double)w, (double)h, borderRadius, border);
		absContentShape.rounded_rect(area.left, area.top, (double)w, (double)h, borderRadius, border);
	}

	size_t sz = insetBoxShadows.size();
	Color transparent(0, 0, 0, 0);
	if (sz && area.width > 0 && area.height > 0) {

		//Color cl(0, 0, 0, 0);
		Canvas can(NULL, &insetBoxShadowSheet);
		insetBoxShadowSheet.create(rc.width, rc.height, OUI_BGRA);
		insetBoxShadowSheet.clear(0);
		//insetBoxShadowSheet.clear_solid(cl);

		iterateIRev(sz) {
			BoxShadow& boxs = insetBoxShadows[i];
			if (ocom::is_zeroi(boxs.offsetX) && ocom::is_zeroi(boxs.offsetY) && ocom::is_zeroi(boxs.blur) && ocom::is_zeroi(boxs.spread)) continue;

			boxs.sheet.create(rc.width + 2 * boxs.blur, rc.height + 2 * boxs.blur, OUI_GRAY);
			boxs.sheet.clear(0xff);

			agg::trans_affine mtx;
			trans_shape trans(contentShape, mtx);
			mtx *= agg::trans_affine_scaling(fmax((area.width + 2 * boxs.spread) / double(area.width), 0),
				fmax((area.height + 2 * boxs.spread) / double(area.height), 0));
			mtx *= agg::trans_affine_translation(boxs.offsetX + boxs.blur, boxs.offsetY + boxs.blur);

			Canvas can2(NULL, &boxs.sheet);
			can2.clear_solid(trans, transparent);

			if (boxs.blur > 0) {
				memset(blur_stack, 0, sizeof(blur_stack));
				stackblurJobG(boxs.sheet.data, boxs.sheet.w, boxs.sheet.h, boxs.sheet.pitch, boxs.blur, 1, blur_stack);
				stackblurJobG(boxs.sheet.data, boxs.sheet.w, boxs.sheet.h, boxs.sheet.pitch, boxs.blur, 2, blur_stack);
			}

			insetBoxShadowSheet.setclip(&contentShape, 0xff);
			can.bit_blt(boxs.sheet, 0, 0, insetBoxShadowSheet.w, insetBoxShadowSheet.h, boxs.blur, boxs.blur, boxs.color);
			insetBoxShadowSheet.unclip();
		}
	}

	sz = outsetBoxShadows.size();
	if (sz && area.width > 0 && area.height > 0) {
		int l, t, r, b;
		bool bfirst = true;

		iterateI(sz) {
			BoxShadow& boxs = outsetBoxShadows[i];
			if (ocom::is_zeroi(boxs.offsetX) && ocom::is_zeroi(boxs.offsetY) && ocom::is_zeroi(boxs.blur) && ocom::is_zeroi(boxs.spread)) continue;

			if (bfirst) {
				l = boxs.offsetX - boxs.spread - boxs.blur;
				t = boxs.offsetY - boxs.spread - boxs.blur;
				r = l + area.width + (boxs.spread + boxs.blur) * 2;
				b = t + area.height + (boxs.spread + boxs.blur) * 2;
				bfirst = false;
			}
			else {
				int L = boxs.offsetX - boxs.spread - boxs.blur,
					T = boxs.offsetY - boxs.spread - boxs.blur;
				l = min(l, L);
				t = min(t, T);
				r = max(r, L + area.width + (boxs.spread + boxs.blur) * 2);
				b = max(b, T + area.height + (boxs.spread + boxs.blur) * 2);
			}
		}

		//Color cl(0, 0, 0, 0);
		Canvas can(NULL, &outsetBoxShadowSheet);
		if (!bfirst) {
			outsetBoxShadowSheet.offset.reset(l - 1, t - 1);
			outsetBoxShadowSheet.create(r - l + 2, b - t + 2, OUI_BGRA);
			outsetBoxShadowSheet.clear(0);
			//outsetBoxShadowSheet.clear_solid(cl);
		}
		else sz = 0;

		iterateIRev(sz) {

			BoxShadow& boxs = outsetBoxShadows[i];
			Rect rc(area);

			if (ocom::is_zeroi(boxs.offsetX) && ocom::is_zeroi(boxs.offsetY) && ocom::is_zeroi(boxs.blur) && ocom::is_zeroi(boxs.spread)) continue;

			rc.left = (boxs.offsetX - boxs.spread - boxs.blur);
			rc.top = (boxs.offsetY - boxs.spread - boxs.blur);
			rc.width += 2 * (boxs.spread + boxs.blur);
			rc.height += 2 * (boxs.spread + boxs.blur);
			boxs.sheet.create(rc.width + 2, rc.height + 2, OUI_GRAY);
			boxs.sheet.clear(0);

			agg::rendering_buffer pixbuff(boxs.sheet.data, boxs.sheet.w, boxs.sheet.h, boxs.sheet.pitch);
			pixfmt_gray pf(pixbuff);
			renderer_base_gray ren_base(pf);
			renderer_scanline_gray ren(ren_base);
			rasterizer_scanline ras;
			scanline sl;

			shape.spread(boxs.spread, boxs.spread, false);
			agg::trans_affine mtx;
			mtx *= agg::trans_affine_translation((boxs.sheet.w - 2 - (area.width + 2 * boxs.spread)) / 2.0 + boxs.spread + 1,
				(boxs.sheet.h - 2 - (area.height + 2 * boxs.spread)) / 2.0 + boxs.spread + 1);

			shape.set_matrix(mtx);
			ras.add_path(shape);
			ren.color(agg::gray8(0xff, 0xff));
			agg::render_scanlines(ras, sl, ren);

			if (boxs.blur > 0) {
				memset(blur_stack, 0, sizeof(blur_stack));
				stackblurJobG(boxs.sheet.data, boxs.sheet.w, boxs.sheet.h, boxs.sheet.pitch, boxs.blur, 1, blur_stack);
				stackblurJobG(boxs.sheet.data, boxs.sheet.w, boxs.sheet.h, boxs.sheet.pitch, boxs.blur, 2, blur_stack);
			}

			shape.spread(0, 0, false);
			mtx.reset();
			mtx *= agg::trans_affine_translation(-l + 1, -t + 1);
			shape.set_matrix(mtx);
			outsetBoxShadowSheet.setclip(&shape, 0xff, true);
			can.bit_blt(boxs.sheet,
				rc.left - l,
				rc.top - t,
				outsetBoxShadowSheet.w, outsetBoxShadowSheet.h, 0, 0, boxs.color);
			outsetBoxShadowSheet.unclip();
			shape.remove_matrix();
		}
	}

	//if (outsetBoxShadows.size()) {
	//	/*OUI* par = parent;
	//	while (par) {
	//		if (rc.left >= par->area.left && rc.top >= par->area.top &&
	//			rc.left + rc.width <= par->area.left + par->area.width &&
	//			rc.top + rc.height <= par->area.top + par->area.height) {
	//			par->set_transparency(true);
	//			break;
	//		}
	//		par->area;
	//		par = par->parent;
	//	}*/
	//}
}

void OUI::on_mouse_down(int x, int y, uint32_t param) {
	x = TOABSX(x);
	y = TOABSY(y);

	if (isScrollable) {
		if (scrollX->area.is_inside(x, y) && scrollX->isVisible) {
			scrollX->on_mouse_down(TORELX(x, scrollX->area), TORELY(y, scrollX->area), param);
			return;
		}
		if (scrollY->area.is_inside(x, y) && scrollY->isVisible) {
			scrollY->on_mouse_down(TORELX(x, scrollY->area), TORELY(y, scrollY->area), param);
			return;
		}
	}

	if (menu && menu->menuActivationMode == MenuActivationMode::PointerDown) {
		menu->pop_up();
	}

	focus();
	set_capture(this);
}

void OUI::on_dbl_click(int x, int y, uint32_t param) {
	x = TOABSX(x);
	y = TOABSY(y);

	if (isScrollable) {
		if (scrollX->area.is_inside(x, y) && scrollX->isVisible) {
			scrollX->on_dbl_click(TORELX(x, scrollX->area), TORELY(y, scrollX->area), param);
			return;
		}
		if (scrollY->area.is_inside(x, y) && scrollY->isVisible) {
			scrollY->on_dbl_click(TORELX(x, scrollY->area), TORELY(y, scrollY->area), param);
			return;
		}
	}

	focus();
}

void OUI::on_mouse_up(int x, int y, uint32_t param) {
	if (get_capture() == this)
		release_capture();
	x = TOABSX(x);
	y = TOABSY(y);

	if (isScrollable) {
		if (scrollX->area.is_inside(x, y) && scrollX->isVisible) {
			scrollX->on_mouse_up(TORELX(x, scrollX->area), TORELY(y, scrollX->area), param);
			return;
		}
		if (scrollY->area.is_inside(x, y) && scrollY->isVisible) {
			scrollY->on_mouse_up(TORELX(x, scrollY->area), TORELY(y, scrollY->area), param);
			return;
		}
	}

	if (menu && menu->menuActivationMode == MenuActivationMode::PointerUp) {
		menu->pop_up();
	}
}

void OUI::on_mouse_move(int x, int y, uint32_t param) {
	x = TOABSX(x);
	y = TOABSY(y);

	if (isScrollable) {
		if (scrollX->area.is_inside(x, y) && scrollX->isVisible)
			scrollX->on_mouse_move(TORELX(x, scrollX->area), TORELY(y, scrollX->area), param);
		if (scrollY->area.is_inside(x, y) && scrollY->isVisible)
			scrollY->on_mouse_move(TORELX(x, scrollY->area), TORELY(y, scrollY->area), param);
	}

	hover(area.is_inside(x, y));
}

bool OUI::on_mouse_wheel(int x, int y, int zDelta, uint32_t param) {
	x = TOABSX(x);
	y = TOABSY(y);

	if (isScrollable) {
		if (scrollX->isVisible && y > scrollX->area.top - 50)
			return scrollX->on_mouse_wheel(TORELX(x, scrollX->area), TORELY(y, scrollX->area), zDelta, param);

		if (scrollY->isVisible) {
			(zDelta > 0) ? scrollY->scroll_rel(-1) : scrollY->scroll_rel(+1);
			return true;
		}
	}

	return true;
}

void OUI::on_update_frame() {
	if (uix == NULL) return;

	if (shouldInvalidate) {
		uix->lock_screen(id);
		on_update();
	}

	if (overflow != Overflow::visible) godSheet->setclip(&absContentShape, opacity);
	int currLayerId = 0, nextLayerId;
	bool cont = true;
	while (cont) {
		cont = false;
		nextLayerId = -1;
		for (auto* elem : elements) {
			if (elem && SHEET_OVERLAP(elem->area, *godSheet)) {
				if (elem->isVisible) {
					if (elem->zIndex == currLayerId) {
						elem->shouldInvalidate |= shouldInvalidate;
						elem->on_update_frame();

						if (elem->shouldInvalidate) {
							elem->shouldInvalidate = false;
							if (shouldInvalidate == false)
								uix->update_section(&elem->area);
						}
					}
					else if (elem->zIndex > currLayerId) {
						cont = true;
						if (nextLayerId < 0) nextLayerId = elem->zIndex;
						else nextLayerId = Min(elem->zIndex, nextLayerId);
					}
				}
			}
		}

		if (currLayerId == 0 && isScrollable) {
			if (scrollX && scrollX->isVisible) {
				scrollX->shouldInvalidate |= shouldInvalidate;
				scrollX->on_update_frame();

				if (scrollX->shouldInvalidate) {
					scrollX->shouldInvalidate = false;
					if (shouldInvalidate == false)
						uix->update_section(&scrollX->area);
				}
			}

			if (scrollX && scrollY->isVisible) {
				scrollY->shouldInvalidate |= shouldInvalidate;
				scrollY->on_update_frame();

				if (scrollY->shouldInvalidate) {
					scrollY->shouldInvalidate = false;
					if (shouldInvalidate == false)
						uix->update_section(&scrollY->area);
				}
			}
		}

		currLayerId = nextLayerId;
	}
	if (overflow != Overflow::visible) godSheet->unclip();

	if (shouldInvalidate) {
		uix->unlock_screen(id);
		uix->update_section(&area);
		shouldInvalidate = false;
	}
}

void OUI::on_update_frame_raw() {
	if (uix == NULL) return;

	if (shouldInvalidate) {
		uix->lock_screen(id);
		canvas.clear_opaque(NULL, backgroundColor);
		on_update();
	}

	int currLayerId = 0, nextLayerId;
	bool cont = true;
	while (cont) {
		cont = false;
		nextLayerId = -1;
		for (auto* elem : elements) {
			if (elem->isVisible) {
				if (SHEET_OVERLAP(elem->area, *godSheet)) {
					if (elem->zIndex == currLayerId) {
						elem->shouldInvalidate |= shouldInvalidate;
						elem->on_update_frame();

						if (elem->shouldInvalidate) {
							elem->shouldInvalidate = false;
							if (shouldInvalidate == false)
								uix->update_section(&elem->area);
						}
					}
					else if (elem->zIndex > currLayerId) {
						cont = true;
						if (nextLayerId < 0) nextLayerId = elem->zIndex;
						else nextLayerId = Min(elem->zIndex, nextLayerId);
					}
				}
			}
		}

		if (currLayerId == 0 && isScrollable) {
			if (scrollX && scrollX->isVisible) {
				scrollX->shouldInvalidate |= shouldInvalidate;
				scrollX->on_update_frame();

				if (scrollX->shouldInvalidate) {
					scrollX->shouldInvalidate = false;
					if (shouldInvalidate == false)
						uix->update_section(&scrollX->area);
				}
			}

			if (scrollX && scrollY->isVisible) {
				scrollY->shouldInvalidate |= shouldInvalidate;
				scrollY->on_update_frame();

				if (scrollY->shouldInvalidate) {
					scrollY->shouldInvalidate = false;
					if (shouldInvalidate == false)
						uix->update_section(&scrollY->area);
				}
			}
		}

		currLayerId = nextLayerId;
	}

	if (shouldInvalidate) {
		uix->unlock_screen(id);
		uix->update_section(&area);
		shouldInvalidate = false;
	}
}

OUI* OUI::get_draggable(int x, int y, uint32_t flags) {
	return isDraggable && (parent ? parent->on_drag_start(this) : true) ? this : 0;
}

void OUI::apply_theme(bool bInvalidate) {
	set_background_color(OUITheme::primary);
	set_color(OUITheme::text);
	if (bInvalidate) invalidate();
}

OUI* OUI::create(int left, int top, int width, int height, OUI* parent, bool bAddToParent) {
	if (width < 0) left += width, width = -width;
	if (height < 0) top += height, height = -height;

	scrollX = new UIScroll();
	scrollY = new UIScroll();
	if (!scrollX || !scrollY) return this;
	isVisible = true;

	if (parent) {
		if (bAddToParent) parent->add_element(this);
		this->parent = parent;
		uix = parent->uix;
		godSheet = parent->godSheet;
		uix->add_element(this);
		fontName = parent->fontName;
	}

	canvas.set_sheet(godSheet);
	boxModel.left = left;
	boxModel.top = top;
	boxModel.width = width;
	boxModel.height = height;
	update_position();

	set_font_name(fontName);

	scrollX->create(0, 0, 10, 10, this, false);
	scrollY->create(0, 0, 10, 10, this, false);
	scrollY->mode = ScrollMode::Vertical;
	scrollX->mode = ScrollMode::Horizontal;

	get_content_area(contentArea);

	if (isScrollable) {
		if (scrollX && scrollX->isVisible)
			scrollX->on_parent_resize();
		if (scrollY && scrollY->isVisible)
			scrollY->on_parent_resize();
	}

	apply_theme(false);
	on_init();
	on_resize(contentArea.width, contentArea.height);
	calc_shape();
	isCreated = true;
	return this;
}

OUI* OUI::create(OUI* parent, bool bAddToParent) {
	return create(0, 0, 0, 0, parent, bAddToParent);
}

void OUI::get_parent_position(Rect& rect) {
	if (parent) {
		rect.set(parent->contentArea);
		rect.shift(parent->contextArea.left, parent->contextArea.top);
	}
}

void OUI::get_content_area(Rect& rcContent) {
	Spacing& pad = padding;
	Border& bor = border;
	rcContent.set(pad.left + bor.left, pad.top + bor.top,
		area.width - pad.left - pad.right - bor.left - bor.right - contextPadding.left - contextPadding.right,
		area.height - pad.top - pad.bottom - bor.top - bor.bottom - contextPadding.top - contextPadding.bottom);
}

void OUI::update_position() {
	Rect rect;
	get_parent_position(rect);
	area.set(boxModel.left + rect.left, boxModel.top + rect.top, boxModel.width, boxModel.height);
	contextArea.set(area);
	contextArea.left += contextPadding.left;
	contextArea.top += contextPadding.top;
	contextArea.width -= contextPadding.left + contextPadding.right;
	contextArea.height -= contextPadding.top + contextPadding.bottom;
	move_shape();

	iterateV(elements) {
		auto elem = *it;
		if (elem) elem->update_position();
	}

	if (isScrollable) {
		if (scrollX && scrollX->isVisible)
			scrollX->update_position();
		if (scrollY && scrollY->isVisible)
			scrollY->update_position();
	}

	if (menu) menu->update_position();
}

void OUI::move(int left, int top) {
	//if (boxModel.left == left && boxModel.top == top)
		//return;
	boxModel.left = left;
	boxModel.top = top;
	update_position();
	on_resize(contentArea.width, contentArea.height);
	invalidate();
}

void OUI::move_fast(int left, int top) {
	//if (boxModel.left == left && boxModel.top == top)
		//return;
	boxModel.left = left;
	boxModel.top = top;
	update_position();
	on_resize(contentArea.width, contentArea.height);
	invalidate();
}

void OUI::move(int left, int top, int width, int height) {
	//if (boxModel.left == left && boxModel.top == top && boxModel.width == width && boxModel.height == height)
		//return;
	if (boxModel.width == width && boxModel.height == height)
		return move(left, top);

	boxModel.left = left;
	boxModel.top = top;
	boxModel.width = width;
	boxModel.height = height;
	update_position();

	if (isScrollable) {
		if (scrollX && scrollX->isVisible)
			scrollX->on_parent_resize();
		if (scrollY && scrollY->isVisible)
			scrollY->on_parent_resize();
	}

	get_content_area(contentArea);
	on_resize(contentArea.width, contentArea.height);
	calc_shape();
	invalidate();
}

void OUI::move_fast(int left, int top, int width, int height) {
	boxModel.left = left;
	boxModel.top = top;
	boxModel.width = width;
	boxModel.height = height;
	update_position();

	if (isScrollable) {
		if (scrollX && scrollX->isVisible)
			scrollX->on_parent_resize();
		if (scrollY && scrollY->isVisible)
			scrollY->on_parent_resize();
	}

	get_content_area(contentArea);
	on_resize(contentArea.width, contentArea.height);
	calc_shape();
	invalidate();
}

void OUI::process_event(OUI* element, uint32_t message, uint64_t param, bool bubbleUp) {
	if (isScrollable) {
		if (message == Event::Scroll && (element == scrollY || element == scrollX)) {
			int dir = ((param) >> 16) & 0xffff;
			int p = param & 0xffff;
			auto scroll = (UIScroll*)element;
			scroll->set_handle_pos(p);
			p = scroll->get_pos();
			if (dir == 1) move_page_ver(p);
			else move_page_hor(p);
			return;
		}
	}

	if (bubbleUp && isVisible && parent)
		parent->process_event(element, message, param, true);
}

void OUI::measure_size(int* w, int* h) {
	if (w) *w = area.width;
	if (h) *h = area.height;
}

void OUI::set_parent(OUI* parent) {
	if (!parent || this->parent == parent) return;
	this->parent = parent;
	update_position();
}

void OUI::pop_up() {
	if (!parent || !uix) return;
	int width, height;
	measure_size(&width, &height);
	move(boxModel.left, boxModel.top, width, max(height, 20));
	show_window(true);
	uix->show_menu(this);
}

void OUI::fade() {
	if (!parent || !uix) return;
	show_window(false);
	uix->hide_menu(this);
	if (parent) parent->process_event(this, Event::Deselect, 0, true);
}

void OUI::show_window(bool bShow) {
	isVisible = bShow;
	invalidate();
}

void OUI::release_press() {
	isPressed = false;
	invalidate();
}

void OUI::enable(bool bEnable) {
	if (this->isEnabled == bEnable) return;
	this->isEnabled = bEnable;
	invalidate();
}

void push_back_elem(aoui& res, OUI* elem, bool nor, bool select, bool& bok) {
	if (select) bok = elem->isSelected;
	else bok = true;
	if (!nor && bok) res.push_back(elem);
}

void OUI::add_element(OUI* newElement) {
	elements.push_back(newElement);
}

void OUI::remove_element(OUI* element) {
	auto pos = std::find(elements.begin(), elements.end(), element);
	if (pos != elements.end()) {
		elements.erase(pos);
	}
}

inline bool OUI::check_transparency() {
	bool trans = false
		|| BYTE_NOT_OPAQUE(backgroundColor.a)
		|| !borderRadius.is_empty()
		|| ((border.left || border.top || border.right || border.bottom) &&
			(BYTE_NOT_OPAQUE(border.leftColor.a) ||
				BYTE_NOT_OPAQUE(border.topColor.a) ||
				BYTE_NOT_OPAQUE(border.rightColor.a) ||
				BYTE_NOT_OPAQUE(border.botColor.a)))
		|| outsetBoxShadows.size() > 0
		// to add a future criteria use ||
		|| (parent && parent->check_transparency());
	return trans;
}

void OUI::invalidate() {
	if (isVisible) {
		uix->invalidate_child();
		shouldInvalidate = true;
		if (parent && check_transparency())
			parent->invalidate();
	}
}

void OUI::set_id(uint32_t newId) {
	id = newId;
}

uint32_t OUI::get_id() {
	return id;
}

void OUI::focus() {
	if (isFocusable) {
		isActive = true;
		invalidate();
	}
	if (uix) uix->set_focused_element(isFocusable ? this : NULL);
}

void OUI::blur() {
	isActive = false;
	invalidate();
}

void OUI::set_timer(uint32_t nTimer, uint32_t nElapse) {
	if (uix) uix->set_timer(this, nTimer, nElapse);
}

void OUI::kill_timer(uint32_t nTimer) {
	if (uix) uix->kill_timer(this, nTimer);
}

void OUI::on_timer(uint32_t timer) {
}

OUI* OUI::get_capture() {
	return uix->get_capture();
}

void OUI::set_capture(OUI* element) {
	if (uix) uix->set_capture(element);
}

void OUI::release_capture() {
	if (uix) uix->release_capture();
}

void OUI::on_key_down(uint32_t key, uint32_t nrep, uint32_t flags) {
}

void OUI::on_mouse_enter(OUI* prev) {
	hover(true);
	if (menu && menu->menuActivationMode == MenuActivationMode::PointerHover) {
		menu->pop_up();
	}
	invalidate();
}

void OUI::on_mouse_leave(OUI* next) {
	hover(false);
	invalidate();
}

void OUI::reset_size() {
	iterateV(elements) {
		auto elem = *it;
		if (elem) elem->on_parent_resize();
	}

	if (isScrollable) {
		if (scrollX && scrollX->isVisible)
			scrollX->on_parent_resize();
		if (scrollY && scrollY->isVisible)
			scrollY->on_parent_resize();
	}

	get_content_area(contentArea);
	on_resize(contentArea.width, contentArea.height);
	calc_shape();
	invalidate();
}

bool OUI::select(bool bSelect) {
	isSelected = bSelect;
	invalidate();
	/*if (parent) {
		parent->reset_size();
		parent->on_parent_resize();
	}
	if (!isSelected) process_event(this, Event::Deselect, 0, true);*/
	return true;
}

void OUI::hover(bool isHover) {
	/*if (this->isHover && !isHover)
		this->on_mouse_leave(0);
	else if (!this->isHover && isHover)
		this->on_mouse_enter(0);
	else return;*/

	this->isHover = isHover;
	invalidate();
}

void OUI::set_background_color(Color backgroundColor) {
	this->backgroundColor.set(backgroundColor);
	invalidate();
}

void OUI::set_color(Color color) {
	this->color.set(color);
	canvas.art.textColor.set(color);
	invalidate();
}

void OUI::set_font_size(int fontSize) {
	this->fontSize = fontSize;
	canvas.load_handle(fontName, fontSize);
	invalidate();
}

void OUI::set_font_name(std::wstring fontName) {
	this->fontName = fontName;
	canvas.load_handle(fontName, fontSize);
	invalidate();
}

void OUI::set_text(std::wstring text) {
	this->text = text;
	invalidate();
}

bool OUI::is_within(int absX, int absY) {
	return area.is_inside(absX, absY);
	return false;
}

void OUI::set_border_radius(double lt, double rt, double rb, double lb) {
	borderRadius.set(lt, rt, rb, lb);
	calc_shape();
	invalidate();
}

void OUI::remove_border_radius() {
	borderRadius.set(0);
	calc_shape();
	invalidate();
}

void OUI::add_box_shadow(bool inset, int offsetX, int offsetY, int blur, int spread, Color color) {
	BoxShadow boxShadow;
	boxShadow.inset = inset;
	boxShadow.offsetX = offsetX;
	boxShadow.offsetY = offsetY;
	boxShadow.blur = blur;
	boxShadow.spread = spread;
	boxShadow.color = color;
	inset ? insetBoxShadows.push_back(boxShadow) : outsetBoxShadows.push_back(boxShadow);
	calc_shape();
	invalidate();
}

void OUI::clear_box_shadow() {
	for (auto& t : insetBoxShadows)
		t.sheet.free();
	for (auto& t : outsetBoxShadows)
		t.sheet.free();

	insetBoxShadows.clear();
	outsetBoxShadows.clear();
	insetBoxShadowSheet.destroy();
	outsetBoxShadowSheet.destroy();

	calc_shape();
	invalidate();
}

OUI* OUI::get_root() {
	auto p = this;
	while (p) {
		if (!p->parent) break;
		p = p->parent;
	}
	return p;
}

void OUI::set_overflow(Overflow overflow) {
	if (this->overflow == overflow) return;
	this->overflow = overflow;
	get_root()->invalidate();
}

void OUI::on_drag_enter(OUI* element) {
}

void OUI::on_drag_leave(OUI* element) {
}

bool OUI::on_drag_start(OUI* child) {
	return true;
}

void OUI::on_drag(OUI* child) {
	if (parent) parent->on_drag(child);
}

void OUI::on_drop(OUI* child) {
	if (parent) parent->on_drop(child);
}

bool OUI::can_drop(OUI* child) {
	return false;
}

bool OUI::focusable() {
	return isFocusable && is_child_visible(this);
}

bool OUI::is_child_visible(OUI* child) {
	if (!parent || !isVisible) return isVisible;
	return parent->is_child_visible(this);
}

void OUI::move_page(int left, int top) {
	if (scrollY) scrollY->set_pos(top);
	if (scrollX) scrollX->set_pos(left);
	if (scrollX || scrollY) invalidate();
}

void OUI::move_page_ver(int top) {
	if (scrollY) {
		scrollY->set_pos(top);
		invalidate();
	}
}

void OUI::move_page_hor(int left) {
	if (scrollX) {
		scrollX->set_pos(left);
		invalidate();
	}
}

void OUI::on_init() {

}

void OUI::on_resize(int width, int height) {

}

void OUI::on_parent_resize() {

}

void OUI::on_destroy() {

}

void OUI::on_window_closed(UIWindow* window, size_t wmsg) {

}

void OUI::delete_elements() {
	for (auto elem : elements) {
		if (elem) {
			elem->delete_elements();
			if (uix) uix->remove_element(elem);
		}
	}
	for (auto& bx : insetBoxShadows) {
		bx.sheet.destroy();
	}
	for (auto& bx : outsetBoxShadows) {
		bx.sheet.destroy();
	}

	insetBoxShadows.clear();
	outsetBoxShadows.clear();
	insetBoxShadowSheet.destroy();
	outsetBoxShadowSheet.destroy();
}