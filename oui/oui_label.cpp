#include "oui_label.h"
#include "agg_svg_parser.h"

std::map<std::string, agg::svg::path_renderer*>* UILabel::cachedPath = NULL;

OUI_API std::string optimizeSVG(std::string svg) {
	std::string res{};

	for (size_t i = 0, sz = svg.length(); i < sz; ++i) {
		char c = svg[i];

		if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
			res += ' ';
			while (++i < sz) {
				c = svg[i];
				if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) {
					i -= 1;
					break;
				}
			}
		}
		else {
			res += svg[i];
		}
	}
	return res;
}

OUI_API agg::svg::path_renderer* parse_svg(std::string svg) {
	//svg = optimizeSVG(svg);
	auto it = UILabel::cachedPath->find(svg);
	if (it == UILabel::cachedPath->end()) {
		auto* path = new agg::svg::path_renderer();
		path->left = 0;
		path->top = 0;
		path->width = 0;
		path->height = 0;
		agg::svg::parse_svg(*path, svg);
		UILabel::cachedPath->insert({ svg, path });
	}
	return UILabel::cachedPath->at(svg);
}

OUI_API double calc_scale(int width, int height, PSVGShape svgShape) {
	if (!svgShape) return 0;
	double res = fmin(double(width) / fabs(svgShape->width), double(height) / fabs(svgShape->height));
	if (svgShape->width <= 0 || svgShape->height <= 0) res = 0;
	return res;
}

UILabel::UILabel() : OUI() {
	UILabel::cachedPath = new std::map<std::string, agg::svg::path_renderer*>();

	ltr = true;
	m_path = NULL;
	canvas.art.alignX = Align::CENTER;
	canvas.art.alignX = Align::CENTER;
}

static std::wstring remove_spaces(std::wstring& str)
{
	str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
	str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

	return str;
}

agg::svg::path_renderer* UILabel::get_path() {
	if (m_path) m_path->m_colors = &colors;
	return m_path;
}

void UILabel::set_text(std::wstring text) {
	size_t i, j;
	i = text.find(L"<svg");
	j = text.find(L"</svg>");

	if (i != std::wstring::npos && j != std::wstring::npos) {
		j += 6;
		auto s = ocom::to_string(text.substr(i, j - i));
		try {
			m_path = parse_svg(s);
			m_path->bounding_rect(&m_path->left, &m_path->top, &m_path->width, &m_path->height);
			m_path->width -= m_path->left;
			m_path->height -= m_path->top;
		}
		catch (agg::svg::exception e) {
			m_path = NULL;
		}
		text = text.substr(0, i) + text.substr(j);
	}

	remove_spaces(text);
	OUI::set_text(text);
}

void UILabel::set_color(Color cr) {
	OUI::set_color(cr);
	colors["currentColor"] = color;
}

void UILabel::on_update() {
	OUI::on_update();

	Rect rc;
	get_content_area(rc);
	auto* path = get_path();
	double ds = 0;

	int l = rc.left, t = rc.top;

	auto box = canvas.get_box16((wchar_t*)text.c_str(), text.length());

	if (path) {

		ds = fmin(float(rc.width - 1.0 - path->pl - path->pr) / fabs(path->width),
			float(rc.height - 1.0 - path->pt - path->pb) / fabs(path->height));

		if (path->width <= 0 || path->height <= 0)
			ds = 0;

		if (canvas.art.alignX == Align::CENTER)
			l += int((rc.width - double(box->get_width()) -
				path->width * ds - path->pl - path->pr) / 2);
		if (canvas.art.alignX == Align::RIGHT)
			l += int(rc.width - box->get_width() -
				path->width * ds - path->pl - path->pr);
	}
	else {
		if (canvas.art.alignX == Align::CENTER)
			l += (rc.width - box->get_width()) / 2;
		if (canvas.art.alignX == Align::RIGHT)
			l += rc.width - box->get_width();
	}

	if (canvas.art.alignY == Align::CENTER) {
		int textH = (int)canvas.get_max_height();
		t += (rc.height - textH) / 2;
	}
	if (canvas.art.alignY == Align::BOTTOM)
		t += rc.height;

	if (ltr)
		l += canvas.draw_text16((wchar_t*)text.c_str(), text.length(), l, t);

	if (path) {
		int tt = rc.top;
		if (canvas.art.alignY == Align::CENTER)
			tt += int((rc.height - path->height * ds - path->pt - path->pb) / 2);
		if (canvas.art.alignY == Align::BOTTOM)
			tt += int(rc.height - path->height * ds - path->pt - path->pb);

		canvas.render_svg(path, l, tt, rc.width, rc.height, opacity, NULL, &overlayMat);
		if (!ltr) l += int(path->width * ds + path->pl + path->pr);
	}

	if (!ltr) canvas.draw_text16((wchar_t*)text.c_str(), text.length(), l, t);
}