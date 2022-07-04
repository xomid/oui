#pragma once
#include "oui_text.h"
#include "oui_button.h"
#include <precise_float.h>

#define UINUMBER_CHANGE_TICK_ELAPSE 33
#define UINUMBER_CHANGE_ELAPSE 600
#define UINUMBER_CHANGE_ACCELERATION 0.6

class OUI_API UINumber : public UIText {
private:
	PreciseNumber number;
	UILightButton btnIncrease, btnDecrease;
	clock_t lastDown, lastChange, waitingElapse;

public:
	using UIText::create;
	UINumber();

	void set_value(double value);
	void set_value(std::string value);
	void set_range(double num, double step, double minValue, double maxValue);
	void set_range(std::string num, std::string step, std::string minValue, std::string maxValue);
	std::string get_number() const;
	void set_text(std::wstring newText) override;
	void trigger_update() override;
	void on_key_down(uint32_t key, uint32_t nrep = 1, uint32_t flags = 0) override;
	bool pre_append(wchar_t c) override;
	bool pre_append(std::wstring str) override;
	void on_update() override;
	void blur() override;
	void increase();
	void decrease();
	void on_resize(int width, int height) override;
	void on_timer(uint32_t nTimer) override;
	void config(std::string number = "0", std::string tick = "1.0", std::string minPrice = "na", std::string maxPrice = "na");
	void text_to_num();
	void num_to_text();
	void on_mouse_move(int x, int y, uint32_t flags) override;
	void on_mouse_down(int x, int y, uint32_t flags) override;
	void on_mouse_up(int x, int y, uint32_t flags) override;
	void on_dbl_click(int x, int y, uint32_t flags) override;
	void on_mouse_enter(OUI* prev) override;
	void on_mouse_leave(OUI* next) override;
};

