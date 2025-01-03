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

#ifndef AGG_ELLIPSE_BRESENHAM_INCLUDED
#define AGG_ELLIPSE_BRESENHAM_INCLUDED


#include "agg_basics.h"


namespace agg
{

    //------------------------------------------ellipse_bresenham_interpolator
    class AGG_API ellipse_bresenham_interpolator
    {
    public:
        ellipse_bresenham_interpolator(int rx, int ry) :
            m_rx2(rx * rx),
            m_ry2(ry * ry),
            m_two_rx2(m_rx2 << 1),
            m_two_ry2(m_ry2 << 1),
            m_dx(0),
            m_dy(0),
            m_inc_x(0),
            m_inc_y(-ry * m_two_rx2),
            m_cur_f(0)
        {}
        
        int dx() const { return m_dx; }
        int dy() const { return m_dy; }

        void operator++ ()
        {
            int  mx, my, mxy, min_m;
            int  fx, fy, fxy;

            mx = fx = m_cur_f + m_inc_x + m_ry2;
            if(mx < 0) mx = -mx;

            my = fy = m_cur_f + m_inc_y + m_rx2;
            if(my < 0) my = -my;

            mxy = fxy = m_cur_f + m_inc_x + m_ry2 + m_inc_y + m_rx2;
            if(mxy < 0) mxy = -mxy;

            min_m = mx; 
            bool flag = true;

            if(min_m > my)  
            { 
                min_m = my; 
                flag = false; 
            }

            m_dx = m_dy = 0;

            if(min_m > mxy) 
            { 
                m_inc_x += m_two_ry2;
                m_inc_y += m_two_rx2;
                m_cur_f = fxy;
                m_dx = 1; 
                m_dy = 1;
                return;
            }

            if(flag) 
            {
                m_inc_x += m_two_ry2;
                m_cur_f = fx;
                m_dx = 1;
                return;
            }

            m_inc_y += m_two_rx2;
            m_cur_f = fy;
            m_dy = 1;
        }

    private:
        int m_rx2;
        int m_ry2;
        int m_two_rx2;
        int m_two_ry2;
        int m_dx;
        int m_dy;
        int m_inc_x;
        int m_inc_y;
        int m_cur_f;

    };

}

#endif

