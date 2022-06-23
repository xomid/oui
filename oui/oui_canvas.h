#pragma once
#include "agg_svg_parser.h"
#include "oui_visual.h"
#include "oui_slot_cacher.h"

class OUI_API Canvas
{
private:
	inline void _draw_glyphs(int penLeft, int penTop, Rect* rc);
	inline void get_size8(char* text, size_t len, Size* size, bool multiLine);
	inline void get_size16(wchar_t* text, size_t len, Size* size, bool multiLine);
public:
	Sheet* sheet;
	Rect* area;
	Art art;
	scanline sl;
	rasterizer_scanline ras;
	double lineHeight;
	FaceHandle faceHandle;

	Canvas();
	Canvas(Rect* area, Sheet* sheet);
	~Canvas();

	Sheet* set_sheet(Sheet* newSheet);
	Rect* set_area(Rect* newArea);

	void set(Rect* newArea, Sheet* sheet);
	void clear(Rect* rect, const Color* color);
	void clear_opaque(Rect* rect, Color& color);
	void clear(Rect& rect, Border& bor, Color& background, byte opacity);
	void clear_bystencil(Rect* Area, Border* bor, Color* background, rasterizer_scanline& stencil_ras, scanline& stencil_sl);
	void draw_rounded_rect(Rect* rect, size_t radius, Color* color);
	void fill_rounded_rect(Rect* rect, size_t radius, Color* color);
	void draw_box_model(Rect* rect, Spacing* margin, Spacing* padding, Border* border);
	void draw_vertical_line(int x, int y0, int y1, size_t lineWidth);
	void draw_horizontal_line(int y, int x0, int x1, size_t lineWidth);
	void draw_line(int x0, int y0, int x1, int y1);
	void draw_box(Rect& rect, Border border, byte opacity);
	void bit_blt(Sheet& srcSheet, int x, int y, int width, int height, int srcLeft, int srcTop, bool useStencil);
	void bit_blt(Sheet& srcSheet, int x, int y, int width, int height, int srcLeft, int srcTop, Color& color);
	void clear_solid(ShapeStorage& shape, int offsetX, int offsetY, Color& color);
	void clear_solid(trans_shape& shape, Color& color);
	void reset();
	void render(Color color, byte opacity = 0xff);
	void render(rasterizer_scanline& ras, scanline& sl, Color color, byte opacity = 0xff);
	void render_svg(agg::svg::path_renderer* path, int left, int top, int width, int height, byte opacity, Spacing* extraPadding = NULL, agg::trans_affine* overlayMatrix = NULL);
	void render_svg_origin(agg::svg::path_renderer* path, int centerX, int centerY, double scale, byte opacity, agg::trans_affine* overlayMatrix = NULL);

	template<class VertexSource>
	void add_path(VertexSource& vs, unsigned path_id = 0) {
		ras.add_path(vs, path_id);
	}

	// TEXT Related stuff
	TextBox* get_box8(char* text, size_t len);
	TextBox* get_box16(wchar_t* text, size_t len);

	int draw_text8(char* text, Rect& rc, bool multiLine = true);
	int draw_text8(std::string& text, Rect& rc, bool multiLine = true);
	int draw_text8(char* text, Rect& rc, std::vector<ColoredSpan>* spans, bool multiLine = true);
	int draw_label8(char* text, size_t len, Rect& rect, BorderRadius& borderRadius, Spacing& padd, Border& bord, Color& fill);
	int draw_text8(char* text, size_t len, int penLeft, int penTop, bool multiLine = true);

	int draw_text16(wchar_t* text, Rect& rc, bool multiLine = true);
	int draw_text16(std::wstring& text, Rect& rc, bool multiLine = true);
	int draw_text16(wchar_t* text, Rect& rc, std::vector<ColoredSpan>* spans, bool multiLine = true);
	int draw_label16(wchar_t* text, size_t len, Rect& rect, BorderRadius& borderRadius, Spacing& padd, Border& bord, Color& fill);
	int draw_text16(wchar_t* text, size_t len, int penLeft, int penTop, bool multiLine = true);

	int draw_tag8(char* text, size_t len, int x, int y, int dir, bool invert, int arrowW, int arrowH, BorderRadius& borderRadius, Spacing& padd, Color& fill);
	int draw_tag16(wchar_t* text, size_t len, int x, int y, int dir, bool invert, int arrowW, int arrowH, BorderRadius& borderRadius, Spacing& padd, Color& fill);

	void set_line_height(double lineHeight);
	size_t get_ch();
	size_t get_max_height();
	FT_Face add_font(std::wstring name, std::wstring path);
	void set_color(const Color& crText);

	void load_handle(std::wstring& fontName, int fontSize) {
		SlotCacher::load_handle(fontName, fontSize, faceHandle);
	}
};