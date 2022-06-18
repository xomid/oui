#pragma once
#include "oui_label.h"
#include "oui_button.h"
#include <precise_float.h>

OUI_API bool is_key_on(size_t key);

class OUI_API UIText : public UILabel
{
public:
	UIText();

	Align defaultAlignX;
	Color placeholderColor;
	std::wstring placeholder;
	int selectionBeginIndex, currIndex, shiftX, shiftY, blinkerSpeed, textW, textH;
	bool blink, showClearIcon, showPlaceHolder;
	TextBox box;
	Point cursorPos;
	agg::svg::path_renderer* closeIcon;
	bool bShowIcon, bCloseIconDown, bIconDown;

	virtual bool pre_append(wchar c);
	virtual bool pre_append(std::wstring str);

	void set_direction(Align align);
	void reset_selection();
	int get_index(int x, int y);
	void clear();
	void set_place_holder_color(Color color);
	void set_place_holder_text(std::wstring placeholder);
	void set_index(int index);
	Point get_cursor_pos(bool absolute = false);
	void on_update() override;
	void focus() override;
	void blur() override;
	void on_timer(size_t nTimer) override;
	void on_key_down(size_t key, size_t nrep = 1, size_t flags = 0) override;
	void set_text(std::wstring text) override;
	void on_resize(int width, int height) override;
	void on_mouse_down(int x, int y, size_t flags) override;
	void on_dbl_click(int x, int y, size_t flags) override;
	void on_mouse_up(int x, int y, size_t flags) override;
	void on_mouse_move(int x, int y, size_t flags) override;
	void on_mouse_enter(OUI* prev) override;
	void on_mouse_leave(OUI* next) override;
	void set_icon(std::string svg);
	void get_content_area(Rect& rc) override;
	OUI* create(int left, int top, int width, int height, OUI* parent, bool bAddToParent = true) override;
	void set_color(Color color) override;
	OUI* get_draggable(int x, int y, size_t flags) override;
	virtual void trigger_update();
};

#define UINUMBER_CHANGE_TICK_ELAPSE 33
#define UINUMBER_CHANGE_ELAPSE 600
#define UINUMBER_CHANGE_ACCELERATION 0.6

class OUI_API UINumber : public UIText {
private:
	PreciseNumber number;
	UILightButton btnIncrease, btnDecrease;
	clock_t lastDown, lastChange, waitingElapse;

public:
	UINumber();


	Float get_number() const;
	void trigger_update() override;
	void on_key_down(size_t key, size_t nrep = 1, size_t flags = 0) override;
	bool pre_append(wchar c) override;
	bool pre_append(std::wstring str) override;
	void on_update() override;
	void blur() override;
	void increase();
	void decrease();
	void on_resize(int width, int height) override;
	void on_timer(size_t nTimer) override;
	void config(std::string number = "0", std::string tick = "1.0", std::string minPrice = "na", std::string maxPrice = "na");
	void text_to_num();
	void num_to_text();
	void on_mouse_move(int x, int y, size_t flags) override;
	void on_mouse_down(int x, int y, size_t flags) override;
	void on_mouse_up(int x, int y, size_t flags) override;
	void on_dbl_click(int x, int y, size_t flags) override;
	void on_mouse_enter(OUI* prev) override;
	void on_mouse_leave(OUI* next) override;
};

class OUI_API UIEditableLabel : public UIText {
public:
	bool bEditable;
	UIEditableLabel();
	void set(bool editable);
	bool is_editable();

	void on_init() override;
	void blur() override;
	void focus() override;
	void on_dbl_click(int x, int y, size_t flags) override;
	void on_mouse_down(int x, int y, size_t flags) override;
	void on_update() override;
	OUI* get_draggable(int x, int y, size_t flags) override;
	bool pre_append(wchar c) override;
	bool pre_append(std::wstring str) override;
	bool focusable() override;
	void on_key_down(size_t key, size_t nrep = 1, size_t flags = 0) override;
};
