#pragma once
#include "oui.h"

enum class OUI_API UIStackMode {
	NOSTACK = 0,
	STACKHOR,
	STACKVER
};

class OUI_API UIStack : public OUI
{
public:
	UIStack();
	std::map<std::string, OUI*> elem;
	UIStackMode mode;
	int contentW, contentH;
	bool bStretch;

	void stretch();
	void set_direction(UIStackMode mode);
	void get_content_area(Rect& rc) override;
	void on_update() override;
	void measure_size(int* width, int* height) override;
	void on_resize(int width, int height) override;
	void move_page_ver(int top) override;
	void move_page_hor(int left) override;
	void move_page(int left, int top) override;
	virtual void measure_content(int* width, int* height);
};
