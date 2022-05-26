#pragma once
#include "agg_basics.h"
#include "agg_bspline.h"
#include "agg_conv_bspline.h"
#include "agg_polygon_ctrl.h"
#include "agg_rendering_buffer.h"
#include "agg_conv_transform.h"
#include "agg_conv_stroke.h"
#include "agg_conv_clip_polyline.h"
#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"
#include "agg_rasterizer_outline_aa.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_pattern_filters_rgba.h"
#include "agg_renderer_outline_aa.h"
#include "agg_renderer_outline_image.h"
#include "agg_pixfmt_rgb.h"
#include "agg_pixfmt_rgba.h"
#include "agg_curves.h"
#include "agg_trans_affine.h"
#include "agg_scanline_u.h"
#include "agg_span_interpolator_linear.h"
#include "agg_span_allocator.h"
#include "agg_span_gradient.h"
#include "agg_conv_dash.h"
#include "agg_conv_stroke.h"
#include "agg_span_gouraud_rgba.h"
#include "agg_span_solid.h"
#include "agg_span_gouraud_gray.h"
#include "agg_span_allocator.h"
#include "agg_path_storage.h"
#include "agg_pixfmt_rgb.h"
#include "agg_conv_curve.h"
#include "agg_rounded_rect.h"
#include "agg_pixfmt_gray.h"

#include "oui_visual_basics.h"
struct OUI_API Sheet;

//==================================================pixfmt_alpha_blend_rgb_clip
template<class Blender, class RenBuf, unsigned Step, unsigned Offset = 0>
class pixfmt_alpha_blend_rgb_clip
{
public:
    typedef agg::pixfmt_rgb_tag pixfmt_category;
    typedef RenBuf   rbuf_type;
    typedef Blender  blender_type;
    typedef typename rbuf_type::row_data row_data;
    typedef typename blender_type::color_type color_type;
    typedef typename blender_type::order_type order_type;
    typedef typename color_type::value_type value_type;
    typedef typename color_type::calc_type calc_type;
   
public:
    explicit pixfmt_alpha_blend_rgb_clip(Sheet* sheet, Rect* area) : sheet(sheet), area(area) {
    }

    AGG_INLINE unsigned width()  const { return sheet ? sheet->w : 0; }
    AGG_INLINE unsigned height() const { return sheet ? sheet->h : 0; }
    AGG_INLINE int      stride() const { return sheet ? sheet->pitch : 0; }
    AGG_INLINE int      get_nbpp() const { return sheet ? sheet->nbpp : 0; }

    AGG_INLINE void copy_hline(int x, int y, unsigned len, const color_type& c) {
        copy_hline(x, y, len, Color(c.r, c.g, c.b, c.a));
    }

    AGG_INLINE void copy_hline(int x, int y, unsigned len, Color& c)  {
        if (!sheet) return;
        int count;
        byte cla, _cla, * d, r, g, b, red, grn, blu, alpha, calpha, a, _a;
        alpha = c.a;
        red = c.r;
        grn = c.g;
        blu = c.b;
        calpha = 0xff - alpha;
        if (area) {
            x += area->left;
            y += area->top;
        }

        if (x < 0) {
            len += x;
            x += -x;
            if (len < 1) return;
        }
        if (y < 0 || y >= sheet->h || x >= sheet->w) return;

        d = sheet->data + y * sheet->pitch + x * sheet->nbpp;
        sheet->clip(x, y, &count, &cla);
        _cla = 0xff - cla;

        do
        {
            d[0] = CLAMP255(DIV255(blu * cla + (d[0] * _cla)));
            d[1] = CLAMP255(DIV255(grn * cla + (d[1] * _cla)));
            d[2] = CLAMP255(DIV255(red * cla + (d[2] * _cla)));
 
            d += 3;
            if (++x >= sheet->w) return;
            if (!--count)
                sheet->clip(x, &count, &cla);
        } while (--len);
    }

    void blend_hline(int x, int y, int len, const color_type& c, agg::int8u cover) {
        Color cr(c.r, c.g, c.b, c.a);
        blend_hline(x, y, len, cr, cover);
    }
    //--------------------------------------------------------------------
    void blend_hline(int x, int y, int len, Color& c, agg::int8u cover) {
        if (!sheet) return;
        int count;
        byte cla, _cla, * d, red, grn, blu, alpha, calpha, a, _a;
        red = c.r;
        grn = c.g;
        blu = c.b;
        alpha = c.a;
        calpha = 0xff - alpha;
        if (area) {
            x += area->left;
            y += area->top;
        }

        if (x < 0) {
            len += x;
            x += -x;
            if (len < 1) return;
        }
        if (y < 0 || y >= sheet->h || x >= sheet->w) return;

        d = sheet->data + y * sheet->pitch + x * sheet->nbpp;
        sheet->clip(x, y, &count, &cla);
        _cla = 0xff - cla;

        do
        {
            if (cover == agg::cover_mask && alpha == 0xff)
            {
                a = cla;
                _a = 0xff - cla;

                d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
                d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
                d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
            }
            else
            {
                a = DIV255(DIV255(alpha * cover) * cla);
                _a = 0xff - a;

                d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
                d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
                d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
            }
            d += 3;
            if (++x >= sheet->w) return;
            if (!--count)
                sheet->clip(x, &count, &cla);
        } while (--len);
    }

    void blend_solid_hspan(int x, int y, int len, const color_type& c, const agg::int8u* covers) {
        Color cr(c.r, c.g, c.b, c.a);
        blend_solid_hspan(x, y, len, cr, covers);
    }
    //--------------------------------------------------------------------
    void blend_solid_hspan(int x, int y, int len, const Color& c, const agg::int8u* covers) {
        if (!sheet) return;
        int count;
        byte cla, _cla, * d, red, grn, blu, alpha, calpha, a, _a;
        red = c.r;
        grn = c.g;
        blu = c.b;
        alpha = c.a;
        calpha = 0xff - alpha;
        if (area) {
            x += area->left;
            y += area->top;
        }

        if (x < 0) {
            len += x;
            x += -x;
            if (len < 1) return;
        }
        if (y < 0 || y >= sheet->h || x >= sheet->w) return;

        d = sheet->data + y * sheet->pitch + x * sheet->nbpp;
        sheet->clip(x, y, &count, &cla);
        _cla = 0xff - cla;

        do
        {
            if (*covers == agg::cover_mask && alpha == 0xff)
            {
                a = cla;
                _a = 0xff - cla;

                d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
                d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
                d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
            }
            else
            {
                a = DIV255(DIV255(alpha * *covers) * cla);
                _a = 0xff - a;

                d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
                d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
                d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
            }
            d += 3;
            ++covers;
            if (++x >= sheet->w) return;
            if (!--count)
                sheet->clip(x, &count, &cla);
        } while (--len);
    }

    void blend_solid_vspan(int x, int y, int len, const color_type& c, const agg::int8u* covers) {
        Color cr(c.r, c.g, c.b, c.a);
        blend_solid_vspan(x, y, len, cr, covers);
    }
    void blend_solid_vspan(int x, int y, int len, const Color& c, const agg::int8u* covers) {
        if (!sheet) return;
        int count;
        byte cla, _cla, * d, red, grn, blu, alpha, calpha, a, _a;

        red = c.r;
        grn = c.g;
        blu = c.b;
        alpha = c.a;
        calpha = 0xff - alpha;
        if (area) {
            x += area->left;
            y += area->top;
        }

        if (y < 0) {
            len += y;
            y += -y;
            if (len < 1) return;
        }
        if (x < 0 || x >= sheet->w || y >= sheet->h) return;

        d = sheet->data + y * sheet->pitch + x * sheet->nbpp;
        sheet->clip(x, y, &count, &cla);
        _cla = 0xff - cla;

        do
        {
            if (*covers == agg::cover_mask && alpha == 0xff)
            {
                a = cla;
                _a = 0xff - cla;

                d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
                d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
                d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
            }
            else
            {
                a = DIV255(DIV255(alpha * *covers) * cla);
                _a = 0xff - a;

                d[0] = CLAMP255(DIV255(blu * a + (d[0] * _a)));
                d[1] = CLAMP255(DIV255(grn * a + (d[1] * _a)));
                d[2] = CLAMP255(DIV255(red * a + (d[2] * _a)));
            }
            d += sheet->pitch;
            ++covers;
            if (++y >= sheet->h) return;
            sheet->clip(x, y, &count, &cla);
        } while (--len);
    }

    void clear_hline_gray(int x, int y, int len, agg::int8u cover) {
        if (!sheet) return;
        byte* d;
        if (area) {
            x += area->left;
            y += area->top;
        }

        if (x < 0) {
            len += x;
            x += -x;
            if (len < 1) return;
        }
        if (y < 0 || y >= sheet->h || x >= sheet->w) return;
        d = sheet->data + y * sheet->pitch + x * sheet->nbpp;

        do
        {
            if (cover == agg::cover_mask) d[0] = 0;
            else d[0] = DIV255((0xff - cover) * d[0]);
            ++d;
            if (++x >= sheet->w) return;
        } while (--len);
    }

    void clear_solid_hspan_gray(int x, int y, int len, const agg::int8u* covers) {
        if (!sheet) return;
        byte* d;
        if (area) {
            x += area->left;
            y += area->top;
        }

        if (x < 0) {
            len += x;
            x += -x;
            if (len < 1) return;
        }
        if (y < 0 || y >= sheet->h || x >= sheet->w) return;
        d = sheet->data + y * sheet->pitch + x * sheet->nbpp;

        do
        {
            if (*covers == agg::cover_mask) d[0] = 0;
            else d[0] = DIV255((0xff - *covers) * d[0]);
            ++d;
            ++covers;
            if (++x >= sheet->w) return;
        } while (--len);
    }

    void clear_hline_bgra(int x, int y, int len, Color& c, agg::int8u cover) {
        if (!sheet) return;
        byte* d, r, g, b;
        r = c.r;
        g = c.g;
        b = c.b;
        if (area) {
            x += area->left;
            y += area->top;
        }

        if (x < 0) {
            len += x;
            x += -x;
            if (len < 1) return;
        }
        if (y < 0 || y >= sheet->h || x >= sheet->w) return;
        d = sheet->data + y * sheet->pitch + x * sheet->nbpp;

        if (c.a == 0) {
            do
            {
                if (cover == agg::cover_mask) d[3] = 0;
                else d[3] = DIV255((0xff - cover) * d[3]);
                d += 4;
                if (++x >= sheet->w) return;
            } while (--len);
        }
        else {
            do
            {
                d[0] = b, d[2] = g, d[2] = r;
                if (cover == agg::cover_mask) d[3] = 0;
                else d[3] = DIV255((0xff - cover) * d[3]);
                d += 4;
                if (++x >= sheet->w) return;
            } while (--len);
        }
    }

    void clear_solid_hspan_bgra(int x, int y, int len, Color& c, const agg::int8u* covers) {
        if (!sheet) return;
        byte* d, r, g, b;
        r = c.r;
        g = c.g;
        b = c.b;
        if (area) {
            x += area->left;
            y += area->top;
        }

        if (x < 0) {
            len += x;
            x += -x;
            if (len < 1) return;
        }
        if (y < 0 || y >= sheet->h || x >= sheet->w) return;
        d = sheet->data + y * sheet->pitch + x * sheet->nbpp;

        if (c.a == 0) {
            do
            {
                if (*covers == agg::cover_mask) d[3] = 0;
                else d[3] = DIV255((0xff - *covers) * d[3]);
                d += 4;
                ++covers;
                if (++x >= sheet->w) return;
            } while (--len);
        }
        else {
            do
            {
                d[0] = b, d[2] = g, d[2] = r;
                if (*covers == agg::cover_mask) d[3] = 0;
                else d[3] = DIV255((0xff - *covers) * d[3]);
                d += 4;
                ++covers;
                if (++x >= sheet->w) return;
            } while (--len);
        }
    }

private:
    Sheet* sheet;
    Rect*  area;
};

typedef pixfmt_alpha_blend_rgb_clip<agg::blender_bgr24, agg::rendering_buffer, 3> pixfmt_bgr;

typedef agg::scanline_u8 scanline;
typedef agg::rasterizer_scanline_aa<> rasterizer_scanline;
typedef agg::renderer_base<pixfmt_bgr> renderer_base_type;
typedef agg::renderer_base<pixfmt_bgr> renderer_base;
typedef agg::renderer_base<pixfmt_bgr> base_ren_type;
typedef agg::renderer_outline_aa<base_ren_type> renderer_type;
typedef agg::rasterizer_outline_aa<renderer_type> rasterizer_type;
typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_scanline;

typedef agg::curve4 acurve;
typedef agg::conv_curve<agg::path_storage> curved;
typedef agg::conv_stroke<curved>      curved_stroked;
typedef agg::conv_transform<agg::path_storage, agg::trans_affine> trans_path;

typedef agg::pixfmt_gray8 pixfmt_gray;
typedef pixfmt_gray::color_type color_type_gray;
typedef agg::renderer_base<pixfmt_gray> renderer_base_gray;
typedef agg::renderer_scanline_aa_solid<renderer_base_gray> renderer_scanline_gray;

#define OUI_GRAY 1
#define OUI_BGR 3
#define OUI_BGRA 4
#define OUI_DEFAULT_PIXELFORMAT OUI_BGR

enum class OUI_API ShapeType {
    shape_type_rect,
    shape_type_polygon,
    shape_type_rounded_rect
};

class OUI_API ShapeStorage {
public:
    // rounded rect stuff
    double x1, y1, x2, y2, rx1, ry1, rx2, ry2, rx3, ry3, rx4, ry4, srx1, sry1, srx2, sry2, srx3, sry3, srx4, sry4, spreadX, spreadY;
    unsigned status, currIndex;
    agg::arc arc;

    byte opacity;
    curved curve;
    scanline sl;
    rasterizer_scanline ras;

    agg::path_storage poly;
    ShapeType type;
    agg::trans_affine mtx;
    bool applyMat;
    byte alphaY;
    int minx, miny, maxx, maxy;
    bool negate;

    ShapeStorage();

    void rounded_rect(double x1, double y1, double w, double h, BorderRadius& borderRadius);

    void rounded_rect(double x1, double y1, double w, double h, BorderRadius& borderRadius, Border& border);

    void recti(int x, int y, int w, int h);

    void finish();

    void rectf(double x1, double y1, double x2, double y2);

    void radius(double r);

    void radius(double rx, double ry);

    void radius(double rx_bottom, double ry_bottom, double rx_top, double ry_top);

    void radius(double rx1, double ry1, double rx2, double ry2, double rx3, double ry3, double rx4, double ry4);

    void normalize_radius();

    void rewind(unsigned r);

    unsigned vertex(unsigned index, double* x, double* y);

    unsigned vertex(double* x, double* y);

    void set_matrix(agg::trans_affine mtx);

    void remove_matrix();

    void spread(double spreadX, double spreadY, bool brender = true);

    bool is_rect();

    bool is_rounded_rect();
};

typedef agg::conv_transform<ShapeStorage, agg::trans_affine> trans_shape;

struct OUI_API Sheet
{
    int w, h, sw, sh, nbpp, pitch, temp, pitexcess, lastY;
    pyte data;
    std::vector<ShapeStorage*> clipArea;
    ShapeStorage* curr;
    scanline::const_iterator span;
    unsigned num_spans;
    veci offset;
    double x1, x2, y1, y2;

    Sheet();
    void free();
    void setclip(ShapeStorage* shape, byte opacity, bool reverse = false);
    void unclip();
    void clear_solid(Color& color);
    void resize(int cx, int cy);
    void destroy();
    void create(int width, int height, int nbpp);
    void clear(byte v = 0);
    void clear(byte red, byte green, byte blue);
    bool getPixel(int x, int y, byte* res);
    bool getPixel(int x, int y, byte* r, byte* g, byte* b);

    inline void __rect_alpha(int x, double x0, double x1, int w, byte& a, int& c) {
        int _x = x << 8;
        int _x0s = int(x0) << 8;
        if (_x < _x0s) {
            a = 0;
            c = (_x0s - _x) >> 8;
        }
        else {
            int _x0m = int(x0 * 256);
            int _x0e = (int(_x0m + 0xff) >> 8) << 8;
            if (_x < _x0e) {
                c = 1;
                a = CLAMP255(_x0e - 1 - _x0m);
            }
            else {
                int _x1s = int(x1) << 8;//floor
                if (_x < _x1s) {
                    a = 0xff;
                    c = (_x1s - _x) >> 8;
                }
                else {
                    int _x1m = int(x1 * 256);
                    int _x1e = (int(_x1m + 0xff) >> 8) << 8;
                    if (_x < _x1e) {
                        c = 1;
                        a = CLAMP255(_x1e - 1 - _x1m);
                    }
                    else {
                        a = 0;
                        c = w - x;
                    }
                }
            }
        }
    }

    inline void clip(int& x, int& y, int* count, byte* alpha) {
        if (curr == NULL) {
            *count = w;
            *alpha = 0xff;
            return;
        }

        *alpha = 0xff;
        *count = w - x;
        lastY = y;

        for (auto it = clipArea.rbegin(), end = clipArea.rend(); it != end; ++it) {
            auto curr = *it;
            byte a = 0;
            int c = w - x;

            if (curr->type == ShapeType::shape_type_rect) {
                if (y < curr->miny || y >curr->maxy) {
                    curr->alphaY = 0;
                    c = w - x;
                    a = 0;
                }
                else {
                    y1 = curr->y1;
                    y2 = curr->y2;
                    x1 = curr->x1;
                    x2 = curr->x2;
                    if (curr->applyMat) {
                        curr->mtx.transform(&x1, &y1);
                        curr->mtx.transform(&x2, &y2);
                    }
                    __rect_alpha(y, y1, y2, h, curr->alphaY, c);
                    if (curr->alphaY == 0) {
                        a = 0;
                        c = w - x;
                    }
                    else {
                        __rect_alpha(x, x1, x2, w, a, c);
                        a = DIV255(a * curr->alphaY);
                    }
                }
            }
            else {
                if ((y >= curr->miny && y <= curr->maxy) && (curr->ras.rewind_scanlines()))
                {
                    curr->sl.reset(curr->minx, curr->maxx);
                    curr->ras.navigate_scanline(y);
                    if (curr->ras.sweep_scanline(curr->sl)) {
                        num_spans = curr->sl.num_spans();
                        span = curr->sl.begin();

                        if (x < span->x) {
                            c = span->x - x;
                            if (c < 1)
                                return;
                            a = 0;
                        }
                        else {
                            while (num_spans > 0) {
                                int len = span->len;
                                if (len <= 0) len = (unsigned)(span->x - span->len - 1);
                                if (len < 1)
                                    return;
                                if (x >= span->x && x < span->x + len) break;
                                ++span;
                                --num_spans;
                            }

                            if (num_spans > 0) {
                                int len = span->len;
                                if (len <= 0) len = (unsigned)(span->x - span->len - 1);

                                if (x > span->x + len)
                                {
                                    c = w - x;
                                    a = 0;
                                }
                                else if (span->len > 0)
                                {
                                    c = 1;
                                    a = *(span->covers + x - span->x);
                                }
                                else
                                {
                                    c = (unsigned)(span->x - span->len - 1);
                                    a = *(span->covers);
                                    if (c < 1)
                                        return;
                                }
                            }
                        }
                    }
                }
            }

            if (curr->negate) a = 0xff - a;
            *count = Min(c, *count);
            *alpha = CLAMP255(DIV255(a * *alpha));
        }

        *count = Max(1, *count);
    }

    inline void clip(int& x, int* count, byte* alpha) {
        if (curr == NULL) {
            *count = w;
            *alpha = 0xff;
            return;
        }

        *alpha = 0xff;
        *count = w - x;

        for (auto it = clipArea.rbegin(), end = clipArea.rend(); it != end; ++it) {
            auto curr = *it;
            byte a = 0;
            int c = w - x;

            if (curr->type == ShapeType::shape_type_rect) {
                if (curr->alphaY == 0) {
                    a = 0;
                    c = w - x;
                }
                else {
                    x1 = curr->x1;
                    x2 = curr->x2;
                    if (curr->applyMat) {
                        curr->mtx.transform(&x1, &y1);
                        curr->mtx.transform(&x2, &y2);
                    }
                    __rect_alpha(x, x1, x2, w, a, c);
                    a = DIV255(a * curr->alphaY);
                }
            }
            else {
                if ((lastY >= curr->miny && lastY <= curr->maxy) && (curr->ras.rewind_scanlines()))
                {
                    curr->sl.reset(curr->minx, curr->maxx);
                    curr->ras.navigate_scanline(lastY);
                    if (curr->ras.sweep_scanline(curr->sl)) {
                        num_spans = curr->sl.num_spans();
                        span = curr->sl.begin();

                        if (x < span->x) {
                            c = span->x - x;
                            if (c < 1)
                                return;
                            a = 0;
                        }
                        else {
                            while (num_spans > 0) {
                                int len = span->len;
                                if (len <= 0) len = (unsigned)(span->x - span->len - 1);
                                if (len < 1)
                                    return;
                                if (x >= span->x && x < span->x + len) break;
                                ++span;
                                --num_spans;
                            }

                            if (num_spans > 0) {
                                int len = span->len;
                                if (len <= 0) len = (unsigned)(span->x - span->len - 1);

                                if (x > span->x + len)
                                {
                                    c = w - x;
                                    a = 0;
                                }
                                else if (span->len > 0)
                                {
                                    c = 1;
                                    a = *(span->covers + x - span->x);
                                }
                                else
                                {
                                    c = (unsigned)(span->x - span->len - 1);
                                    a = *(span->covers);
                                    if (c < 1)
                                        return;
                                }
                            }
                        }
                    }
                }
            }

            if (curr->negate) a = 0xff - a;
            *count = Min(c, *count);
            if (a == 0) {
                *alpha = 0;
                break;
            }
            *alpha = CLAMP255(DIV255(a * *alpha));
        }

        *count = Max(1, *count);
    }
};