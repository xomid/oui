#pragma once
#include "oui_basics.h"
#include <oui.h>

class UIMagnifier : public OUI {
	double scale;
	Rect zoomArea;
	Sheet img;

public:
	void update_zoom_rect();
	void set_scale(double scale);
	void on_init() override;
	void on_update() override;
	void on_resize(int width, int height) override;
	void move(int left, int top) override;
	void move(int left, int top, int width, int height) override;
};