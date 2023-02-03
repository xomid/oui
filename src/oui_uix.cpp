#include "oui_uix.h"
#include "oui_menu.h"
#include <windowsx.h>

// UIX platform specific methods

bool UIX::is_window_visible() const {
	return IsWindowVisible(hWnd);
}

void UIX::set_title(std::wstring title)
{
	if (hWnd)
		SetWindowTextW(hWnd, title.c_str());
}

void UIX::invalidate_child() {
	if (shouldUpdate) return;
	shouldUpdate = true;
}

#ifdef _WIN32
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto uix = (UIX*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (uMsg) {
	case WM_CREATE:
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		int* userdata = reinterpret_cast<int*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)userdata);
	}
	break;

	case WM_ERASEBKGND:
		uix->OnEraseBkgnd(0);
		break;

	case WM_PAINT:
		uix->OnPaint();
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		uix->destroy();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
	{
		size_t width = LOWORD(lParam);
		size_t height = HIWORD(lParam);
		uix->OnSize(width, height);
	}
	break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	{
		auto vkCode = LOWORD(wParam);
		auto nRepCount = LOWORD(lParam);
		auto nFlags = HIWORD(lParam);

		switch (uMsg)
		{
		case WM_KEYDOWN:
			uix->OnKeyDown(vkCode, nRepCount, nFlags);
			break;
		case WM_KEYUP:
			uix->OnKeyUp(vkCode, nRepCount, nFlags);
			break;
		}
	}
	break;

	case WM_MOUSEWHEEL:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	{
		auto xPos = GET_X_LPARAM(lParam);
		auto yPos = GET_Y_LPARAM(lParam);

		switch (uMsg) {
		case WM_MOUSEMOVE:
			uix->OnMouseMove((uint32_t)wParam, xPos, yPos);
			break;
		case WM_LBUTTONDOWN:
		{
			uix->PreLButtonDown(xPos, yPos, (uint32_t)wParam);
		}
		break;
		case WM_LBUTTONUP:
			uix->OnLButtonUp((uint32_t)wParam, xPos, yPos);
			break;
		case WM_MOUSEWHEEL:
		{
			auto fwKeys = GET_KEYSTATE_WPARAM(wParam);
			auto zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			uix->OnMouseWheel(fwKeys, zDelta, xPos, yPos);
		}
		break;
		}
	}
	break;
	case WM_SHOWWINDOW:
		uix->invalidate();
		uix->update();
		return 0;
	default:
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

UIX::UIX()
	:
	title(L"Window"),
	m_hInstance(GetModuleHandleW(nullptr)),
	bit(NULL),
	hdc(NULL),
	displayDC(NULL),
	capturedElement(NULL),
	currentElementHovering(NULL),
	hWnd(NULL),
	hTimer(NULL),
	container(NULL),
	currDialog(NULL),
	hTimerQueue(NULL),
	activeWindow(NULL),
	lastDragHoverElement(NULL),
	focusedElementID(-1),
	id(-1),
	last_time(0),
	lockerID(-2),
	lXPos(0),
	lYPos(0),
	visibleWindows(0),
	countBuffer(0),
	dragZIndex(0),
	bAsyncTimer(false),
	bDrawBoxModel(false),
	bDrawMagnifier(false),
	terminate(false),
	terminated(true),
	shouldInvalidate(false),
	shouldUpdate(false),
	lastLButtonDownTime(0)
{

	WNDCLASSW wndClass = {};
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.lpfnWndProc = WindowProc;

	RegisterClassW(&wndClass);

	style = WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_SYSMENU;
	style &= ~WS_VISIBLE;
}

UIX::~UIX()
{
	UnregisterClassW(CLASS_NAME, m_hInstance);
}

void UIX::move(Rect* rc) {
	if (!rc || !IsWindow(hWnd)) return;

	RECT rect;
	rect.left = rc->left;
	rect.top = rc->top;
	rect.right = rect.left + rc->width;
	rect.bottom = rect.top + rc->height;

	AdjustWindowRect(&rect, style, false);

	MoveWindow(hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
}

bool UIX::create(Rect* rc) {

	int width = 640;
	int height = 480;

	RECT rect;
	rect.left = 250;
	rect.top = 250;

	if (rc) {
		width = rc->width;
		height = rc->height;
		rect.left = rc->left;
		rect.top = rc->top;
	}

	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, style, false);

	hWnd = CreateWindowExW(
		0,
		CLASS_NAME,
		title.c_str(),
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		m_hInstance,
		this
	);

	if (!IsWindow(hWnd))
		return false;

	displayDC = ::GetDC(hWnd);
	last_time = clock();
	link_bitmap(width, height, OUI_BGR);
	resize(width, height);

	// init magnifier
	magnifier.uix = this;
	magnifier.godSheet = &sheet;
	magnifier.create(0, 0, 100, 100, 0, false);
	magnifier.show_window(false);

	OUI::init();

	return hdc && bit;
}

void UIX::show_window(bool show) {
	container->invalidate();
	ShowWindow(hWnd, show ? SW_SHOW : SW_HIDE);
}

bool UIX::ProcessMessages()
{
	MSG msg = {};

	while (PeekMessageW(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			return false;
		}
		PreTranslateMessage(&msg);
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return true;
}

void UIX::OnIdle() {
	if (toDelete.size()) {
		for (auto it = toDelete.begin(); it != toDelete.end(); ++it) {
			auto elem = *it;
			if (elem) {
				delete elem;
			}
			*it = 0;
		}

		toDelete.clear();
		toDelete.shrink_to_fit();
	}

	if (timers.size()) {
		for (auto tm : timers) {
			if (tm->killed) continue;
			clock_t t = clock();
			__int64 a = t - tm->last;
			while (a >= tm->elapse) {
				if (tm->killed || !tm->element) break;
				a -= tm->elapse;
				tm->last += tm->elapse;
				tm->element->on_timer(tm->nTimer);
			}
		}
		for (auto it = timers.begin(); it != timers.end();) {
			auto tm = *it;
			if (tm->killed || !tm->element) {
				delete* it;
				it = timers.erase(it);
			}
			else ++it;
		}
	}

	update();
}

void UIX::set_timer(OUI* element, uint32_t nTimer, uint32_t nElapse) {
	if (!element) return;
	nElapse = max(nElapse, 1);
	uint32_t id = (uint32_t)TOUINT2(element->get_id(), nTimer);
	timer* tm = NULL;
	for (auto _tm : timers) {
		if (_tm->nTimer == nTimer && _tm->element == element) {
			tm = _tm;
			break;
		}
	}
	if (!tm) timers.push_back(new timer(element, nTimer, nElapse, id));
	else {
		tm->elapse = nElapse;
		tm->killed = false;
		tm->last = clock();
		tm->nTimer = nTimer;
		tm->element = element;
		tm->id = id;
	}
}

void UIX::kill_timer(OUI* element, uint32_t nTimer) {
	for (auto tm : timers) {
		if (tm->element == element && tm->nTimer == nTimer) {
			tm->killed = true;
			break;
		}
	}
}

void UIX::destroy() {
	terminate = true;
	if (container) container->on_destroy();
}

BOOL UIX::OnEraseBkgnd(HDC pDC)
{
	if (!container) return 1;
	auto d = displayDC;
	displayDC = pDC;
	invalidate();
	update();
	displayDC = d;
	return 0;
}

void UIX::OnSize(size_t width, size_t height)
{
	if (!::IsWindow(hWnd)) return;
	RECT rc;
	GetClientRect(hWnd, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;
	resize(width, height);
	update();
}

void UIX::PreLButtonDown(int x, int y, uint32_t param) {
	auto prev = lastLButtonDownTime;
	lastLButtonDownTime = clock();
	auto delta = lastLButtonDownTime - prev;
	if (prev && delta <= (long)GetDoubleClickTime() && x == lXPos && y == lYPos) {
		OnLButtonDblClk(param, x, y);
	}
	else {
		lXPos = x;
		lYPos = y;
		OnLButtonDown(param, x, y);
	}
}

void UIX::OnPaint()
{
	PAINTSTRUCT ps;
	auto prevHDC = hdc;
	HDC hdc = BeginPaint(hWnd, &ps);
	this->hdc = hdc;
	update();
	EndPaint(hWnd, &ps);
	this->hdc = prevHDC;
}

void UIX::OnTimer(uint32_t nEventID) {
	if (!bAsyncTimer) {
		//for (auto& x : timers) {
			//for (auto it : x.second) {
				//timer& tm = *it.second;
				//if (tm.id == nEventID)
					//tm.element->on_timer(tm.nTimer);
			//}
		//}
		//update();
	}
}

void UIX::OnMouseMove(uint32_t nFlags, int x, int y)
{
	cursorPos.set(x, y);
	if (!container) return;
	bool locked = is_locked();

	if (bDrawMagnifier) {
		int l = x - (magnifier.boxModel.width >> 1);
		int t = y - (magnifier.boxModel.height >> 1);
		magnifier.move(l, t);
	}

	for (auto it = menus.begin(); it != menus.end();) {
		UIMenu* menu = (UIMenu*)it->second;
		if (!menu->area.is_inside(x, y) && !menu->parent->area.is_inside(x, y) && menu->menuType == MenuType::Fluid) {
			it = menus.erase(it);
			menu->show_window(false);
			if (menu->parent) menu->parent->process_event(menu, Event::Deselect, 0, true);
			continue;
		}
		it++;
	}

	if (capturedElement) {
		if (dragMan.mouseAction == MouseDragStatus::STRATED) {
			int left = capturedElement->boxModel.left, top = capturedElement->boxModel.top;
			dragMan.drag(x, y, left, top);
			{
				capturedElement->boxModel.left = left;
				capturedElement->boxModel.top = top;
				capturedElement->update_position();
				capturedElement->invalidate();
			}
			capturedElement->on_drag(capturedElement);
			auto dst = OUI::find_element(container, x, y);
			if (dst != lastDragHoverElement) {
				if (lastDragHoverElement) {
					lastDragHoverElement->on_drag_leave(capturedElement);
					lastDragHoverElement = 0;
				}
				if (dst && dst != capturedElement) {
					lastDragHoverElement = dst;
					dst->on_drag_enter(capturedElement);
				}
			}
			container->invalidate();
		}
		else capturedElement->on_mouse_move(TORELX(x, capturedElement->area), TORELY(y, capturedElement->area), nFlags);
	}
	else {
		OUI* curr = NULL, * currMenu = NULL, * currWindow = NULL;

		if (locked) {
			for (auto& it : menus) {
				UIMenu* menu = (UIMenu*)it.second;
				if (menu->area.is_inside(x, y) && menu->isVisible) {
					curr = currMenu = OUI::find_element(menu, x, y);
					break;
				}
			}

			if (!curr) {
				if (currDialog->area.is_inside(x, y))
					curr = currWindow = OUI::find_element(currDialog, x, y);
				else curr = container;
			}
		}
		else
		{
			for (auto& it : menus) {
				UIMenu* menu = (UIMenu*)it.second;
				if (menu->area.is_inside(x, y) && menu->isVisible) {
					curr = currMenu = OUI::find_element(menu, x, y);
					break;
				}
			}

			for (int i = (int)windows.size() - 1; i > -1; --i) {
				auto* win = windows[i];
				if (win->area.is_inside(x, y) && win->isVisible) {
					curr = currWindow = OUI::find_element(win, x, y);
					break;
				}
			}
		}

		if (!curr) curr = OUI::find_element(container, x, y);
		if (curr) {
			if (!currentElementHovering || curr->get_id() != currentElementHovering->get_id()) {
				if (currentElementHovering) currentElementHovering->on_mouse_leave(curr);
				curr->on_mouse_enter(currentElementHovering);
				currentElementHovering = curr;
			}
		}

		if (locked) {
			if (currentElementHovering)
				currentElementHovering->on_mouse_move(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
		}
		if (currentElementHovering) currentElementHovering->on_mouse_move(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
	}

	update();
}

void UIX::OnLButtonDown(uint32_t nFlags, int x, int y)
{
	if (!container) return;
	bool locked = is_locked();

	for (auto it = menus.begin(), end = menus.end(); it != end; ) {
		UIMenu* menu = (UIMenu*)it->second;
		if (!menu->area.is_inside(x, y) && !menu->parent->area.is_inside(x, y)) {
			menu->show_window(false);
			it = menus.erase(it);
			if (menu->parent)
				menu->parent->process_event(menu, Event::Deselect, 0, true);
			invalidate();
			continue;
		}
		it++;
	}

	if (!locked) {
		OUI* node = currentElementHovering;
		while (visibleWindows > 0 && node) {
			if (windowsIndex.count((UIWindow*)node)) {
				select_window(windowsIndex[(UIWindow*)node]);
				break;
			}
			node = node->parent;
		}
	}

	OUI* dragElem = 0;
	if (currentElementHovering &&
		(dragElem = currentElementHovering->get_draggable(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags))) {
		currentElementHovering = dragElem;
		dragMan.startDragging(x, y, currentElementHovering->boxModel.left, currentElementHovering->boxModel.top);
		capturedElement = currentElementHovering;
		dragZIndex = currentElementHovering->zIndex;
		currentElementHovering->zIndex = 1;
	}

	if (capturedElement) {
		capturedElement->on_mouse_down(TORELX(x, capturedElement->area), TORELY(y, capturedElement->area), nFlags);
	}
	else {
		//OUI* currMenu = NULL, * currWindow = NULL;
		//if (locked) {
		//	if (currDialog->area.is_inside(x, y)) {
		//		currWindow = OUI::find_element(currDialog, x, y);
		//		if (currWindow)
		//			currWindow->on_mouse_down(TORELX(x, currWindow->area), TORELY(y, currWindow->area), nFlags);
		//	}
		//}
		//else
		//{
		//	for (auto it = menus.begin(); it != menus.end(); it++) {
		//		UIMenu* menu = (UIMenu*)it->second;
		//		if (menu->area.is_inside(x, y) && menu->isVisible) {
		//			currMenu = menu;
		//			break;
		//		}
		//	}

		//	for (int i = (int)windows.size() - 1; i > -1; --i) {
		//		auto* win = windows[i];
		//		if (win->area.is_inside(x, y) && win->isVisible) {
		//			currWindow = win;
		//			break;
		//		}
		//	}

		//	/*if (currMenu) currentElementHovering->on_mouse_down(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
		//	else if (currWindow) currentElementHovering->on_mouse_down(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
		//	else */

		//}

		if (currentElementHovering) currentElementHovering->on_mouse_down(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
	}

	update();
}

void UIX::OnLButtonUp(uint32_t nFlags, int x, int y)
{
	if (!container) return;
	bool b = dragMan.mouseAction == MouseDragStatus::STRATED;
	if (b && currentElementHovering) {
		if (lastDragHoverElement != currentElementHovering->parent &&
			lastDragHoverElement != container && lastDragHoverElement &&
			lastDragHoverElement->can_drop(currentElementHovering)) {
			currentElementHovering->on_drop(0);
			currentElementHovering->parent = lastDragHoverElement;
			currentElementHovering->zIndex = dragZIndex;
			lastDragHoverElement->on_drop(currentElementHovering);
			lastDragHoverElement->on_drag_leave(0);
		}
		else {
			currentElementHovering->zIndex = dragZIndex;
			currentElementHovering->on_drop(currentElementHovering);
		}
		lastDragHoverElement = NULL;
		currentElementHovering = NULL;
		capturedElement = NULL;
	}
	dragMan.stopDragging();
	bool locked = is_locked();

	if (capturedElement) {
		capturedElement->on_mouse_up(TORELX(x, capturedElement->area), TORELY(y, capturedElement->area), nFlags);
		if (b) capturedElement = NULL;
	}
	else {
		//OUI* currMenu = NULL, * currWindow = NULL;

		//if (locked) {
		//	if (currDialog->area.is_inside(x, y)) {
		//		currWindow = OUI::find_element(currDialog, x, y);
		//		if (currWindow && currentElementHovering)
		//			currentElementHovering->on_mouse_up(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
		//	}
		//}
		//else
		//{
		//	for (auto it = menus.begin(); it != menus.end(); it++) {
		//		UIMenu* menu = (UIMenu*)it->second;
		//		if (menu->area.is_inside(x, y) && menu->isVisible) {
		//			currMenu = menu;
		//			break;
		//		}
		//	}

		//	for (int i = (int)windows.size() - 1; i > -1; --i) {
		//		auto* win = windows[i];
		//		if (win->area.is_inside(x, y) && win->isVisible) {
		//			currWindow = win;
		//			break;
		//		}
		//	}

		//	//if (currMenu) currentElementHovering->on_mouse_up(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
		//	//else if (currWindow) currentElementHovering->on_mouse_up(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
		//	//else 
		//}

		if (currentElementHovering) currentElementHovering->on_mouse_up(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
	}

	update();
}

void UIX::OnLButtonDblClk(uint32_t nFlags, int x, int y)
{
	if (!container) return;
	bool locked = is_locked();

	if (!locked) {
		for (auto it = menus.begin(); it != menus.end(); ) {
			UIMenu* menu = (UIMenu*)it->second;
			if (!menu->area.is_inside(x, y) && !menu->parent->area.is_inside(x, y) && menu->menuType == MenuType::Solid) {
				menu->fade();
				if (menus.count(menu) < 1) {
					it = menus.begin();
					continue;
				}
			}
			it++;
		}
	}

	if (capturedElement) {
		capturedElement->on_dbl_click(TORELX(x, capturedElement->area), TORELY(y, capturedElement->area), nFlags);
	}
	else {
		OUI* currMenu = NULL, * currWindow = NULL;
		if (locked) {
			if (currDialog->area.is_inside(x, y)) {
				currWindow = OUI::find_element(currDialog, x, y);
				if (currWindow && currentElementHovering)
					currentElementHovering->on_dbl_click(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
			}
		}
		else
		{
			for (auto it = menus.begin(); it != menus.end(); it++) {
				UIMenu* menu = (UIMenu*)it->second;
				if (menu->area.is_inside(x, y) && menu->isVisible) {
					currMenu = menu;
					break;
				}
			}

			for (int i = (int)windows.size() - 1; i > -1; --i) {
				auto* win = windows[i];
				if (win->area.is_inside(x, y) && win->isVisible) {
					currWindow = win;
					break;
				}
			}

			/*if (currMenu) currentElementHovering->on_dbl_click(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
			else if (currWindow) currentElementHovering->on_dbl_click(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
			else */
			if (currentElementHovering) currentElementHovering->on_dbl_click(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), nFlags);
		}
	}

	update();
}

BOOL UIX::OnMouseWheel(uint32_t nFlags, short zDelta, int x, int y)
{
	if (!container) return true;
	RECT rc;
	GetWindowRect(hWnd, &rc);
	x -= rc.left;
	y -= rc.top;

	if (capturedElement) {
		capturedElement->on_mouse_wheel(TORELX(x, capturedElement->area), TORELY(y, capturedElement->area), zDelta, nFlags);
	}
	else {
		OUI* currMenu = NULL, * currWindow = NULL;
		bool locked = is_locked();
		if (locked) {
			if (currDialog->area.is_inside(x, y)) {
				currWindow = OUI::find_element(currDialog, x, y);
				if (currWindow)
					currentElementHovering->on_mouse_wheel(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), zDelta, nFlags);
			}
		}
		else
		{
			for (auto it = menus.begin(); it != menus.end(); it++) {
				UIMenu* menu = (UIMenu*)it->second;
				if (menu->area.is_inside(x, y) && menu->isVisible) {
					currMenu = menu;
					break;
				}
			}

			for (int i = (int)windows.size() - 1; i > -1; --i) {
				auto* win = windows[i];
				if (win->area.is_inside(x, y) && win->isVisible) {
					currWindow = win;
					break;
				}
			}

			if (currMenu) currentElementHovering->on_mouse_wheel(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), zDelta, nFlags);
			else if (currWindow) currentElementHovering->on_mouse_wheel(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), zDelta, nFlags);
			else if (currentElementHovering) currentElementHovering->on_mouse_wheel(TORELX(x, currentElementHovering->area), TORELY(y, currentElementHovering->area), zDelta, nFlags);
		}
	}
	update();
	return true;
}

BOOL UIX::PreTranslateMessage(LPMSG msg)
{
	auto wParam = msg->wParam;
	auto lParam = msg->lParam;
	if (msg->message == WM_LBUTTONDBLCLK) {
		auto xPos = GET_X_LPARAM(lParam);
		auto yPos = GET_Y_LPARAM(lParam);
		OnLButtonDblClk((uint32_t)wParam, xPos, yPos);
		return 0;
	}

	return 1;
}

void UIX::OnKeyUp(uint32_t nChar, uint32_t nRepCnt, uint32_t nFlags)
{
	if (nChar == VK_TAB) {
		if (::GetKeyState(VK_SHIFT) & 0x8000) focus_prev();
		else focus_next();
	}

	update();
}

void UIX::OnKeyDown(uint32_t nChar, uint32_t nRepCnt, uint32_t nFlags)
{
	if (focusedElementID > -1 && focusedElementID < int(focusables.size())) {
		focusables[focusedElementID]->on_key_down(nChar, nRepCnt, nFlags);
	}

	update();
}

void UIX::update() {
	if (!shouldUpdate) return;

#ifdef DEBUG_UPDATEFRAME
#endif
	clock_t b, e;
	b = clock();

	if (!container) {
		shouldInvalidate = false;
		return;
	}

	countBuffer = 0;
	bool drawMenus = menus.size() > 0;
	if (visibleWindows < 0) throw;
	bool drawWindows = visibleWindows > 0;
	bool dragging = dragMan.mouseAction == MouseDragStatus::STRATED;
	if (!shouldInvalidate && (bDrawBoxModel || bDrawMagnifier || drawMenus || drawWindows || dragging)) shouldInvalidate = true;
	strbuffer = "";

	if (shouldInvalidate)
		container->shouldInvalidate = true;
	if (shouldInvalidate) {
		lockerID = -1;
	}
	container->on_update_frame_raw();
	if (drawWindows) {
		for (auto* window : windows) {
			if (window && window->isVisible) {
				if (SHEET_OVERLAP(window->area, sheet)) {
					window->shouldInvalidate |= shouldInvalidate;
					window->on_update_frame();
				}
			}
		}
	}
	if (drawMenus) {
		for (auto it : menus) {
			auto menu = it.second;
			if (SHEET_OVERLAP(menu->area, sheet)) {
				if (menu && menu->isVisible) {
					menu->shouldInvalidate |= shouldInvalidate;
					menu->on_update_frame();
				}
			}
		}
	}

	if (container->shouldInvalidate) {
		container->shouldInvalidate = false;
		if (shouldInvalidate == false) update_section(&container->area);
	}

	if (shouldInvalidate) {
		lockerID = -2;
		shouldInvalidate = false;
	}

	if (bDrawBoxModel && currentElementHovering && currentElementHovering != container) {
		OUI* e = currentElementHovering;
		Canvas can(&container->area, &sheet);
		can.draw_box_model(&e->area,
			&e->margin,
			&e->padding,
			&e->border);

		std::string s = '#' + std::to_string(e->get_id()),
			sw = std::to_string(e->area.width),
			sh = std::to_string(e->area.height),
			cls = "";

		std::wstring fontName = InitialValues::fontName;
		can.load_handle(fontName, InitialValues::fontSize);
		auto box = can.get_box8((char*)s.c_str(), s.length());
		int tw = box->get_width() + 10;
		int th = box->get_height() + 10;
		Rect rc(e->area.left + (e->area.width - tw) / 2,
			e->area.top - th - 15,
			tw, th);

		rc.left = CLAMP3(0, rc.left, container->area.width - rc.width);
		if (rc.top < 0) rc.top = e->area.top + 15;
		else if (rc.top >= container->area.height - rc.height)
			rc.top = e->area.top - rc.height - 15;

		std::vector<ColoredSpan> spans = {
			ColoredSpan(Color("#58854a"), 0, int(s.length()))
		};

		can.fill_rounded_rect(&rc, 2, &Color(57, 57, 57));
		can.art.alignX = Align::CENTER;
		can.art.alignY = Align::CENTER;
		can.art.textColor.set(0xff, 0, 0);
		can.draw_text8((char*)s.c_str(), rc, &spans);
	}

	if (bDrawMagnifier) {
		magnifier.shouldInvalidate = true;
		magnifier.on_update();
	}

	update_section(&container->area);
	shouldUpdate = false;

	e = clock() - b;
	strbuffer = std::to_string(e);
	SetWindowTextA(hWnd, strbuffer.c_str());

#ifdef DEBUG_UPDATEFRAME
	/*e = clock() - b;
	strbuffer = "";
	if (watchList.size()) {
		strbuffer = " [";
		for (auto var : watchList) {
			strbuffer += "'" + var->get_name() + "':" + var->get_value() + ", ";
		}
		strbuffer[strbuffer.length() - 1] = ']';
	}
	strbuffer = std::to_string(countBuffer) + " items, " + (shouldInvalidate ? "UIX(1)" : "UIX(0)") + strbuffer;
	sprintf_s(ocom::buff, sizeof(ocom::buff), "%dms | ", e);
	strbuffer = std::string(ocom::buff) + strbuffer;
	SetWindowTextA(hWnd, strbuffer.c_str());*/
#endif
}

bool UIX::pop_up_dialog(UIWindow* newWindow) {
	auto* cont = get_container();
	if (is_locked() || IS_NULL(newWindow) || IS_NULL(cont)) return false;
	if (newWindow->isVisible) {
		currDialog = newWindow;
		int l, t, w, h;
		w = currDialog->boxModel.width;
		h = currDialog->boxModel.height;
		l = (cont->boxModel.width - w) / 2;
		t = (cont->boxModel.height - h) / 2;
		currDialog->move(l, t, w, h);
		select_window(currDialog);
		return true;
	}
	else {
		currDialog = NULL;
		invalidate();
		update();
		return false;
	}
}

void UIX::select_window(UIWindow* activeWindow) {
	size_t curr = 0;
	for (auto* x : windows) {
		if (x != activeWindow) {
			windows[curr++] = x;
		}
	}
	windows.back() = activeWindow;
}

bool UIX::is_locked() {
	return currDialog && currDialog->isVisible;
}

void UIX::add_window(UIWindow* newWindow) {
	if (!newWindow) return;
	windowsIndex[newWindow] = newWindow;
	windows.push_back(newWindow);
	add_element(newWindow);
}

void UIX::link_bitmap(int iw, int ih, int nBits)
{
	BITMAPINFO i;
	DIBSECTION dibsection;
	int nBytes;

	dibsection = { 0 };
	i.bmiHeader.biBitCount = nBits * 8;
	i.bmiHeader.biCompression = BI_RGB;
	i.bmiHeader.biHeight = -ih;
	i.bmiHeader.biWidth = iw + 1;
	i.bmiHeader.biSize = sizeof(BITMAPINFO);
	i.bmiHeader.biPlanes = 1;
	i.bmiHeader.biSizeImage = iw * ih * nBits / 8;

	pyte data = NULL;

	if (hdc = CreateCompatibleDC(0)) {
		bit = CreateDIBSection(hdc, &i, DIB_RGB_COLORS, (void**)&data, 0, 0);
		if (bit) {
			SelectObject(hdc, bit);
			nBytes = ::GetObjectA(bit, sizeof(DIBSECTION), &dibsection);
			if (iw >= dibsection.dsBm.bmWidth || ih > dibsection.dsBm.bmHeight) {
				iw = ih = 0;
				data = NULL;
			}
		}
	}

	int pitch = Max(0, dibsection.dsBm.bmWidthBytes);
	sheet.attach(data, iw, ih, pitch, dibsection.dsBm.bmBitsPixel / 8);
}

void UIX::lock_screen(int lockerID) {
	if (this->lockerID > -2 || lockerID < -2) return;
	this->lockerID = lockerID;
}

void UIX::unlock_screen(int lockerID) {
	if (lockerID < -2 || this->lockerID != lockerID) return;
	this->lockerID = -2;
}

void UIX::set_container(OUI* newContainer) {
	if (!newContainer) return;
	int w, h;
	RECT rc;
	GetClientRect(hWnd, &rc);
	w = rc.right - rc.left;
	h = rc.bottom - rc.top;
	container = newContainer;
	container->uix = this;
	container->godSheet = &sheet;
	container->create(0, 0, w, h, NULL);
	container->set_id(0);
	container->reset_size();
}

void UIX::resize(size_t newWidth, size_t newHeight) {
	if (sheet.data) {
		::SelectObject(hdc, bit);
		::DeleteObject(bit);
		::DeleteObject(hdc);
		sheet.data = NULL;
		sheet.sw = sheet.w = sheet.sh = sheet.h = 0;
	}

	link_bitmap((int)newWidth, (int)newHeight, OUI_BGR);
	if (container) container->move(0, 0, (int)newWidth, (int)newHeight);

	invalidate();
}

void UIX::update_section(Rect* rc) {
	if (lockerID > -2) return;
	if (displayDC) BitBlt(displayDC, rc->left, rc->top, rc->width, rc->height, hdc, rc->left, rc->top, SRCCOPY);
}

void addElem(OUI* curr, aoui* res, bool bfirst = false)
{
	if (!curr) return;
	iterateV(curr->elements) {
		auto elem = *it;
		addElem(elem, res);
	}

	if (!bfirst) res->push_back(curr);
}

void UIX::update_elements_list() {
	elements.clear();
	addElem(container, &elements, true);
}

OUI* UIX::get_container() {
	return container;
}

void UIX::add_element(OUI* newElement) {
	if (!newElement) return;
	newElement->set_id((uint32_t)(elements.size() + 1U));
	elements.push_back(newElement);

	if (newElement->isFocusable) {
		add_focusable(newElement);
		if (focusedElementID < 0)
			newElement->focus();
	}

	newElement->on_parent_resize();
}

void UIX::remove_element(OUI* element) {
	if (!element) return;
	auto it = std::find(elements.begin(), elements.end(), element);
	if (it != elements.end()) elements.erase(it);
	it = std::find(focusables.begin(), focusables.end(), element);
	if (it != focusables.end()) focusables.erase(it);
	if (element == currentElementHovering) currentElementHovering = NULL;
	if (element == lastDragHoverElement) lastDragHoverElement = NULL;
	if (element == capturedElement) {
		currentElementHovering = NULL;
		dragMan.stopDragging();
	}
	for (auto it = timers.begin(); it != timers.end(); ) {
		auto tm = *it;
		if (tm->element == element) {
			it = timers.erase(it);
		}
		else ++it;
	}
}

void UIX::add_focusable(OUI* newElement) {
	if (!newElement) return;
	auto it = std::find(focusables.begin(), focusables.end(), newElement);
	if (it == focusables.end()) {
		auto sz = (int)focusables.size();
		size_t place = 0;
		if (sz == 1) {
			if (focusables.back()->get_id() < newElement->get_id())
				place = 1;
		}
		else if (sz >= 2) {
			auto id = newElement->get_id();
			int low, mid, high;
			low = 0;
			high = sz;
			while (low < high) {
				mid = (low + high) >> 1;
				auto a = focusables[mid]->get_id();
				if (a < id) low = mid + 1;
				else if (a > id) high = mid;
				else break;
			}
			place = low;
		}
		focusables.insert(focusables.begin() + place, newElement);
	}
}

void UIX::remove_focusable(OUI* element) {
	if (!element) return;
	auto it = std::find(focusables.begin(), focusables.end(), element);
	if (it != focusables.end()) focusables.erase(it);
}

void UIX::delete_element(OUI* element) {
	if (!element) return;
	remove_element(element);
	auto it = std::find(toDelete.begin(), toDelete.end(), element);
	element->delete_elements();
	toDelete.push_back(element);
}

void UIX::set_capture(OUI* element) {
	if (element == container) return;
	if (capturedElement != NULL) return;
	capturedElement = element;
	SetCapture(hWnd);
}

OUI* UIX::get_capture() {
	return capturedElement;
}

void UIX::release_capture() {
	capturedElement = NULL;
	ReleaseCapture();
}

bool UIX::set_focused_element(OUI* focusedElem) {
	for (auto elem : focusables)
		if (elem->isActive && elem != focusedElem) elem->blur();
	focusedElementID = -1;
	iterateI(focusables.size()) {
		if (focusables[i] == focusedElem) {
			focusedElementID = (int)i;
			break;
		}
	}

	/*if (focusedElementID > -1 && focusedElementID < int(focusables.size())) {
		if (focusedElem != focusables[focusedElementID])
			focusables[focusedElementID]->blur();
	}*/

	/*iterateI(focusables.size()) {
		if (focusables[i] == focusedElem) {
			focusedElementID = i;
			break;
		}
	}*/

	return true;
}

void UIX::copy_to_clipboard(std::wstring str) {
	if (str.length()) {
		if (OpenClipboard(hWnd)) {
			auto len = str.length();
			HANDLE res = GlobalAlloc(GMEM_MOVEABLE, SIZE_T((len + 1) * sizeof(WCHAR)));

			if (res) {
				WCHAR* newText = (WCHAR*)GlobalLock(res);
				for (int i = 0; i < len; i++) {
					newText[i] = str[i];
					//wcscpy_s(newText, len, str.c_str());
				}

				newText[len] = '\0';
				GlobalUnlock(res);

				EmptyClipboard();
				if (!SetClipboardData(CF_UNICODETEXT, res))
					GlobalFree(res);
			}

			CloseClipboard();
		}
	}
}

std::wstring UIX::copy_from_clipboard() {

	std::wstring str = L"";

	if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		if (OpenClipboard(hWnd)) {
			HANDLE clip = GetClipboardData(CF_UNICODETEXT);
			if (clip) {
				WCHAR* text = (WCHAR*)GlobalLock(clip);
				auto len = wcslen(text);
				HANDLE res = GlobalAlloc(GMEM_MOVEABLE, SIZE_T((len + 1) * sizeof(WCHAR)));

				if (res) {
					WCHAR* newText = (WCHAR*)GlobalLock(res);
					for (int i = 0; i < len; i++) {
						newText[i] = text[i];
					}
					newText[len] = '\0';
					str = newText;

					GlobalUnlock(res);
					GlobalUnlock(clip);
					GlobalFree(res);
				}
				else GlobalUnlock(clip);
			}
			CloseClipboard();
		}
	}

	return str;
}

void UIX::show_menu(OUI* menu) {
	if (!menu) return;
	menus[menu] = menu;
	menu->invalidate();
}

void UIX::hide_menu(OUI* menu) {
	if (!menu || menus.count(menu) < 1) return;
	menus.erase(menu);
	invalidate();
}

void UIX::invalidate() {
	shouldInvalidate = true;
}

wchar_t UIX::convert_char(uint32_t key) {
	GUITHREADINFO Gti;
	::ZeroMemory(&Gti, sizeof(GUITHREADINFO));
	Gti.cbSize = sizeof(GUITHREADINFO);
	::GetGUIThreadInfo(0, &Gti);
	DWORD dwThread = ::GetWindowThreadProcessId(Gti.hwndActive, 0);
	HKL lang = ::GetKeyboardLayout(dwThread);

	size_t ScanCode = MapVirtualKeyExW((UINT)key, MAPVK_VK_TO_VSC, lang);
	BYTE uKeyboardState[256];

	for (int i = 0; i < 256; ++i)
		uKeyboardState[i] = (BYTE)GetKeyState(i);

	WCHAR ch = 0;
	ToUnicodeEx((UINT)key, (UINT)ScanCode, uKeyboardState, &ch, 1, 0U, lang);
	return ch;
}

Point UIX::get_cursor_pos() {
	return cursorPos;
}

void UIX::watch(_Variable* var) {
	if (!var) return;
	auto it = std::find(watchList.begin(), watchList.end(), var);
	if (it == watchList.end())
		watchList.push_back(var);
}

void UIX::ignore(_Variable* var) {
	if (!var) return;
	auto it = std::find(watchList.begin(), watchList.end(), var);
	if (it != watchList.end())
		watchList.erase(it);
}

void UIX::focus_next() {
	int sz = (int)focusables.size(), i;
	if (focusedElementID > -1 && focusedElementID < sz) {
		for (auto elem : focusables)
			if (elem->isActive) elem->blur();
	}
	++focusedElementID;
	focusedElementID = focusedElementID % (sz + 1);
	for (i = focusedElementID; i < sz; i++) {
		OUI* next = focusables[i];
		if (next && next->isEnabled && next->focusable()) {
			next->focus();
			break;
		}
		else if (i == sz - 1) {
			focusedElementID = sz;
		}
	}
}

void UIX::focus_prev() {
	int sz = (int)focusables.size(), i;
	if (focusedElementID > -1 && focusedElementID < sz) {
		for (auto elem : focusables)
			if (elem->isActive) elem->blur();
	}
	--focusedElementID;
	if (focusedElementID < 0) focusedElementID = sz;
	int e = focusedElementID < sz ? -1 : sz + 1;
	for (i = focusedElementID; i > e; --i) {
		OUI* next = focusables[i];
		if (next && next->isEnabled && next->focusable()) {
			next->focus();
			break;
		}
		else if (i == 0) {
			focusedElementID = sz;
		}
	}
}

bool UIX::is_key_down(uint32_t key) {
	return (GetKeyState(key) & 0x8000) != 0;
}

bool UIX::is_key_on(uint32_t key) {
	return (GetKeyState(key) & 0x0001) != 0;
}

void UIX::show_magnifier(bool show, int width, int height, double scale) {
	bDrawMagnifier = show;
	magnifier.set_scale(scale);
	auto pos = get_cursor_pos();
	magnifier.move(pos.x, pos.y, width, height);
	magnifier.show_window(bDrawMagnifier);
}

void UIX::show_box_model(bool show) {
	if (show == bDrawBoxModel) return;
	bDrawBoxModel = show;
	shouldInvalidate = true;
}

void UIX::set_magnifier_scale(double scale) {
	magnifier.set_scale(scale);
}

void UIX::apply_theme_all(OUI* element) {
	element->apply_theme(false);
	for (auto e : element->elements)
		apply_theme_all(e);
}

void UIX::apply_theme_all() {
	// apply to container, menus and windows
	if (container)
		apply_theme_all(container);
	for (auto& m : menus)
		apply_theme_all(m.second);
	for (auto w : windows)
		apply_theme_all(w);
}

void UIX::show_window(UIWindow* window, bool show) {
	if (show != window->isVisible) {
		visibleWindows += (show ? +1 : -1);
		window->isVisible = show;
	}
}

HWND UIX::get_hwnd() {
	return hWnd;
}

#endif