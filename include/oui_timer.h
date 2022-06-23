#pragma once
#include "oui_basics.h"
#include <time.h>

struct OUI_API timer {
	OUI* element;
	uint32_t nTimer, id;
	clock_t last, elapse;
	bool killed;

	timer();
	timer(OUI* element, uint32_t nTimer, clock_t elapse, uint32_t id);
};
