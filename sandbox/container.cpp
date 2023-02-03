#include "container.h"
#include "oui_uix.h"
#include "oui_scroll.h"
#include "oui_stack.h"
#include "oui_uix.h"
#include "UIDOpenFile.h"
#include "UIDSaveFile.h"

UIDialog dlg;
UIStack list;
UIButton btnOpen;

void UIContainer::on_init()
{
	int l = -30,
		t = 100,
		w = 340,
		h = 200;
	
	btnOpen.create(l, t, 115, 40, this);
	btnOpen.isDraggable = true;

	set_background_color(Color(L"#ecf0f3"));
	btnOpen.set_background_color(Color(L"#ecf0f3"));
	btnOpen.borderRadius.set(20);
	btnOpen.add_box_shadow(false, 7, 7, 10, 0, Color("#cbced1"));
	btnOpen.add_box_shadow(false, -7, -7, 10, 0, Color("#fff"));

	/*dlg.create(w, h, this, DialogButtonSet::OK_Cancel);
	dlg.set_default_button(DialogButtonId::OK);
	dlg.set_background_color(Color("#ddd"));
	dlg.show_window();*/

	//OUITheme::primary.set("#eee");
	//OUITheme::secondary.set("#ddd");
	//OUITheme::text.set("#444");
	//uix->apply_theme_all();
	
	uix->show_box_model();
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