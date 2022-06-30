#pragma once
#include "oui_button.h"
#include "UIGradient.h"
#include <precise_float.h>

OUI_API enum class SlideDirection {
	HORIZONTAL,
	VERTICAL
};

class OUI_API UISlide : public UIButton {
protected:
	RangedFloat range;
	GradientFunc gradientFunc;
	Sheet slideImg;
	Color back, fill, stroke;
	double rad, strokeWidth;
	void* chosenHandle;

	void calc_handle_pos(RangedFloat& number, void* handle);
	virtual void on_range_update();
	virtual void update_slide_img() {}
	virtual void alert_parent(void* handle);
	virtual void* get_handle(int x, int y, uint32_t flags);
	virtual double calc_value(void* handle, int x, int y, uint32_t flags);
	virtual void calc_handle_pos(void* handle, double t);
	virtual RangedFloat& get_range(void* handle);

public:
	UISlide();
	void set_gradient_func(GradientFunc gradFunc);
	
	void apply_theme(bool bInvalidate = true) override;
	void on_mouse_move(int x, int y, uint32_t flags) override;
	void on_mouse_down(int x, int y, uint32_t flags) override;
	void on_dbl_click(int x, int y, uint32_t flags) override;
	void on_mouse_up(int x, int y, uint32_t flags) override;

	void set_range(double minValue, double maxValue);
	void set_step(double stepValue);
	void set_value(double value);
	double get_value() const;
};

class OUI_API UILinearSlide : public UISlide {
	
protected:
	SlideDirection dir;
	Rect rcHandle;
	int handleWidth, minPos, maxPos;
	int pad;

	void* get_handle(int x, int y, uint32_t flags) override;
	double calc_value(void* handle, int x, int y, uint32_t flags) override;
	void calc_handle_pos(void* handle, double t) override;
	void update_slide_img() override;

public:
	void set_direction(SlideDirection newDir);
	SlideDirection get_direction() const;
	void on_init() override;
	void on_update() override;
	void on_resize(int width, int height) override;
};


