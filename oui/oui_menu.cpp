#include "oui_menu.h"
#include "oui_uix.h"

UIMenu::UIMenu() : UIStack() {
	mode = UIStackMode::STACKVER;
	bVisible = false;
	zIndex = 1;
	bMenu = true;
	bStretch = true;
	bScrollable = true;
}

OUI* UIMenu::create(int left, int top, int width, int height, OUI* parent, bool bAddToParent) {
	OUI::create(left, top, width, height, parent->uix->get_container(), bAddToParent);
	show_window(false);
	return this;
}

