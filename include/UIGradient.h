#pragma once
#include "oui_button.h"

typedef void (*GradientFunc)(void* param, double t, Color& res);

OUI_API void gradient_func_black_white(void* param, double t, Color& res);

class OUI_API UIGradient : public UIButton {
	Sheet img;
	GradientFunc gradientFunc;
	bool reverse, vertical;
	
public:
	void apply_theme(bool bInvalidate = true) override;
	void on_init() override;
	void on_resize(int width, int height) override;
	void on_update() override;
	void set_gradient(GradientFunc gradientFunc, bool vertical, bool reverse);
	void fill_image();
};
