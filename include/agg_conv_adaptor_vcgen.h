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

#ifndef AGG_CONV_ADAPTOR_VCGEN_INCLUDED
#define AGG_CONV_ADAPTOR_VCGEN_INCLUDED

#include "agg_basics.h"

namespace agg
{
    //------------------------------------------------------------NULL_markers
    struct AGG_API NULL_markers
    {
        void remove_all() {}
        void add_vertex(double, double, unsigned) {}
        void prepare_src() {}

        void rewind(unsigned) {}
        unsigned vertex(double*, double*) { return path_cmd_stop; }
    };


    //------------------------------------------------------conv_adaptor_vcgen
    template<class VertexSource, 
             class Generator, 
             class Markers=NULL_markers> class conv_adaptor_vcgen
    {
        enum status
        {
            initial,
            accumulate,
            generate
        };

    public:
        explicit conv_adaptor_vcgen(VertexSource& source) :
            m_source(&source), 
            m_status(initial)
        {}
        void attach(VertexSource& source) { m_source = &source; }

        Generator& generator() { return m_generator; }
        const Generator& generator() const { return m_generator; }

        Markers& markers() { return m_markers; }
        const Markers& markers() const { return m_markers; }
        
        void rewind(unsigned path_id) 
        { 
            m_source->rewind(path_id); 
            m_status = initial;
        }

        unsigned vertex(double* x, double* y);

    private:
        // Prohibit copying
        conv_adaptor_vcgen(const conv_adaptor_vcgen<VertexSource, Generator, Markers>&);
        const conv_adaptor_vcgen<VertexSource, Generator, Markers>& 
            operator = (const conv_adaptor_vcgen<VertexSource, Generator, Markers>&);

        VertexSource* m_source;
        Generator     m_generator;
        Markers       m_markers;
        status        m_status;
        unsigned      m_last_cmd;
        double        m_start_x;
        double        m_start_y;
    };





    //------------------------------------------------------------------------
    template<class VertexSource, class Generator, class Markers> 
    unsigned conv_adaptor_vcgen<VertexSource, Generator, Markers>::vertex(double* x, double* y)
    {
        unsigned cmd = path_cmd_stop;
        bool done = false;
        while(!done)
        {
            switch(m_status)
            {
            case initial:
                m_markers.remove_all();
                m_last_cmd = m_source->vertex(&m_start_x, &m_start_y);
                m_status = accumulate;

            case accumulate:
                if(is_stop(m_last_cmd)) return path_cmd_stop;

                m_generator.remove_all();
                m_generator.add_vertex(m_start_x, m_start_y, path_cmd_move_to);
                m_markers.add_vertex(m_start_x, m_start_y, path_cmd_move_to);

                for(;;)
                {
                    cmd = m_source->vertex(x, y);
                    if(is_vertex(cmd))
                    {
                        m_last_cmd = cmd;
                        if(is_move_to(cmd))
                        {
                            m_start_x = *x;
                            m_start_y = *y;
                            break;
                        }
                        m_generator.add_vertex(*x, *y, cmd);
                        m_markers.add_vertex(*x, *y, path_cmd_line_to);
                    }
                    else
                    {
                        if(is_stop(cmd))
                        {
                            m_last_cmd = path_cmd_stop;
                            break;
                        }
                        if(is_end_poly(cmd))
                        {
                            m_generator.add_vertex(*x, *y, cmd);
                            break;
                        }
                    }
                }
                m_generator.rewind(0);
                m_status = generate;

            case generate:
                cmd = m_generator.vertex(x, y);
                if(is_stop(cmd))
                {
                    m_status = accumulate;
                    break;
                }
                done = true;
                break;
            }
        }
        return cmd;
    }

}

#endif
