#pragma once
#include "oui.h"
#include "oui_button.h"

enum class OUI_API WindowType {
	Dialog,
	Pallette
};

enum class OUI_API DialogButtonSet {
	None,
	OK,
	OK_Cancel,
	Yes_No,
	Yes_No_Cancel
};

class OUI_API UIWindow : public OUI
{
public:
	OUI* caller;
	Rect titleBarArea;
	std::wstring title;
	svg_path *svgApp;
	bool btnCloseHover, btnCloseDown, btnMaxHover, btnMaxDown, btnMinHover, btnMinDown, bShowTitleBar,
		bShowMinBtn, bShowMaxBtn, bShowCloseBtn;
	int icw, ich;
	WindowType type;
	Color btnCloseColor, crHover, crDown;

	void apply_theme(bool bInvalidate) override;
	void show_window(bool show = true) override;
	virtual void close(uint32_t);
	OUI* get_draggable(int x, int y, uint32_t flags) override;
	void on_update() override;
	void on_mouse_move(int x, int y, uint32_t flags) override;
	void on_mouse_down(int x, int y, uint32_t flags) override;
	void on_mouse_up(int x, int y, uint32_t flags) override;
	void on_mouse_leave(OUI* next) override;
	void on_dbl_click(int x, int y, uint32_t flags) override;
	OUI* create(int left, int top, int width, int height, OUI* caller, WindowType type);
	virtual void set_title(std::wstring newTitle);
	bool is_child_visible(OUI* child) override;
	void update_position() override;

	virtual void get_title_bar_area(Rect& rcTitleBar);
	virtual void show_title_bar(bool bShow = true);
	virtual void show_buttons(bool minimize, bool maximize, bool close);
};

struct OUI_API ButtonName {
	uint32_t id;
	std::wstring name;
};

class OUI_API UIDialog : public UIWindow {
private:
	OUI* create(int width, int height, OUI* caller, size_t buttonNumber);
	UIButton* find(uint32_t buttonId);

public:
	std::vector<UIButton*> buttons;
	std::map<UIButton*, uint32_t> buttonIds;

	OUI* create(int width, int height, OUI* caller, DialogButtonSet buttonSet);
	OUI* create(int width, int height, OUI* caller, std::initializer_list<ButtonName> list);
	void process_event(OUI* element, uint32_t message, uint64_t param, bool bubbleUp) override;
	void set_default_button(uint32_t buttonId);
	UIButton* get_button(uint32_t id);
	void on_resize(int width, int height) override;
};
