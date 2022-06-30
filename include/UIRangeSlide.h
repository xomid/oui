#pragma once
#include "UIDoubleSlide.h"

class OUI_API UIRangeSlide : public UIDoubleSlide
{
protected:
	Rect rcHandle3;
	RangedFloat boundRange;
	Color fill3;
	double mid;

	void* get_handle(int x, int y, uint32_t flags) override;
	double calc_value(void* handle, int x, int y, uint32_t flags) override;
	void calc_handle_pos(void* handle, double t) override;
	RangedFloat& get_range(void* handle) override;
	void alert_parent(void* handle) override;
	void on_range_update() override;
public:
	UIRangeSlide();
	void on_init() override;
	void on_update() override;

	void set_bound_range(double low, double high, double step);
	void set_handle_color(int handleId, Color fillColor) override;
	double get_value(int handleId) const override;
	void set_value(int handleId, double value) override;
};


