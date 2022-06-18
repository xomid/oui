#include "oui_check.h"

UICheck::UICheck() {
	m_checked_path = NULL;
}

void UICheck::set_checked_shape(std::wstring shape) {
	size_t i, j;
	i = shape.find(L"<svg");
	j = shape.find(L"</svg>");

	if (i != std::string::npos && j != std::string::npos && j > i) {
		j += 6;
		auto svg = ocom::to_string(shape.substr(i, j - i));
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