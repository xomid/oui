#pragma once
#include "oui_radio.h"
#include "oui_menu.h"
#include "oui_table.h"

class UISelectDefaultMenu : public UIMenu {
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
	OUI* selectedOption;
	bool bStaticTitle;

	enum class OptionState {
		Availability, Visibility
	};

	std::vector<bool> ghostState;

protected:
	void change_option_state(size_t index, bool bOn, OptionState optionState) {
		bool bSelecteItemUnselectable = false;
		OUI* prevSelOption = selectedOption;

		auto sz = menu->elements.size();
		for (size_t i = index; i < sz; ++i) {
			auto e = menu->elements[i];
			if (ghostState[i]) continue;

			if (i == index) {
				if (optionState == OptionState::Availability)
					e->enable(bOn);
				else
					e->show_window(bOn);
			}

			if (bSelecteItemUnselectable) {
				if (optionState == OptionState::Availability ?
					e->bEnabled : e->bVisible)
				{
					selectedOption = e;
					bSelecteItemUnselectable = false;
					break;
				}
			}
			if (e == selectedOption && !bOn) {
				selectedOption = NULL;
				bSelecteItemUnselectable = true;
			}
		}

		if (bSelecteItemUnselectable) {
			for (size_t i = 0; i <= index; ++i) {
				auto e = menu->elements[i];
				if (ghostState[i]) continue;

				if (optionState == OptionState::Availability ?
					e->bEnabled : e->bVisible)
				{
					selectedOption = e;
					bSelecteItemUnselectable = false;
					break;
				}
			}
		}

		if (prevSelOption != selectedOption)
			select_option_ptr(selectedOption);
	}

	void select_option_ptr(OUI* option) {
		if (option == NULL) {
			reset_title();
			return;
		}
		process_event(option, Event::Click, 0, true);
	}


public:

	OUI* get_selected_option() const {
		return selectedOption;
	}

	int get_selected_option_index() const {
		if (selectedOption == NULL) return -1;

		auto sz = menu->elements.size();
		for (size_t i = 0, visibleIndex = 0; i < sz; ++i) {
			auto e = menu->elements[i];
			if (e->bVisible && !ghostState[i]) {
				if (e == selectedOption)
					return visibleIndex;
				++visibleIndex;
			}
		}

		return -1;
	}

	void apply_theme(bool bInvalidate = true) override {
		set_background_color(OUITheme::secondary);
		set_color(OUITheme::text);
		if (bInvalidate) invalidate();
		set_font_size(InitialValues::fontSize);
		border.set(1, backgroundColor.bright(-30));
		menu->apply_theme(true);
		for (auto e : menu->elements) {
			e->set_background_color(menu->backgroundColor);
		}
	}

	void set_title(std::wstring title, bool bStaticTitle = true) {
		this->bStaticTitle = bStaticTitle;
		staticTitle = title;
		reset_title();
	}

	void reset_title() {
		if (bStaticTitle) UIRadio::set_text(staticTitle + L": <Choose an option>");
		else UIRadio::set_text(L": <Choose an option>");
	}

	void enable_option(size_t index, bool bEnable = true) {
		change_option_state(index, bEnable, OptionState::Availability);
		menu->invalidate();
	}

	void show_option(size_t index, bool bShow = true) {
		change_option_state(index, bShow, OptionState::Visibility);
		menu->reset_size();
	}

	void select_option(uint32_t index) {
		auto sz = menu->elements.size();
		if (index >= sz) return;
		for (size_t i = 0, visibleIndex = 0; i < sz; ++i) {
			auto e = menu->elements[i];
			if (e->bVisible && !ghostState[i]) {
				if (visibleIndex++ == index) {
					select_option_ptr(e);
					break;
				}
			}
		}
	}

	void on_resize(int width, int height) override {
		if (menu) {
			for (auto e : menu->elements) {
				e->move(0, 0, area.width, e->boxModel.height);
			}
			menu->reset_size();
		}
	}

	template<typename Element>
	Element* add_option(std::wstring text, int height, bool bGhost = false) {
		Element* elem = new Element();
		elem->create(0, 0, 100, height, menu);
		elem->set_text(text);
		elem->canvas.art.alignX = canvas.art.alignX;
		elem->padding = padding;
		ghostState.push_back(bGhost);
		menu->reset_size();
		return elem;
	}

	UISelect() {
		menu = new UIList();
		type = UIRadioType::Button;
		selectedOption = NULL;
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
			menu->move(-padding.left, area.height, boxModel.width, menu->boxModel.height);
			menu->pop_up();
			menu->invalidate();
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
			selectedOption = element;
			((UIMenu*)menu)->fade();
			invalidate();
			if (parent) parent->process_event(this, Event::Update, 0, true);
			return;
		}

		OUI::process_event(element, message, param, true);
	}
};
