#include "container.h"
#include "oui_uix.h"
#include "oui_scroll.h"
#include "oui_stack.h"
#include "oui_uix.h"
#include "UIDOpenFile.h"
#include "UIDSaveFile.h"

UIStack list;
UIButton btnOpen;

void UIContainer::on_init()
{
	int l = 100,
		t = 100,
		w = 340,
		h = 200;

	
	btnOpen.create(l, t, 100, 30, this);
	btnOpen.set_text(L"Open...");

	/*dlg.create(w, h, this, DialogButtonSet::OK_Cancel);
	dlg.set_default_button(DialogButtonId::OK);
	dlg.set_background_color(Color("#ddd"));
	dlg.show_window();*/

	//list.create(l, t, 260, 200, this);
	////list.mode = UIStackMode::STACKVER;

	//for (int i = 0; i < 20; ++i) {
	//	auto button = new UIButton();
	//	button->create(0, 0, 100, 40, &list);
	//	button->set_text(L"button");
	//}

	//list.reset_size();

	OUITheme::primary.set("#eee");
	OUITheme::secondary.set("#ddd");
	OUITheme::text.set("#444");
	uix->apply_theme_all();
}


void UIContainer::process_event(OUI* element, uint32_t message, uint64_t param, bool bubbleUp) {
	if (element == &btnOpen && message == Event::Click) {
		UIDSaveFile dlgSave;

		dlgSave.set_default_extention(L"gif");
		dlgSave.set_file_name(L"foo");

		dlgSave.set_default_dir(L"C:\\dev");
		dlgSave.set_directory(L"C:\\users\\omid\\pictures");
		dlgSave.set_type_index(1);
		dlgSave.set_file_types({
			{ L"JPEG", L"*.jpg;*.jpeg" },
			{ L"PNG", L"*.png" },
			{ L"BMP", L"*.bmp" },
			{ L"GIF", L"*.gif" },
			{ L"All", L"*.*" },
			});
		if (dlgSave.show_window(this) == 0) {
			auto fileNames = dlgSave.get_selected_file_names();
			std::wstring res = L"";
			for (auto fileName : fileNames)
				res += fileName + L" ";
			MessageBoxW(NULL, res.c_str(), L"File Path", MB_OK);
		}
	}
}