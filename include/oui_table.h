#pragma once
#include "oui_stack.h"

typedef bool (*Compare)(void* const a, void* const b, void* param);

class UITable : public UIStack
{
public:
	using UIStack::create;
	UITable();
	OUI* rowMenu;
	bool bDragOrder;

	virtual void sort(void* param, Compare compare);
};