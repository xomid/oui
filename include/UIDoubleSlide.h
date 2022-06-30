#pragma once
#include "UISlide.h"

class OUI_API UIDoubleSlide : public UILinearSlide
{
protected:
	Rect rcHandle2;
	Color fill2;
	RangedFloat range2;

	virtual void* get_handle(int x, int y, uint32_t flags);
	virtual double calc_value(void* handle, int x, int y, uint32_t flags);
	virtual void on_range_update();
	void alert_parent(void* handle) override;
	RangedFloat& get_range(void* handle) override;

public:
	UIDoubleSlide();
	void on_init() override;
	void on_update() override;
	virtual void set_handle_color(int handleId, Color fillColor);
	virtual void set_value(int handleId, double value);
	virtual double get_value(int handleId) const;
};


