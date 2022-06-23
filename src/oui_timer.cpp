#include "oui_timer.h"

timer::timer () {
	this->element = NULL;
	this->elapse = -1000;
	this->nTimer = -1;
	this->id = -1;
	last = clock();
	killed = true;
}

timer::timer(OUI* element, uint32_t nTimer, clock_t elapse, uint32_t id) {
	this->element = element;
	this->elapse = elapse;
	this->nTimer = nTimer;
	this->id = id;
	last = clock();
	killed = false;
}