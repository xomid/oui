#include "oui_group.h"

void UIGroup::process_event(OUI* element, uint32_t message, uint64_t param, bool bubbleUp) {
	UIStack::process_event(element, message, param, bubbleUp);

	if (element && message == Event::Select) {
		bool res = element->bSelected;
		if (res) {
			iterateI(elements.size()) {
				OUI* elem = elements[i];
				if (elem != element && elem->bSelected) {
					elem->select(false);
				}
			}
		}

		//invalidate();

		/*OUI* par = this;
		while (par) {
			if (par->parent && !par->parent->bSizeSensitive)
				break;
			par = par->parent;
		}*/

		//on_resize(0, 0);
		//parent->reset_size();
	}
}