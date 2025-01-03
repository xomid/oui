//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.3
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
// Copyright (c) 2008 Rene Rebe <rene@exactcode.de> [arc parser code]
// Copyright (c) 2017 John Horigan <john@glyphic.com> [align_subpath()]
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// SVG path renderer.
//
//----------------------------------------------------------------------------

#include <stdio.h>
#include <cmath>
#include "agg_svg_path_renderer.h"
#include <string>
#include <map>

namespace agg
{
namespace svg
{
    //------------------------------------------------------------------------
    path_renderer::path_renderer() : path_renderer(NULL) {
    }

    //------------------------------------------------------------------------
    path_renderer::path_renderer(std::map<std::string, Color>* colors) :
        m_curved(m_storage),
        m_curved_count(m_curved),
        m_colors(colors),
        m_curved_stroked(m_curved_count),
        m_curved_stroked_trans(m_curved_stroked, m_transform),

        m_curved_trans(m_curved_count, m_transform),
        m_curved_trans_contour(m_curved_trans)
    {
        m_curved_trans_contour.auto_detect_orientation(false);
        left = top = width = height = -1;
        pl = pt = pr = pb = 0;
    }


    //------------------------------------------------------------------------
    void path_renderer::remove_all()
    {
        m_storage.remove_all();
        m_attr_storage.remove_all();
        m_attr_stack.remove_all();
        m_transform.reset();
    }

    //------------------------------------------------------------------------
    void path_renderer::begin_path()
    {
        push_attr();
        unsigned idx = m_storage.start_new_path();
        m_attr_storage.add(path_attributes(cur_attr(), idx));
    }

    //------------------------------------------------------------------------
    void path_renderer::end_path()
    {
        if(m_attr_storage.size() == 0) 
        {
            throw exception("end_path : The path was not begun");
        }
        path_attributes attr = cur_attr();
        unsigned idx = m_attr_storage[m_attr_storage.size() - 1].index;
        attr.index = idx;
        m_attr_storage[m_attr_storage.size() - 1] = attr;
        pop_attr();
    }

    //------------------------------------------------------------------------
    void path_renderer::move_to(double x, double y, bool rel)          // M, m
    {
        if(rel) m_storage.rel_to_abs(&x, &y);
        m_storage.move_to(x, y);
    }

    //------------------------------------------------------------------------
    void path_renderer::line_to(double x,  double y, bool rel)         // L, l
    {
        if(rel) m_storage.rel_to_abs(&x, &y);
        m_storage.line_to(x, y);
    }

    //------------------------------------------------------------------------
    void path_renderer::hline_to(double x, bool rel)                   // H, h
    {
        double x2 = 0.0;
        double y2 = 0.0;
        if(m_storage.total_vertices())
        {
            m_storage.vertex(m_storage.total_vertices() - 1, &x2, &y2);
            if(rel) x += x2;
            m_storage.line_to(x, y2);
        }
    }

    //------------------------------------------------------------------------
    void path_renderer::vline_to(double y, bool rel)                   // V, v
    {
        double x2 = 0.0;
        double y2 = 0.0;
        if(m_storage.total_vertices())
        {
            m_storage.vertex(m_storage.total_vertices() - 1, &x2, &y2);
            if(rel) y += y2;
            m_storage.line_to(x2, y);
        }
    }

    //------------------------------------------------------------------------
    void path_renderer::curve3(double x1, double y1,                   // Q, q
                               double x,  double y, bool rel)
    {
        if(rel) 
        {
            m_storage.rel_to_abs(&x1, &y1);
            m_storage.rel_to_abs(&x,  &y);
        }
        m_storage.curve3(x1, y1, x, y);
    }

    //------------------------------------------------------------------------
    void path_renderer::curve3(double x, double y, bool rel)           // T, t
    {
        if(rel) 
        {
            m_storage.curve3_rel(x, y);
        } else 
        {
            m_storage.curve3(x, y);
        }
    }

    //------------------------------------------------------------------------
    void path_renderer::curve4(double x1, double y1,                   // C, c
                               double x2, double y2, 
                               double x,  double y, bool rel)
    {
        if(rel) 
        {
            m_storage.rel_to_abs(&x1, &y1);
            m_storage.rel_to_abs(&x2, &y2);
            m_storage.rel_to_abs(&x,  &y);
        }
        m_storage.curve4(x1, y1, x2, y2, x, y);
    }

    //------------------------------------------------------------------------
    void path_renderer::curve4(double x2, double y2,                   // S, s
                               double x,  double y, bool rel)
    {
        if(rel) 
        {
            m_storage.curve4_rel(x2, y2, x, y);
        } else 
        {
            m_storage.curve4(x2, y2, x, y);
        }
    }

    //------------------------------------------------------------------------
    void path_renderer::arc(double rx, double ry,
                            double angle,
                            bool large_arc_flag,
                            bool sweep_flag,
                            double x, double y, bool rel)
    {
      angle = deg2rad (angle);
      if(rel)
      {
          m_storage.arc_rel(rx, ry, angle, large_arc_flag, sweep_flag, x, y);
      } else
      {
          m_storage.arc_to(rx, ry, angle, large_arc_flag, sweep_flag, x, y);
      }
    }

    //------------------------------------------------------------------------
    void path_renderer::close_subpath()
    {
        m_storage.end_poly(path_flags_close);
    }

    //------------------------------------------------------------------------
    // If the end points of a subpath are very, very close then make them
    // exactly equal so that AGG is not confused.
    //------------------------------------------------------------------------
    void path_renderer::align_subpath(unsigned start_idx)
    {
        m_storage.align_path(start_idx);
    }

    //------------------------------------------------------------------------
    path_attributes& path_renderer::cur_attr()
    {
        if(m_attr_stack.size() == 0)
        {
            throw exception("cur_attr : Attribute stack is empty");
        }
        return m_attr_stack[m_attr_stack.size() - 1];
    }

    //------------------------------------------------------------------------
    void path_renderer::push_attr()
    {
        m_attr_stack.add(m_attr_stack.size() ? 
                         m_attr_stack[m_attr_stack.size() - 1] :
                         path_attributes());
    }

    //------------------------------------------------------------------------
    void path_renderer::pop_attr()
    {
        if(m_attr_stack.size() == 0)
        {
            throw exception("pop_attr : Attribute stack is empty");
        }
        m_attr_stack.remove_last();
    }

    //------------------------------------------------------------------------
    void path_renderer::fill(const Color& f)
    {
        path_attributes& attr = cur_attr();
        attr.fill_color = f;
        attr.fill_flag = true;
    }

    //------------------------------------------------------------------------
    void path_renderer::fill(const char* f)
    {
        path_attributes& attr = cur_attr();
        attr.fill_scolor = (char*)f;
        attr.fill_flag = true;
    }

    //------------------------------------------------------------------------
    void path_renderer::stroke(const Color& s)
    {
        path_attributes& attr = cur_attr();
        attr.stroke_color = s;
        attr.stroke_flag = true;
    }

    //------------------------------------------------------------------------
    void path_renderer::stroke(const char* s)
    {
        path_attributes& attr = cur_attr();
        attr.stroke_scolor = (char*)s;
        attr.stroke_flag = true;
    }

    //------------------------------------------------------------------------
    void path_renderer::even_odd(bool flag)
    {
        cur_attr().even_odd_flag = flag;
    }
    
    //------------------------------------------------------------------------
    void path_renderer::stroke_width(double w)
    {
        cur_attr().stroke_width = w;
    }

    //------------------------------------------------------------------------
    void path_renderer::fill_none()
    {
        cur_attr().fill_flag = false;
    }

    //------------------------------------------------------------------------
    void path_renderer::stroke_none()
    {
        cur_attr().stroke_flag = false;
    }

    //------------------------------------------------------------------------
    void path_renderer::fill_opacity(double op)
    {
        int clamped_op = (int)round(op);
        clamped_op = clamped_op < 0 ? 0 : clamped_op > 255 ? 255 : clamped_op;
        cur_attr().fill_color.opacity((byte)clamped_op);
    }

    //------------------------------------------------------------------------
    void path_renderer::stroke_opacity(double op)
    {
        int clamped_op = (int)round(op);
        clamped_op = clamped_op < 0 ? 0 : clamped_op > 255 ? 255 : clamped_op;
        cur_attr().stroke_color.opacity((byte)clamped_op);
    }

    //------------------------------------------------------------------------
    void path_renderer::line_join(line_join_e join)
    {
        cur_attr().line_join = join;
    }

    //------------------------------------------------------------------------
    void path_renderer::line_cap(line_cap_e cap)
    {
        cur_attr().line_cap = cap;
    }

    //------------------------------------------------------------------------
    void path_renderer::miter_limit(double ml)
    {
        cur_attr().miter_limit = ml;
    }

    //------------------------------------------------------------------------
    trans_affine& path_renderer::transform()
    {
        return cur_attr().transform;
    }

    //------------------------------------------------------------------------
    void path_renderer::parse_path(path_tokenizer& tok)
    {
	unsigned move_idx = 0;
    char last_cmd = 'z';

        while(tok.next())
        {
            double arg[10];
            char cmd = tok.last_command();

	    if (m_storage.total_vertices() == 0 && !(cmd == 'M' || cmd == 'm')) 
	    {
		throw exception("path element data attributes must begin with a 'move to'");
	    }
            unsigned i;
            switch(cmd)
            {
                case 'M': case 'm':
		    align_subpath(move_idx);
                    arg[0] = tok.last_number();
                    arg[1] = tok.next(cmd);
                    if (last_cmd == 'm' && cmd == 'm') line_to(arg[0], arg[1], true);
                    else move_to(arg[0], arg[1], cmd == 'm');
		    move_idx = m_storage.total_vertices() - 1;
                    break;

                case 'L': case 'l':
                    arg[0] = tok.last_number();
                    arg[1] = tok.next(cmd);
                    line_to(arg[0], arg[1], cmd == 'l');
                    break;

                case 'V': case 'v':
                    vline_to(tok.last_number(), cmd == 'v');
                    break;

                case 'H': case 'h':
                    hline_to(tok.last_number(), cmd == 'h');
                    break;
                
                case 'Q': case 'q':
                    arg[0] = tok.last_number();
                    for(i = 1; i < 4; i++)
                    {
                        arg[i] = tok.next(cmd);
                    }
                    curve3(arg[0], arg[1], arg[2], arg[3], cmd == 'q');
                    break;

                case 'T': case 't':
                    arg[0] = tok.last_number();
                    arg[1] = tok.next(cmd);
                    curve3(arg[0], arg[1], cmd == 't');
                    break;

                case 'C': case 'c':
                    arg[0] = tok.last_number();
                    for(i = 1; i < 6; i++)
                    {
                        arg[i] = tok.next(cmd);
                    }
                    curve4(arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], cmd == 'c');
                    break;

                case 'S': case 's':
                    arg[0] = tok.last_number();
                    for(i = 1; i < 4; i++)
                    {
                        arg[i] = tok.next(cmd);
                    }
                    curve4(arg[0], arg[1], arg[2], arg[3], cmd == 's');
                    break;

                case 'A': case 'a':
                    arg[0] = tok.last_number();
                    for(i = 1; i < 7; ++i)
                    {
                        arg[i] = tok.next(cmd);
                    }
                    arc(arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6],
                        cmd == 'a');
                    break;

                case 'Z': case 'z':
		    align_subpath(move_idx);
                    //close_subpath();
                    break;

                default:
                {
                    char buf[100];
                    sprintf_s(buf, sizeof(buf), "parse_path: Invalid Command %c", cmd);
                    throw exception(buf);
                }
            }

            if (last_cmd != 'm' || cmd != 'm') last_cmd = cmd;
        }
        align_subpath(move_idx);
        m_storage.start_new_path();
    }

}
}

