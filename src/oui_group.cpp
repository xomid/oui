#include "oui_group.h"

void UIGroup::process_event(OUI* element, uint32_t message, uint64_t param, bool bubbleUp) {
	UIStack::process_event(element, message, param, bubbleUp);

	if (element && message == Event::PostSelect) {
		bool res = element->isSelected;
		if (res) {
			iterateI(elements.size()) {
				OUI* elem = elements[i];
				if (elem != element && elem->isSelected) {
					elem->select(false);
				}
			}
		}
	}
}