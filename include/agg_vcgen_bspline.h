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

#ifndef AGG_VCGEN_BSPLINE_INCLUDED
#define AGG_VCGEN_BSPLINE_INCLUDED

#include "agg_basics.h"
#include "agg_array.h"
#include "agg_bspline.h"


namespace agg
{

    //==========================================================vcgen_bspline
    class AGG_API vcgen_bspline
    {
        enum status_e
        {
            initial,
            ready,
            polygon,
            end_poly,
            stop
        };

    public:
        typedef pod_bvector<point_d, 6> vertex_storage;

        vcgen_bspline();

        void interpolation_step(double v) { m_interpolation_step = v; }
        double interpolation_step() const { return m_interpolation_step; }

        // Vertex Generator Interface
        void remove_all();
        void add_vertex(double x, double y, unsigned cmd);

        // Vertex Source Interface
        void     rewind(unsigned path_id);
        unsigned vertex(double* x, double* y);

    private:
        vcgen_bspline(const vcgen_bspline&);
        const vcgen_bspline& operator = (const vcgen_bspline&);

        vertex_storage m_src_vertices;
        bspline        m_spline_x;
        bspline        m_spline_y;
        double         m_interpolation_step;
        unsigned       m_closed;
        status_e       m_status;
        unsigned       m_src_vertex;
        double         m_cur_abscissa;
        double         m_max_abscissa;
    };

}


#endif

