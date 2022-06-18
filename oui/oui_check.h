#pragma once
#include "oui_radio.h"

class OUI_API UICheck : public UIRadio
{
public:
	UICheck();
	agg::svg::path_renderer* m_checked_path;

	void set_checked_shape(std::wstring text);
	agg::svg::path_renderer* get_path() override;
};