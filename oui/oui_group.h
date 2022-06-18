#pragma once
#include "oui_stack.h"
class OUI_API UIGroup : public UIStack
{
public:
	virtual void process_event(OUI* element, size_t message, uint64_t param, bool bubbleUp);
};