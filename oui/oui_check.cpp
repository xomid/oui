#include "oui_check.h"

UICheck::UICheck() {
	m_checked_path = NULL;
}

void UICheck::set_checked_shape(std::wstring shape) {
	int i, j;
	i = shape.find(L"<svg");
	j = shape.find(L"</svg>");

	if (i > -1 && j > -1) {
		
		j += 6;
		auto svg = oui::to_string(shape.substr(i, i - j));
		try {
			m_checked_path = parse_svg(svg);
		}
		catch (agg::svg::exception e) {
			m_checked_path = NULL;
		}
	}
}

agg::svg::path_renderer* UICheck::get_path() {
	if (bSelected && m_checked_path)
		m_checked_path->m_colors = &colors;
	return bSelected ? m_checked_path : m_path;
}