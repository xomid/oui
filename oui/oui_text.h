#pragma once
#include "oui_label.h"

OUI_API bool is_key_on(uint32_t key);

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

	virtual bool pre_append(wchar_t c);
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
	void on_timer(uint32_t nTimer) override;
	void on_key_down(uint32_t key, uint32_t nrep = 1, uint32_t flags = 0) override;
	void set_text(std::wstring text) override;
	void on_resize(int width, int height) override;
	void on_mouse_down(int x, int y, uint32_t flags) override;
	void on_dbl_click(int x, int y, uint32_t flags) override;
	void on_mouse_up(int x, int y, uint32_t flags) override;
	void on_mouse_move(int x, int y, uint32_t flags) override;
	void on_mouse_enter(OUI* prev) override;
	void on_mouse_leave(OUI* next) override;
	void set_icon(std::string svg);
	void get_content_area(Rect& rc) override;
	OUI* create(int left, int top, int width, int height, OUI* parent, bool bAddToParent = true) override;
	void set_color(Color color) override;
	OUI* get_draggable(int x, int y, uint32_t flags) override;
	virtual void trigger_update();
};
