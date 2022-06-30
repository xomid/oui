#include "oui_table.h"

UITable::UITable(): UIStack() {
	mode = UIStackMode::STACKVER;
	rowMenu = NULL;
	bDragOrder = false;
}

void UITable::sort(void* param, Compare compare) {
	struct _compare {
		explicit _compare(void* param, Compare compare) { 
			this->param = param;
			this->m_compare = compare;
		}
		bool operator ()(OUI* const& a, OUI* const& b) const {
			return m_compare((void*)a, (void*)b, param);
		}
		void* param;
		Compare m_compare;
	};

	std::sort(elements.begin(), elements.end(), _compare(param, compare));
	reset_size();
}

/*void UITable::on_mouse_move(int x, int y, size_t param) {
	UIStack::on_mouse_move(x, y, param);
	if (rowMenu) {
		iterateU(elements) {
			if (elem->area.is_inside(toAbsX(x), toAbsY(y))) {
				rowMenu->set_parent(elem);
				rowMenu->on_parent_resize();
				Rect rc;
				elem->get_content_area(rc);

				int ofy = 0, ofx = 0;
				ofx = rowMenu->area.left - rc.left - elem->area.left + rowMenu->margin.left;
				ofy = rowMenu->area.top - rc.top - elem->area.top + rowMenu->margin.top;

				if (rowMenu->canvas.art.alignX == Align::CENTER)
					ofx = (elem->area.width - rowMenu->area.width) / 2;
				if (rowMenu->canvas.art.alignY == Align::CENTER)
					ofy = (elem->area.height - rowMenu->area.height) / 2;
			}
		}
	}
}*/