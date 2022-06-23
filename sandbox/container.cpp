#include "container.h"
#include "oui_radio.h"
#include "oui_group.h"
#include "oui_uix.h"
#include "oui_button.h"
#include "oui_check.h"
#include "oui_text.h"
#include "oui_number.h"
#include "oui_menu.h"

UIGroup group;
UIRadio radio1, radio2, radio3;
UICheck chk1, chk2, chk3;
//UIText txt;
//UINumber num;
UIButton btn1, btn2;

void UIContainer::on_init()
{
	int l = 100,
		t = 100;

	/*uix->bDrawBoxModel = 0;*/

	/*group.create(l, t + 60, 100, 100, this);
	group.overflow = Overflow::hidden;
	radio1.create(0, 0, 300, 14, &group);
	radio2.create(0, 30, 300, 14, &group);
	radio3.create(0, 60, 300, 14, &group);
	radio1.set_text(L"Radio 1");
	radio2.set_text(L"Radio 2");
	radio3.set_text(L"Radio 3");*/

	btn1.create(100, 100, 100, 30, this);
	btn2.create(100, 140, 100, 30, this);

	btn1.menu = new UIMenu();
	btn2.menu = new UIMenu();

	btn1.menu->create(0, 0, 100, 100, &btn1);
	btn2.menu->create(300, 100, 100, 100, &btn2);

	btn1.menu->menuActivationMode = MenuActivationMode::PointerDown;
	btn2.menu->menuActivationMode = MenuActivationMode::PointerDown;
	btn1.menu->menuType = MenuType::Solid;
	btn2.menu->menuType = MenuType::Solid;

	btn1.menu->border.set(1, Colors::purple);
	btn2.menu->border.set(1, Colors::purple);

	/*txt.create(100, 100, 100, 15, this);
	txt.set_text(L"000");

	num.create(100, 200, 100, 18, this);
	num.set_text(L"100");*/

	/*chk1.create(l, t, 300, 14, this);
	chk1.set_text(L"CheckBox 1");*/
}


void UIContainer::process_event(OUI* element, uint32_t message, uint64_t param, bool bubbleUp) {
	if (message == Event::Select) {
		element->select(true);
	} else if (message == Event::Deselect) {
		element->select(false);
	}
}