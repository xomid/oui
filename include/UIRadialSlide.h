#pragma once
#include "oui_button.h"
#include "UISlide.h"

class OUI_API UIRadialSlide : public UISlide
{
	Point center;
	double shift, angle;

protected:
	void* get_handle(int x, int y, uint32_t flags) override;
	double calc_value(void* handle, int x, int y, uint32_t flags) override;
	void calc_handle_pos(void* handle, double t) override;

public:
	void apply_theme(bool bInvalidate = true) override;
	void set_shift(double shift);
	void on_update() override;
	void on_init() override;
	void on_resize(int width, int height) override;
};

