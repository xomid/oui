#pragma once
#include "oui_basics.h"
#include "oui.h"
#include "oui_window.h"
#include "oui_variable.h"
#include "oui_timer.h"
#include "oui_magnifier.h"
#include <unordered_map>
#include <map>

#ifdef _WIN32

#include <Windows.h>
class OUI_API UIX
{
public:
	UIX();
	UIX(const UIX&) = delete;
	UIX& operator=(const UIX&) = delete;
	~UIX();
	bool ProcessMessages();
	void render();
	Point get_cursor_pos();
	void focus_next();
	void focus_prev();
	void select_window(UIWindow* activeWindow);
	bool is_locked();
	bool pop_up_dialog(UIWindow* newWindow);
	void add_window(UIWindow* newWindow);
	void destroy();
	void update();
	void set_container(OUI* newContainer);
	OUI* get_container();
	std::wstring copy_from_clipboard();
	void copy_to_clipboard(std::wstring str);
	void invalidate();
	void show_menu(OUI* menu);
	void hide_menu(OUI* menu);
	bool set_focused_element(OUI* focusedElem);
	void lock_screen(int lockerID);
	void unlock_screen(int lockerID);
	void update_section(Rect* rc);
	void set_timer(OUI* element, uint32_t nTimer, uint32_t nElapse);
	void kill_timer(OUI* element, uint32_t ntimer);
	void set_capture(OUI* element);
	void release_capture();
	OUI* get_capture();
	void link_bitmap(int iw, int ih, int nBits);
	void resize(size_t width, size_t height);
	void add_element(OUI* newElement);
	void remove_element(OUI* newElement);
	void delete_element(OUI* newElement);
	void update_elements_list();
	void add_focusable(OUI* focusable);
	void remove_focusable(OUI* focusable);
	void watch(_Variable* var);
	void ignore(_Variable* var);
	wchar_t convert_char(uint32_t key);
	bool is_key_down(uint32_t key);
	bool is_key_on(uint32_t key);
	void show_window(bool show);
	void set_title(std::wstring title);
	bool is_window_visible() const;
	void invalidate_child();

	void PreLButtonDown(int x, int y, uint32_t param);
	void OnIdle();
	void OnPaint();
	void OnMouseMove(uint32_t nFlags, int x, int y);
	void OnLButtonDown(uint32_t nFlags, int x, int y);
	void OnLButtonUp(uint32_t nFlags, int x, int y);
	void OnSize(size_t width, size_t height);
	void OnKeyUp(uint32_t nChar, uint32_t nRepCnt, uint32_t nFlags);
	void OnKeyDown(uint32_t nChar, uint32_t nRepCnt, uint32_t nFlags);
	void OnLButtonDblClk(uint32_t nFlags, int x, int y);
	BOOL OnEraseBkgnd(HDC pDC);
	BOOL OnMouseWheel(uint32_t nFlags, short zDelta, int x, int y);
	BOOL PreTranslateMessage(LPMSG msg);
	void move(Rect* rc);
	bool create(Rect* rc);
	void set_magnifier_scale(double scale);
	void show_magnifier(bool show, int width, int height, double scale);
	void show_box_model(bool show = true);
	void apply_theme_all();
	void show_window(UIWindow* window, bool show = true);
	HWND get_hwnd();

private:
	void apply_theme_all(OUI* element);

	HDC displayDC, hdc;
	HBITMAP bit;
	HANDLE hTimer, hTimerQueue;
	HINSTANCE m_hInstance;
	HWND hWnd;
	DWORD style;

	Sheet sheet;
	std::vector<_Variable*> watchList;
	std::vector<timer*> timers;
	std::map<OUI*, OUI*> menus;
	std::map<UIWindow*, UIWindow*> windowsIndex;
	std::vector<UIWindow*> windows;
	std::vector<OUI*> elements, focusables, toDelete;
	UIWindow* currDialog;
	UIWindow* activeWindow;
	OUI* container, * currentElementHovering, * capturedElement, * lastDragHoverElement;
	UIMagnifier magnifier;
	Drag<int, int> dragMan;
	bool shouldInvalidate, shouldDrawBoxModel, shouldDrawMagnifier, shouldTerminate, isTerminated, shouldUpdate, shouldInitiateDrag;
	int id, lockerID, focusedElementID, visibleWindows, countBuffer, lXPos, lYPos, dragZIndex;
	std::string strbuffer;
	std::wstring title;
	clock_t last_time, lastLButtonDownTime;
	Point cursorPos;
	const wchar_t* CLASS_NAME = L"UIX";
};

#endif