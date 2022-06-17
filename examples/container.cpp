#include "container.h"

void UIContainer::on_init()
{
	int l = 100,
		t = 100;

	btn.create(l, t, 100, 32, this);
	btn.set_text(L"Press");

	btn.bDraggable = true;
}
