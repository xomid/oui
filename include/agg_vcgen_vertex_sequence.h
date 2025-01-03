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

#ifndef AGG_VCGEN_VERTEX_SEQUENCE_INCLUDED
#define AGG_VCGEN_VERTEX_SEQUENCE_INCLUDED

#include "agg_basics.h"
#include "agg_vertex_sequence.h"
#include "agg_shorten_path.h"

namespace agg
{

    //===================================================vcgen_vertex_sequence
    class AGG_API vcgen_vertex_sequence
    {
    public:
        typedef vertex_dist_cmd                 vertex_type;
        typedef vertex_sequence<vertex_type, 6> vertex_storage;

        vcgen_vertex_sequence() :
            m_flags(0),
            m_cur_vertex(0),
            m_shorten(0.0),
            m_ready(false)
        {
        }

        // Vertex Generator Interface
        void remove_all();
        void add_vertex(double x, double y, unsigned cmd);

        // Vertex Source Interface
        void     rewind(unsigned path_id);
        unsigned vertex(double* x, double* y);

        void shorten(double s) { m_shorten = s; }
        double shorten() const { return m_shorten; }

    private:
        vcgen_vertex_sequence(const vcgen_vertex_sequence&);
        const vcgen_vertex_sequence& operator = (const vcgen_vertex_sequence&);

        vertex_storage m_src_vertices;
        unsigned       m_flags;
        unsigned       m_cur_vertex;
        double         m_shorten;
        bool           m_ready;
    };


    //------------------------------------------------------------------------
    inline void vcgen_vertex_sequence::remove_all()
    {
        m_ready = false;
        m_src_vertices.remove_all();
        m_cur_vertex = 0;
        m_flags = 0;
    }

    //------------------------------------------------------------------------
    inline void vcgen_vertex_sequence::add_vertex(double x, double y, unsigned cmd)
    {
        m_ready = false;
        if(is_move_to(cmd))
        {
            m_src_vertices.modify_last(vertex_dist_cmd(x, y, cmd));
        }
        else
        {
            if(is_vertex(cmd))
            {
                m_src_vertices.add(vertex_dist_cmd(x, y, cmd));
            }
            else
            {
                m_flags = cmd & path_flags_mask;
            }
        }
    }


    //------------------------------------------------------------------------
    inline void vcgen_vertex_sequence::rewind(unsigned) 
    { 
        if(!m_ready)
        {
            m_src_vertices.close(is_closed(m_flags));
            shorten_path(m_src_vertices, m_shorten, get_close_flag(m_flags));
        }
        m_ready = true;
        m_cur_vertex = 0; 
    }

    //------------------------------------------------------------------------
    inline unsigned vcgen_vertex_sequence::vertex(double* x, double* y)
    {
        if(!m_ready)
        {
            rewind(0);
        }

        if(m_cur_vertex == m_src_vertices.size())
        {
            ++m_cur_vertex;
            return path_cmd_end_poly | m_flags;
        }

        if(m_cur_vertex > m_src_vertices.size())
        {
            return path_cmd_stop;
        }

        vertex_type& v = m_src_vertices[m_cur_vertex++];
        *x = v.x;
        *y = v.y;
        return v.cmd;
    }


}

#endif
