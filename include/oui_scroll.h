#pragma once
#include "oui_button.h"

enum class OUI_API ScrollMode {
	Horizontal,
	Vertical
};

class OUI_API UIScroll : public UIButton
{
public:
	using UIButton::create;
	UIScroll();

	int min, viewport, page, page_pos, handlePos, handleLength, lastHandlePos, lastX, lastY;
	ScrollMode mode;
	ShapeStorage handle;

	void calc_shape() override;
	OUI* create(int left, int top, int width, int height, OUI* parent, bool bAddToParent = true) override;
	void on_update_frame() override;
	void on_resize(int width, int height) override;
	void on_mouse_move(int x, int y, uint32_t param) override;
	void on_mouse_down(int x, int y, uint32_t param) override;
	void on_update() override;
	void on_click(int x, int y, uint32_t param) override;

	virtual void calc_handle();
	virtual void scroll_rel(int delta);
	virtual int get_total() const;
	virtual int get_pos() const;
	virtual void set_pos(int pos);
	virtual void set_handle_pos(int pos);
	virtual void calc_handle_pos(int pos);
	virtual void set_page(int page, int viewport);
};
