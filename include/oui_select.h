#pragma once
#include "oui_radio.h"
#include "oui_menu.h"
#include "oui_table.h"

class OUI_API UISelectDefaultMenu : public UIMenu {
public:
	void measure_size(int* width, int* height) override {
		measure_content(width, height);
		if (width) *width = parent->area.width;
	}
	void on_resize(int width, int height) override {
		UIMenu::on_resize(width, height);
	}
};

template<typename UIList = UISelectDefaultMenu>
class UISelect : public UIRadio
{
	std::wstring staticTitle;
	bool bStaticTitle;

public:

	void apply_theme(bool bInvalidate = true) override {
		set_background_color(OUITheme::secondary);
		set_color(OUITheme::text);
		if (bInvalidate) invalidate();
		set_font_size(InitialValues::fontSize);
		border.set(1, backgroundColor.bright(-30));
		menu->apply_theme(true);
		for (auto e : menu->elements)
			e->apply_theme(true);
	}

	void set_title(std::wstring title, bool bStaticTitle = true) {
		this->bStaticTitle = bStaticTitle;
		staticTitle = title;
		UIRadio::set_text(title + L": <Choose an option>");
	}

	void select_item(uint32_t index) {
		if (index >= menu->elements.size()) return;
		process_event(menu->elements[index], Event::Click, 0, true);
	}

	template<typename Element>
	Element* add(std::wstring text) {
		Element* elem = new Element();
		elem->create(0, 0, 100, area.height, menu);
		elem->set_text(text);
		elem->canvas.art.alignX = canvas.art.alignX;
		elem->padding = padding;
		menu->reset_size();
		return elem;
	}

	UISelect() {
		menu = new UIList();
		type = UIRadioType::Button;
		borderRadius.set(4, 4, 4, 4);
	}

	void on_init() override {
		UIRadio::on_init();
		padding.set(8, 1);
		int brr = 4;
		menu->create(0, area.height, area.width, 60, this);
		menu->set_parent(this);
		menu->set_border_radius(brr, brr, brr, brr);
		menu->add_box_shadow(false, 2, 2, 20, 0, Color(0, 0, 0, 77));
		menu->menuType = MenuType::Solid;
		menu->menuActivationMode = MenuActivationMode::PointerDown;
	}

	void on_mouse_down(int x, int y, uint32_t param) override {
		select(!menu->bVisible);
		if (!menu->bVisible) {
			menu->pop_up();
		}
		else {
			menu->fade();
		}
	}

	void on_update() override {
		UIRadio::on_update();
	}

	void process_event(OUI* element, uint32_t message, uint64_t param, bool bubbleUp) override {
		if (message == Event::Deselect && element == menu) {
			UIRadio::select(menu->bVisible);
		}

		if (message == Event::Click && element != (OUI*)menu->scrollY && element != (OUI*)menu->scrollX) {
			if (bStaticTitle) this->text = staticTitle + L": " + element->text;
			else this->text = element->text;

			((UIMenu*)menu)->fade();
			message = Event::Select;
			element = this;
			invalidate();
			if (parent) parent->process_event(this, Event::Update, 0, true);
			return;
		}

		OUI::process_event(element, message, param, true);
	}
};
