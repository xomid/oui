#include "oui_slot_cacher.h"
#include <harfbuzz/hb-ft.h>
#include <freetype/ftlcdfil.h>
#include "oui_default_fonts.h"

SlotCacher::FontsMap* SlotCacher::fonts = NULL;
SlotCacher::FaceTable* SlotCacher::table = NULL;
FT_Library SlotCacher::lib = NULL;
FT_Face SlotCacher::defaultFont = NULL;
#define SL(x) ((int)(x) >> 6)

int TextBox::get_width() {
	return ABS(xMax - xMin);
}
int TextBox::get_height() {
	return ABS(yMax - yMin);
}
Rect TextBox::to_rect() {
	return Rect(0, 0, ABS(xMax - xMin), ABS(yMax - yMin));
}


FaceHandle::FaceHandle() 
	: face(0), cache(0), dir(HB_DIRECTION_LTR), gpos(0), ginfo(0), gcount(0U), fontSize(14U) {
}

size_t FaceHandle::get_max_height() {
	return cache ? cache->maxHeight : 0;
}

size_t FaceHandle::get_ch() {
	return cache ? cache->ch : 0;
}

SlotCache::SlotCache() 
	: font(NULL), buffer(NULL), maxHeight(0U), ch(0U) {
}

SlotCache::~SlotCache() {
	/*	map<hb_codepoint_t, PGlyphSlot>::iterator it;
		for (it = slots.begin(); it != slots.end(); it++) {
			PGlyphSlot& slot = it->second;
			if (slot->outline.points) free(slot->outline.points);
			if (slot->outline.contours) free(slot->outline.contours);
			if (slot->outline.tags) free(slot->outline.tags);
			if (slot->bitmap.buffer) free(slot->bitmap.buffer);
		}*/
}
PGlyphSlot SlotCache::get_slot(hb_codepoint_t codep) {
	return slots[codep];
}
void SlotCache::insert(hb_codepoint_t codep, PGlyphSlot slot) {
	slots[codep] = slot;
}

int force_ucs2_charmap(FT_Face ftf) {
	for (int i = 0; i < ftf->num_charmaps; i++) {
		if (((ftf->charmaps[i]->platform_id == 0)
			&& (ftf->charmaps[i]->encoding_id == 3))
			|| ((ftf->charmaps[i]->platform_id == 3)
				&& (ftf->charmaps[i]->encoding_id == 1))) {
			return FT_Set_Charmap(ftf, ftf->charmaps[i]);
		}
	}
	return -1;
}

SlotCacher::~SlotCacher() {
	for (auto it : *table){
		for (auto it2 : *it.second) {
			delete it2.second;
		}
	}
}

bool SlotCacher::check_ready() {
	if (!lib) FT_Init_FreeType(&lib);
	if (!fonts) fonts = new FontsMap();
	if (!table) table = new FaceTable();
	return IS_NOT_NULL(lib) && IS_NOT_NULL(fonts) && IS_NOT_NULL(table);
}

int SlotCacher::init() {
	std::wstring fontName = L"<default>";
	defaultFont = add_font_from_memory(fontName, fontsOpenSansRegularData, fontsOpenSansRegularSize);
	return (defaultFont == NULL ? 1 : 0);
}

FT_Face SlotCacher::add_font(std::wstring& name, std::wstring path) {
	FT_Face face;
	if (!check_ready()) return NULL;
	if (fonts->count(name)) {
		return fonts->at(name);
	}
	if (FT_New_Face(lib, ocom::to_string(path).c_str(), 0, &face)) {
		return NULL;
	}
	force_ucs2_charmap(face);
	fonts->insert({ name, face });
	return face;
}

FT_Face SlotCacher::add_font_from_memory(std::wstring& name, const byte* fontData, size_t fontSize) {
	FT_Face face;
	if (!check_ready()) return NULL;
	if (fonts->count(name)) {
		return fonts->at(name);
	}
	if (FT_New_Memory_Face(lib, fontData, (FT_Long)fontSize, 0, &face)) {
		return NULL;
	}
	force_ucs2_charmap(face);
	fonts->insert({ name, face });
	return face;
}

void SlotCacher::load_handle(std::wstring& fontName, size_t fontSize, FaceHandle& handle) {
	if (!lib) return;
	if (fonts->find(fontName) == fonts->end())
		handle.face = defaultFont;
	else
		handle.face = fonts->at(fontName);
	if (!handle.face) return;

	auto& face = handle.face;
	auto& box = handle.box;
	auto& cache = handle.cache;
	handle.fontSize = fontSize;

	if (table->find(face) == table->end()) {
		table->insert({ face, new std::map<size_t, SlotCache*>() });
	}
	auto row = table->at(face);
	if (row->find(fontSize) == row->end()) {
		row->insert({ fontSize, NULL });
	}
	cache = row->at(fontSize);
	if (cache == NULL) {
		cache = new SlotCache();
		cache->font = hb_ft_font_create(face, NULL);
		cache->buffer = hb_buffer_create();
		if (!hb_buffer_allocation_successful(cache->buffer) || !cache->font) {
			delete cache;
			return;
		}
		table->at(face)->at(fontSize) = cache;
		load_box8((char*)"0", 1, handle);
		cache->maxHeight = Max(ABS(box.yMax), ABS(box.yMin));
		cache->ch = box.get_height();
	}
}

void SlotCacher::get_text_box(FaceHandle& handle) {
	long ymx, ymn, xmx, xmn, sx, sy, ex, ey, posx, posy;
	size_t i;

	xmx = ymx = LONG_MIN;
	xmn = ymn = LONG_MAX;
	posx = posy = 0;
	PGlyphSlot slot;
	auto gcount = handle.gcount;
	auto ginfo = handle.ginfo;
	auto gpos = handle.gpos;
	auto& box = handle.box;

	for (i = 0; i < gcount; i++) {
		slot = load_glyph(ginfo[i].codepoint, handle);
		if (!slot) continue;

		sx = posx + SL(gpos[i].x_offset + slot->metrics.horiBearingX);
		ex = sx + slot->bitmap.width;
		sy = SL(gpos[i].y_offset + slot->metrics.horiBearingY);
		ey = sy - slot->bitmap.rows;

		xmn = Min(sx, xmn);
		xmx = Max(ex, xmx);
		ymn = Min(ey, ymn);
		ymx = Max(sy, ymx);

		posx += SL(gpos[i].x_advance);
		posy += SL(gpos[i].y_advance);
	}

	if (!gcount) {
		xmx = ymx = xmn = ymn = 0;
	}

	xmn = 0;
	box.xMin = xmn;
	box.xMax = xmx;
	box.yMin = ymn;
	box.yMax = ymx;
}

void SlotCacher::shape8(char* text, size_t len, FaceHandle& handle) {
	handle.gcount = 0;
	if (!lib || !text || !len) return;

	hb_font_t* font;
	hb_buffer_t* buffer;
	FT_Error e = FT_Set_Pixel_Sizes(handle.face, 0U, (FT_UInt)handle.fontSize);
	if (e) return;

	font = handle.cache->font;
	buffer = handle.cache->buffer;
	if (!font || !buffer) return;

	hb_buffer_reset(buffer);
	hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
	hb_buffer_set_language(buffer, hb_language_from_string("en", 2));
	hb_buffer_add_utf8(buffer, text, (int)len, 0U, (int)len);
	hb_shape(font, buffer, NULL, 0);
	handle.ginfo = hb_buffer_get_glyph_infos(buffer, &handle.gcount);
	handle.gpos = hb_buffer_get_glyph_positions(buffer, &handle.gcount);
}

void SlotCacher::shape16(wchar_t* text, size_t len, FaceHandle& handle) {
	handle.gcount = 0;
	if (!lib || !text || !len) return;

	hb_font_t* font;
	hb_buffer_t* buffer;
	FT_Error e = FT_Set_Pixel_Sizes(handle.face, 0U, (FT_UInt)handle.fontSize);
	if (e) return;

	font = handle.cache->font;
	buffer = handle.cache->buffer;
	if (!font || !buffer) return;

	hb_buffer_reset(buffer);
	hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
	hb_buffer_set_language(buffer, hb_language_from_string("en", 2));
	hb_buffer_add_utf16(buffer, (uint16_t*)text, (int)len, 0U, (int)len);
	hb_shape(font, buffer, NULL, 0);
	handle.ginfo = hb_buffer_get_glyph_infos(buffer, &handle.gcount);
	handle.gpos = hb_buffer_get_glyph_positions(buffer, &handle.gcount);
}

void SlotCacher::load_box8(char* text, size_t len, FaceHandle& handle) {
	shape8(text, len, handle);
	get_text_box(handle);
}

void SlotCacher::load_box16(wchar_t* text, size_t len, FaceHandle& handle) {
	shape16(text, len, handle);
	get_text_box(handle);
}

PGlyphSlot SlotCacher::load_glyph(hb_codepoint_t codep, FaceHandle& handle) {
	if (!lib) return NULL;
	auto cache = handle.cache;
	auto face = handle.face;
	if (!cache) return NULL;
	PGlyphSlot slot = cache->get_slot(codep);

	if (!slot) {
		FT_Error e = FT_Load_Glyph(face, codep, FT_LOAD_DEFAULT);
		if (e) return NULL;
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		FT_GlyphSlot src = face->glyph;
		slot = new GlyphSlot();
		slot->metrics = src->metrics;
		slot->outline = src->outline;
		slot->bitmap = src->bitmap;

		// clone slot pointers;
		slot->outline.points = new FT_Vector[slot->outline.n_points];
		slot->outline.contours = new short[slot->outline.n_contours];
		slot->outline.tags = new char[slot->outline.n_points];

		for (int i = 0; i < slot->outline.n_points; i++) {
			slot->outline.points[i] = FT_Vector(src->outline.points[i]);
			slot->outline.tags[i] = char(src->outline.tags[i]);
		}

		for (int i = 0; i < slot->outline.n_contours; i++) {
			slot->outline.contours[i] = short(src->outline.contours[i]);
		}

		int sz = slot->bitmap.rows * slot->bitmap.pitch;
		slot->bitmap.buffer = new byte[sz];
		slot->bitmap.palette = NULL;

		memcpy(slot->bitmap.buffer, src->bitmap.buffer, size_t(sz * sizeof(byte)));

		cache->insert(codep, slot);
	}

	return slot;
}