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
	OUI::create(left, top, width, height, parent, false);
	show_window(false);
	return this;
}

void UIMenu::apply_theme(bool bInvalidate) {
	OUI::apply_theme(bInvalidate);
	set_background_color(OUITheme::menu);
}