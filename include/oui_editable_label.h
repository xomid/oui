#pragma once
#include "oui_text.h"

class OUI_API UIEditableLabel : public UIText {
public:
	using UIText::create;
	bool bEditable;
	UIEditableLabel();
	void set(bool editable);
	bool is_editable();

	void on_init() override;
	void blur() override;
	void focus() override;
	void on_dbl_click(int x, int y, uint32_t flags) override;
	void on_mouse_down(int x, int y, uint32_t flags) override;
	void on_update() override;
	OUI* get_draggable(int x, int y, uint32_t flags) override;
	bool pre_append(wchar_t c) override;
	bool pre_append(std::wstring str) override;
	bool focusable() override;
	void on_key_down(uint32_t key, uint32_t nrep = 1, uint32_t flags = 0) override;
};
