#pragma once
#include <time.h>
#include <map>
#include "oui_canvas.h"
//#include "oui_uix.h"
//#include "oui_scroll.h"

class OUI_API OUI
{
public:
	static int init();

	OUI();
	~OUI();
	static OUI* find_element(OUI* curr, int x, int y);
	static byte* blur_stack;

	std::string string_id;
	uint32_t id;

	std::vector<OUI*> elements;
	OUI* parent, * menu;
	UIScroll* scrollX, * scrollY;
	UIX* uix;
	Canvas canvas;
	Rect area, contentArea;
	std::wstring text, fontName;
	Sheet* godSheet;
	int lockerID, transparenElementsSize, zIndex, fontSize;
	bool bInvalidated, bVisible, bFocusable, bCreated,
		bActive, bEnabled, bSelected, bHover, bMenu, bScrollable, bDraggable;
	byte opacity;
	Color color, backgroundColor;
	ShapeStorage shape, contentShape, absContentShape;
	Border border;
	BorderRadius borderRadius;
	Spacing margin, padding;
	Overflow overflow;
	BoxModel boxModel;
	UIFloatType floatType;
	Sheet outsetBoxShadowSheet, insetBoxShadowSheet;
	std::vector<BoxShadow> outsetBoxShadows;
	std::vector<BoxShadow> insetBoxShadows;
	MenuType menuType;
	MenuActivationMode menuActivationMode;
	std::map<std::string, std::string> values;

	virtual void delete_elements();
	virtual OUI* get_draggable(int x, int y, uint32_t flags);
	inline bool check_transparency();
	virtual void invalidate();
	virtual uint32_t get_id();
	virtual void set_id(uint32_t newId);
	virtual void on_key_down(uint32_t key, uint32_t nrep = 1, uint32_t flags = 0);
	virtual void on_update();
	virtual void on_update_frame();
	virtual void on_update_frame_raw();
	virtual void on_click(int x, int y, uint32_t param) {}
	virtual void on_dbl_click(int x, int y, uint32_t param);
	virtual void on_mouse_down(int x, int y, uint32_t param);
	virtual void on_mouse_up(int x, int y, uint32_t param);
	virtual void on_mouse_move(int x, int y, uint32_t param);
	virtual void on_mouse_enter(OUI* prev);
	virtual void on_mouse_leave(OUI* next);
	virtual bool on_mouse_wheel(int x, int y, int zDelta, uint32_t param);
	virtual void move(int x, int y);
	virtual void move(int x, int y, int width, int height);
	virtual void update_position();
	virtual void calc_shape();
	virtual void move_shape();
	virtual void hover(bool bHover);
	virtual void set_capture(OUI* element);
	virtual void release_capture();
	virtual OUI* get_capture();
	virtual void reset_size();
	virtual void show_window(bool bShow = true);
	virtual void set_timer(uint32_t nTimer, uint32_t nElapse);
	virtual void kill_timer(uint32_t ntimer);
	virtual void add_element(OUI* newElement);
	virtual void remove_element(OUI* newElement);
	virtual void process_event(OUI* element, uint32_t message, uint64_t param, bool bubbleUp);
	virtual OUI* create(int left, int top, int width, int height, OUI* parent, bool bAddToParent = true);
	virtual void focus();
	virtual void blur();
	virtual bool select(bool bSelect);
	virtual void on_timer(uint32_t nTimer);
	virtual void get_parent_position(Rect& rc);
	virtual void get_content_area(Rect& rc);
	virtual void set_color(Color color);
	virtual void set_font_size(int fontSize);
	virtual void set_font_name(std::wstring fontName);
	virtual void set_background_color(Color color);
	virtual void measure_size(int* width, int* height);
	virtual void set_text(std::wstring text);
	virtual bool is_within(int absX, int absY);
	virtual void set_border_radius(double lt, double rt, double rb, double lb);
	virtual void remove_border_radius();
	virtual void add_box_shadow(bool inset, int offsetX, int offsetY, int blur, int spread, Color color);
	virtual void clear_box_shadow();
	virtual void set_parent(OUI* parent);
	virtual void pop_up();
	virtual void fade();
	virtual OUI* get_root();
	virtual void set_overflow(Overflow overflow);
	virtual bool focusable();
	virtual bool is_child_visible(OUI* child);

	// no default behavior
	virtual bool on_drag_start(OUI* child);
	virtual void on_drag(OUI* child);
	virtual void on_drop(OUI* child);
	virtual bool can_drop(OUI* child);
	virtual void on_drag_enter(OUI* element);
	virtual void on_drag_leave(OUI* element);
	virtual void on_init();
	virtual void on_resize(int width, int height);
	virtual void on_parent_resize();
	virtual void on_destroy();
	virtual void on_window_closed(UIWindow* window, size_t wmsg);

	void set_color(byte red, byte green, byte blue, byte alpha = 0xff);
	void set_background_color(byte red, byte green, byte blue, byte alpha = 0xff);

	// experimental
	virtual void move_fast(int x, int y);
	virtual void move_fast(int x, int y, int width, int height);
	virtual void move_page(int left, int top);
};
