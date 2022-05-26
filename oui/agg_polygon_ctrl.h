//----------------------------------------------------------------------------
// Anti-Grain Geometry (AGG) - Version 2.5
// A high quality rendering engine for C++
// Copyright (C) 2002-2006 Maxim Shemanarev
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://antigrain.com
// 
// AGG is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// AGG is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with AGG; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA 02110-1301, USA.
//----------------------------------------------------------------------------

#ifndef POLYGON_CTRL_INCLUDED
#define POLYGON_CTRL_INCLUDED

#include "agg_array.h"
#include "agg_conv_stroke.h"
#include "agg_ellipse.h"
#include "agg_color_rgba.h"

namespace agg
{
    class AGG_API simple_polygon_vertex_source
    {
    public:
        simple_polygon_vertex_source(const double* polygon, unsigned np, 
                                     bool roundoff = false,
                                     bool close = true) :
            m_polygon(polygon),
            m_num_points(np),
            m_vertex(0),
            m_roundoff(roundoff),
            m_close(close)
        {
        }

        void close(bool f) { m_close = f;    }
        bool close() const { return m_close; }

        void rewind(unsigned)
        {
            m_vertex = 0;
        }

        unsigned vertex(double* x, double* y)
        {
            if(m_vertex > m_num_points) return path_cmd_stop;
            if(m_vertex == m_num_points) 
            {
                ++m_vertex;
                return path_cmd_end_poly | (m_close ? path_flags_close : 0);
            }
            *x = m_polygon[m_vertex * 2];
            *y = m_polygon[m_vertex * 2 + 1];
            if(m_roundoff)
            {
                *x = floor(*x) + 0.5;
                *y = floor(*y) + 0.5;
            }
            ++m_vertex;
            return (m_vertex == 1) ? path_cmd_move_to : path_cmd_line_to;
        }

    private:
        const double* m_polygon;
        unsigned m_num_points;
        unsigned m_vertex;
        bool     m_roundoff;
        bool     m_close;
    };
}

#endif

