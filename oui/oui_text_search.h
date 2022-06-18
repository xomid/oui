#pragma once
#include "oui_text.h"
#include "agg_svg_parser.h"

class OUI_API UITextSearch : public UIText
{
public:
	OUI* create(int left, int top, int width, int height, OUI* parent, bool bAddToParent = true) override;
};
