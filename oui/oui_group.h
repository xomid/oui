#pragma once
#include "oui_stack.h"
class OUI_API UIGroup : public UIStack
{
public:
	void process_event(OUI* element, uint32_t message, uint64_t param, bool bubbleUp) override;
};