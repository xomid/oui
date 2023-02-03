#pragma once
#include "oui_label.h"

struct OUI_API UILightButton {
	Rect rc;
	bool isHover, isPressed;
	Color color, hoverColor, downColor;
	std::map<std::string, Color> colors;
	agg::trans_affine mat;
	int sw, sh, aw, ah;
	double scale;

	agg::svg::path_renderer* icon;
	UILightButton() : isHover(false), isPressed(false), sw(0), sh(0), aw(0), ah(0), scale(1.0), icon(0)
	{}
	void create(Rect rc, agg::svg::path_renderer* back, agg::svg::path_renderer* icon) {
		this->icon = icon;
		this->icon->m_colors = &colors;
		this->rc.set(rc);
		isHover = isPressed = false;
		aw = rc.width;
		ah = rc.height;
		double x0, y0, x1, y1;
		x0 = y0 = x1 = y1 = 0;
		icon->bounding_rect(&x0, &y0, &x1, &y1);
		sw = ABS(int(x1 - x0));
		sh = ABS(int(y1 - y0));
		scale = 1;
		mat.reset();
		adj_colors();
	}
	double get_aratio() const {

		return double(sw) / double(sh);
	}
	void set_colors(Color color, Color hoverColor, Color downColor) {
		this->color = color;
		this->hoverColor = hoverColor;
		this->downColor = downColor;
		adj_colors();
	}
	void set_origin(int left, int top, double scale) {
		rc.set(left - aw / 2, top - ah / 2, 0, 0);
	}
	bool is_in_cirlce(int cx, int cy, double rad, int x, int y) {
		auto r = sqrt(pow(cx - x, 2) + pow(cy - y, 2));
		return r <= rad;
	}
	void draw(Canvas& canvas) {
		icon->m_colors = &colors;
		canvas.render_svg(icon, rc.left, rc.top, rc.width, rc.height, 0xff, 0, &mat);
	}
	UILightButton& on_mouse_move(int x, int y, size_t flags) {
		isHover = icon && rc.is_inside(x, y);
		adj_colors();
		return *this;
	}
	bool on_mouse_down(int x, int y, size_t flags) {
		isPressed = isHover;
		adj_colors();
		return isPressed;
	}
	bool on_mouse_up(int x, int y, size_t flags) {
		auto temp = isPressed && isHover;
		isPressed = false;
		adj_colors();
		return temp;
	}
	void on_dbl_click(int x, int y, size_t flags) {
		isPressed = false;
		adj_colors();
	}
	void adj_colors() {
		if (isPressed && isHover) colors["currentColor"] = downColor;
		else if (isHover) colors["currentColor"] = hoverColor;
		else colors["currentColor"] = color;
	}
	int get_shape_width() {
		return sw;
	}
	int get_shape_height() {
		return sh;
	}
};


enum class OUI_API ButtonType {
	Clickable = 0,
	Checkbox
};

#define SBTN2D_TIMER_HOVER  0x01
#define SBTN2D_TIMER_LEAVE  0x02
#define SBTN2D_TIMER_DOWN   0x03
#define SBTN2D_TIMER_UP     0x04
#define SBTN2D_TIMER_INTERVAL   0
#define DEFAULT_CORNER_RADIUS 5

class OUI_API UIButton : public UILabel
{
public:
	bool isPressed;
	Rect handleArea;
	Color downBackColor, hoverBackColor;

	using UILabel::create;
	UIButton();

	void apply_theme(bool bInvalidate = true) override;
	bool is_withinArea(int x, int y);
	void on_update() override;
	void on_mouse_enter(OUI* prev) override;
	void on_mouse_leave(OUI* next) override;
	void on_mouse_move(int x, int y, uint32_t param) override;
	void on_mouse_down(int x, int y, uint32_t param) override;
	void on_mouse_up(int x, int y, uint32_t param) override;
	void on_timer(uint32_t nTimer) override;
	void on_resize(int width, int height) override;
	void on_dbl_click(int x, int y, uint32_t param) override;
	void on_click(int x, int y, uint32_t param) override;
	void set_background_color(Color color) override;
	bool select(bool bSelect) override;
};