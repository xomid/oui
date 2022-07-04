#pragma once
#include "oui.h"
#include "agg_svg_parser.h"

typedef agg::svg::path_renderer SVGShape;
typedef SVGShape* PSVGShape;

class OUI_API UILabel : public OUI
{
public:
	using OUI::create;
	UILabel();

	static std::map<std::string, agg::svg::path_renderer*> *cachedPath;
	agg::svg::path_renderer* m_path;
	std::map<std::string, Color> colors;
	agg::trans_affine overlayMat;
	bool ltr;

	void on_update() override;
	void set_color(Color color) override;
	void set_text(std::wstring text) override;
	virtual agg::svg::path_renderer* get_path();
};

// util

OUI_API std::string optimizeSVG(std::string svg);

OUI_API agg::svg::path_renderer* parse_svg(std::string svg);

OUI_API double calc_scale(int width, int height, PSVGShape svgShape);