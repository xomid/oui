#pragma once
#include "oui_visual_basics.h"
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include <harfbuzz/hb.h>

struct OUI_API GlyphSlot {
	FT_Glyph_Metrics  metrics;
	FT_Bitmap         bitmap;
	FT_Outline        outline;
};

struct OUI_API TextBox {
	signed long xMin, yMin;
	signed long xMax, yMax;
	int get_width();
	int get_height();
	Rect to_rect();
};

typedef GlyphSlot* PGlyphSlot;

struct OUI_API SlotCache {
	std::map<hb_codepoint_t, PGlyphSlot> slots;
	hb_font_t* font;
	hb_buffer_t* buffer;
	size_t maxHeight, ch;
	~SlotCache();
	PGlyphSlot get_slot(hb_codepoint_t codep);
	void insert(hb_codepoint_t codep, PGlyphSlot slot);
};

struct OUI_API FaceHandle {
	FT_Face face;
	SlotCache* cache;
	hb_direction_t dir;
	hb_glyph_position_t* gpos;
	hb_glyph_info_t* ginfo;
	TextBox box;
	uint32_t gcount;
	size_t fontSize;
	size_t get_max_height();
	size_t get_ch();
};

class OUI_API SlotCacher
{
public:
	SlotCacher();
	~SlotCacher();

	typedef std::map<std::wstring, FT_Face> FontsMap;
	typedef std::map<FT_Face, std::map<size_t, SlotCache*>*> FaceTable;

	static FontsMap* fonts;
	static FaceTable* table;
	static FT_Library lib;
	static FT_Face defaultFont;

	static bool check_ready();
	static int init();
	static FT_Face add_font_from_memory(std::wstring& name, const byte* fontData, size_t fontSize);
	static FT_Face add_font(std::wstring& name, std::wstring path);
	static void shape8(char* text, size_t len, FaceHandle& handle);
	static void shape16(wchar_t* text, size_t len, FaceHandle& handle);
	static void load_box8(char* text, size_t len, FaceHandle& handle);
	static void load_box16(wchar_t* text, size_t len, FaceHandle& handle);
	static void get_text_box(FaceHandle& handle);
	static PGlyphSlot load_glyph(hb_codepoint_t codep, FaceHandle& handle);

	static void load_handle(std::wstring& fontName, size_t fontSize, FaceHandle& handle);
};