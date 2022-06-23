//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.3
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
// Copyright (c) 2008 Rene Rebe <rene@exactcode.de> [ellipse and circle code]
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
// SVG parser.
//
//----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "agg_svg_parser.h"
#include <expat.h>

namespace agg
{
namespace svg
{
    AGG_API void parse_svg(path_renderer& path, std::string val) {
        path.width = -1;
        path.height = -1;
        agg::svg::parser p(path);
        p.parse(val.c_str());
        path.expand(0);
        if (path.height < 0 || path.width < 0) {
            path.bounding_rect(&path.left, &path.top, &path.width, &path.height);
            path.width -= path.left;
            path.height -= path.top;
        }
    }

    //------------------------------------------------------------------------
    parser::~parser()
    {
        delete [] m_attr_value;
        delete [] m_attr_name;
        delete [] m_buf;
        delete [] m_title;
    }

    //------------------------------------------------------------------------
    parser::parser(path_renderer& path) :
        m_path(path),
        m_tokenizer(),
        m_buf(new char[buf_size]),
        m_title(new char[256]),
        m_title_len(0),
        m_title_flag(false),
        m_path_flag(false),
        m_attr_name(new char[128]),
        m_attr_value(new char[1024]),
        m_attr_name_len(127),
        m_attr_value_len(1023)
    {
        m_title[0] = 0;
    }

    //------------------------------------------------------------------------
    void parser::parse(const char* text)
    {
        char msg[1024];

	    XML_Parser p = XML_ParserCreate(NULL);
	    if(p == 0) 
	    {
		    throw exception("Couldn't allocate memory for parser");
	    }

        XML_SetUserData(p, this);
	    XML_SetElementHandler(p, start_element, end_element);
	    XML_SetCharacterDataHandler(p, content);

        if(!XML_Parse(p, text, (int)strlen(text), true))
        {
            sprintf_s(msg, sizeof(msg),
                "%s at line %lu\n",
                XML_ErrorString(XML_GetErrorCode(p)),
                XML_GetCurrentLineNumber(p));
            throw exception(msg);
        }

        XML_ParserFree(p);

        char* ts = m_title;
        while(*ts)
        {
            if(*ts < ' ') *ts = ' ';
            ++ts;
        }
    }

    //------------------------------------------------------------------------
    void parser::start_element(void* data, const char* el, const char** attr)
    {
        parser& self = *(parser*)data;

        if(strcmp(el, "title") == 0)
        {
            self.m_title_flag = true;
        }
        else 
        if (strcmp(el, "svg") == 0)
        {
            self.m_path.push_attr();
            self.parse_attr(attr);
        }
        else
        if(strcmp(el, "g") == 0)
        {
            self.m_path.push_attr();
            self.parse_attr(attr);
        }
        else
        if(strcmp(el, "path") == 0)
        {
            if(self.m_path_flag)
            {
                throw exception("start_element: Nested path");
            }
            self.m_path.begin_path();
            self.parse_path(attr);
            self.m_path.end_path();
            self.m_path_flag = true;
        }
        else
        if(strcmp(el, "rect") == 0) 
        {
            self.parse_rect(attr);
        }
        else
        if(strcmp(el, "line") == 0) 
        {
            self.parse_line(attr);
        }
        else
        if(strcmp(el, "polyline") == 0) 
        {
            self.parse_poly(attr, false);
        }
        else
        if(strcmp(el, "polygon") == 0) 
        {
            self.parse_poly(attr, true);
        }
        else
        if(strcmp(el, "circle") == 0)
        {
            self.parse_circle(attr);
        }
        else
        if(strcmp(el, "ellipse") == 0)
        {
            self.parse_ellipse(attr);
        }
        //else
        //if(strcmp(el, "<OTHER_ELEMENTS>") == 0) 
        //{
        //}
        // . . .
    } 


    //------------------------------------------------------------------------
    void parser::end_element(void* data, const char* el)
    {
        parser& self = *(parser*)data;

        if(strcmp(el, "title") == 0)
        {
            self.m_title_flag = false;
        }
        else
        if(strcmp(el, "g") == 0)
        {
            self.m_path.pop_attr();
        }
        else
        if(strcmp(el, "path") == 0)
        {
            self.m_path_flag = false;
        }
        //else
        //if(strcmp(el, "<OTHER_ELEMENTS>") == 0) 
        //{
        //}
        // . . .
    }


    //------------------------------------------------------------------------
    void parser::content(void* data, const char* s, int len)
    {
        parser& self = *(parser*)data;

        // m_title_flag signals that the <title> tag is being parsed now.
        // The following code concatenates the pieces of content of the <title> tag.
        if(self.m_title_flag)
        {
            if(len + self.m_title_len > 255) len = 255 - self.m_title_len;
            if(len > 0) 
            {
                memcpy(self.m_title + self.m_title_len, s, len);
                self.m_title_len += len;
                self.m_title[self.m_title_len] = 0;
            }
        }
    }


    //------------------------------------------------------------------------
    void parser::parse_attr(const char** attr)
    {
        int i;
        for(i = 0; attr[i]; i += 2)
        {
            if(strcmp(attr[i], "style") == 0)
            {
                parse_style(attr[i + 1]);
            }
            else
            {
                parse_attr(attr[i], attr[i + 1]);
            }
        }
    }

    //-------------------------------------------------------------
    void parser::parse_path(const char** attr)
    {
        int i;

        for(i = 0; attr[i]; i += 2)
        {
            // The <path> tag can consist of the path itself ("d=") 
            // as well as of other parameters like "style=", "transform=", etc.
            // In the last case we simply rely on the function of parsing 
            // attributes (see 'else' branch).
            if(strcmp(attr[i], "d") == 0)
            {
                m_tokenizer.set_path_str(attr[i + 1]);
                m_path.parse_path(m_tokenizer);
            }
            else
            {
                // Create a temporary single pair "name-value" in order
                // to avoid multiple calls for the same attribute.
                const char* tmp[4];
                tmp[0] = attr[i];
                tmp[1] = attr[i + 1];
                tmp[2] = 0;
                tmp[3] = 0;
                parse_attr(tmp);
            }
        }
    }

    double parse_double(const char* str)
    {
        while(*str == ' ') ++str;
        return atof(str);
    }

    //-------------------------------------------------------------
    bool parser::parse_attr(const char* name, const char* value)
    {
        if(strcmp(name, "style") == 0)
        {
            parse_style(value);
        }
        else
        if(strcmp(name, "fill") == 0)
        {
            if(strcmp(value, "none") == 0)
            {
                m_path.fill_none();
            }
            else
            {
                if (is_valid_color(value)) m_path.fill(Color(value));
                else m_path.fill(value);
            }
        }
        else
        if(strcmp(name, "fill-opacity") == 0)
        {
            m_path.fill_opacity(parse_double(value));
        }
        else
        if(strcmp(name, "stroke") == 0)
        {
            if(strcmp(value, "none") == 0)
            {
                m_path.stroke_none();
            }
            else
            {
                if (is_valid_color(value)) m_path.stroke(Color(value));
                else m_path.stroke(value);
            }
        }
        else
        if(strcmp(name, "stroke-width") == 0)
        {
            m_path.stroke_width(parse_double(value));
        }
        else
        if(strcmp(name, "stroke-linecap") == 0)
        {
            if(strcmp(value, "butt") == 0)        m_path.line_cap(butt_cap);
            else if(strcmp(value, "round") == 0)  m_path.line_cap(round_cap);
            else if(strcmp(value, "square") == 0) m_path.line_cap(square_cap);
        }
        else
        if(strcmp(name, "stroke-linejoin") == 0)
        {
            if(strcmp(value, "miter") == 0)      m_path.line_join(miter_join);
            else if(strcmp(value, "round") == 0) m_path.line_join(round_join);
            else if(strcmp(value, "bevel") == 0) m_path.line_join(bevel_join);
        }
        else
        if(strcmp(name, "stroke-miterlimit") == 0)
        {
            m_path.miter_limit(parse_double(value));
        }
        else
        if(strcmp(name, "stroke-opacity") == 0)
        {
            m_path.stroke_opacity(parse_double(value));
        }
        else
        if(strcmp(name, "transform") == 0)
        {
            parse_transform(value);
        }
		else
        if (strcmp(name, "viewBox") == 0)
        {
            parse_viewbox(value);
        }
        else
        if (strcmp(name, "padding") == 0)
        {
            parse_padding(value);
        }
        else
		if(strcmp(name, "fill-rule") == 0)
		{
		    m_path.even_odd(strcmp(value, "evenodd") == 0);
		}
        //else
        //if(strcmp(el, "<OTHER_ATTRIBUTES>") == 0) 
        //{
        //}
        // . . .
        else
        {
            return false;
        }
        return true;
    }



    //-------------------------------------------------------------
    void parser::copy_name(const char* start, const char* end)
    {
        unsigned len = unsigned(end - start);
        if(m_attr_name_len == 0 || len > m_attr_name_len)
        {
            delete [] m_attr_name;
            m_attr_name = new char[len + 1];
            m_attr_name_len = len;
        }
        if(len) memcpy(m_attr_name, start, len);
        m_attr_name[len] = 0;
    }



    //-------------------------------------------------------------
    void parser::copy_value(const char* start, const char* end)
    {
        unsigned len = unsigned(end - start);
        if(m_attr_value_len == 0 || len > m_attr_value_len)
        {
            delete [] m_attr_value;
            m_attr_value = new char[len + 1];
            m_attr_value_len = len;
        }
        if(len) memcpy(m_attr_value, start, len);
        m_attr_value[len] = 0;
    }


    //-------------------------------------------------------------
    bool parser::parse_name_value(const char* nv_start, const char* nv_end)
    {
        const char* str = nv_start;
        while(str < nv_end && *str != ':') ++str;

        const char* val = str;

        // Right Trim
        while(str > nv_start && 
            (*str == ':' || isspace(*str))) --str;
        ++str;

        copy_name(nv_start, str);

        while(val < nv_end && (*val == ':' || isspace(*val))) ++val;
        
        copy_value(val, nv_end);
        return parse_attr(m_attr_name, m_attr_value);
    }



    //-------------------------------------------------------------
    void parser::parse_style(const char* str)
    {
        while(*str)
        {
            // Left Trim
            while(*str && isspace(*str)) ++str;
            const char* nv_start = str;
            while(*str && *str != ';') ++str;
            const char* nv_end = str;

            // Right Trim
            while(nv_end > nv_start && 
                (*nv_end == ';' || isspace(*nv_end))) --nv_end;
            ++nv_end;

            parse_name_value(nv_start, nv_end);
            if(*str) ++str;
        }

    }


    //-------------------------------------------------------------
    void parser::parse_rect(const char** attr)
    {
        int i;
        double x = 0.0;
        double y = 0.0;
        double w = 0.0;
        double h = 0.0;

        m_path.begin_path();
        for(i = 0; attr[i]; i += 2)
        {
            if(!parse_attr(attr[i], attr[i + 1]))
            {
                if(strcmp(attr[i], "x") == 0)      x = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "y") == 0)      y = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "width") == 0)  w = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "height") == 0) h = parse_double(attr[i + 1]);
                // rx - to be implemented 
                // ry - to be implemented
            }
        }

        m_path.left   = x;
        m_path.top    = y;
        m_path.width  = w;
        m_path.height = h;

        if(w != 0.0 && h != 0.0)
        {
            if(w < 0.0) throw exception("parse_rect: Invalid width: %f", w);
            if(h < 0.0) throw exception("parse_rect: Invalid height: %f", h);

            m_path.move_to(x,     y);
            m_path.line_to(x + w, y);
            m_path.line_to(x + w, y + h);
            m_path.line_to(x,     y + h);
            m_path.close_subpath();
        }
        m_path.end_path();
    }


    //-------------------------------------------------------------
    void parser::parse_line(const char** attr)
    {
        int i;
        double x1 = 0.0;
        double y1 = 0.0;
        double x2 = 0.0;
        double y2 = 0.0;

        m_path.begin_path();
        for(i = 0; attr[i]; i += 2)
        {
            if(!parse_attr(attr[i], attr[i + 1]))
            {
                if(strcmp(attr[i], "x1") == 0) x1 = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "y1") == 0) y1 = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "x2") == 0) x2 = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "y2") == 0) y2 = parse_double(attr[i + 1]);
            }
        }

        m_path.move_to(x1, y1);
        m_path.line_to(x2, y2);
        m_path.end_path();
    }


    //-------------------------------------------------------------
    void parser::parse_poly(const char** attr, bool close_flag)
    {
        int i;
        double x = 0.0;
        double y = 0.0;

        m_path.begin_path();
        for(i = 0; attr[i]; i += 2)
        {
            if(!parse_attr(attr[i], attr[i + 1]))
            {
                if(strcmp(attr[i], "points") == 0) 
                {
                    m_tokenizer.set_path_str(attr[i + 1]);
                    if(!m_tokenizer.next())
                    {
                        throw exception("parse_poly: Too few coordinates");
                    }
                    x = m_tokenizer.last_number();
                    if(!m_tokenizer.next())
                    {
                        throw exception("parse_poly: Too few coordinates");
                    }
                    y = m_tokenizer.last_number();
                    m_path.move_to(x, y);
                    while(m_tokenizer.next())
                    {
                        x = m_tokenizer.last_number();
                        if(!m_tokenizer.next())
                        {
                            throw exception("parse_poly: Odd number of coordinates");
                        }
                        y = m_tokenizer.last_number();
                        m_path.line_to(x, y);
                    }
                }
            }
        }
        if(close_flag) 
        {
            m_path.close_subpath();
        }
        m_path.end_path();
    }

    //-------------------------------------------------------------
    void parser::parse_circle(const char** attr)
    {
        int i;
        double cx = 0.0;
        double cy = 0.0;
        double r = 0.0;

        m_path.begin_path();
        for(i = 0; attr[i]; i += 2)
        {
            if(!parse_attr(attr[i], attr[i + 1]))
            {
                if(strcmp(attr[i], "cx") == 0) cx = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "cy") == 0) cy = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "r") == 0) r = parse_double(attr[i + 1]);
            }
        }

        m_path.move_to(cx-r, cy);
        m_path.arc(r, r, 360, true, true, 0, .0001, true);
        m_path.end_path();
    }


    void parser::parse_ellipse(const char** attr)
    {
        int i;
        double cx = 0.0;
        double cy = 0.0;
        double rx = 0.0;
        double ry = 0.0;

        m_path.begin_path();
        for(i = 0; attr[i]; i += 2)
        {
            if(!parse_attr(attr[i], attr[i + 1]))
            {
                if(strcmp(attr[i], "cx") == 0) cx = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "cy") == 0) cy = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "rx") == 0) rx = parse_double(attr[i + 1]);
                if(strcmp(attr[i], "ry") == 0) ry = parse_double(attr[i + 1]);
            }
        }

        m_path.move_to(cx-rx, cy);
        m_path.arc(rx, ry, 360, true, true, 0, .0001, true);
        m_path.end_path();
    }
 
    inline void trim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));

        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    bool isNumber(const char* c) {
        if (c[0] >= '0' && c[0] <= '9')
            return true;
        if (c[0] == '.' || c[0] == '-')
            return isNumber(c + 1);
        return false;
    }

    //-------------------------------------------------------------
    void parser::parse_padding(const char* str)
    {
        std::string s = str, a;
        int i, j, k, l, sz = int(s.length());
        bool b;
        char* c = (char*)s.c_str();

        i = j = k = l = -1;
        b = true;

        for (int n = 0; n < sz;) {
            if (isNumber(c + n) && b) {
                if (i < 0) i = n;
                else if (j < 0) j = n;
                else if (k < 0)
                    k = n;
                else if (l < 0) {
                    l = n;
                    break;
                }
                b = false;
            }
            else if (c[n] == ' ')
                b = true;
            else if (c[n] == '-' ||
                (c[n] == '.' && !n) ||
                (c[n] == '.' && n && c[n - 1] == ' '))
            {
                b = true;
                continue;
            }
            ++n;
        }

        if (i > -1 && j > -1 && k > -1 && l > -1) {
            a = s.substr(i, j - i);
            trim(a);
            m_path.pl = int(atof(a.c_str()));

            a = s.substr(j, k - j);
            trim(a);
            m_path.pt = int(atof(a.c_str()));

            a = s.substr(k, l - k);
            trim(a);
            m_path.pr = int(atof(a.c_str()));

            a = s.substr(l);
            trim(a);
            m_path.pb = int(atof(a.c_str()));
        }
    }

    //-------------------------------------------------------------
    void parser::parse_viewbox(const char* str)
    {
        std::string s = str, a;
        int i, j, k, l, sz = int(s.length());
        bool b;
        char* c = (char*)s.c_str();

        i = j = k = l = -1;
        b = true;

        for (int n = 0; n < sz;) {
            if (isNumber(c + n) && b) {
                if (i < 0) i = n;
                else if (j < 0) j = n;
                else if (k < 0)
                    k = n;
                else if (l < 0) {
                    l = n;
                    break;
                }
                b = false;
            }
            else if (c[n] == ' ')
                b = true;
            else if (c[n] == '-' ||
                (c[n] == '.' && !n) ||
                (c[n] == '.' &&  n && c[n-1] == ' '))
            {
                b = true;
                continue;
            }
            ++n;
        }

        if (i > -1 && j > -1 && k > -1 && l > -1) {
            a = s.substr(i, j - i);
            trim(a);
            m_path.left = int(atof(a.c_str()));

            a = s.substr(j, k - j);
            trim(a);
            m_path.top = int(atof(a.c_str()));
            
            a = s.substr(k, l - k);
            trim(a);
            m_path.width = int(atof(a.c_str()));

            a = s.substr(l);
            trim(a);
            m_path.height = int(atof(a.c_str()));
        }
    }

    //-------------------------------------------------------------
    void parser::parse_transform(const char* str)
    {
        while(*str)
        {
            if(islower(*str))
            {
                if(strncmp(str, "matrix", 6) == 0)    str += parse_matrix(str);    else 
                if(strncmp(str, "translate", 9) == 0) str += parse_translate(str); else 
                if(strncmp(str, "rotate", 6) == 0)    str += parse_rotate(str);    else 
                if(strncmp(str, "scale", 5) == 0)     str += parse_scale(str);     else 
                if(strncmp(str, "skewX", 5) == 0)     str += parse_skew_x(str);    else 
                if(strncmp(str, "skewY", 5) == 0)     str += parse_skew_y(str);    else
                {
                    ++str;
                }
            }
            else
            {
                ++str;
            }
        }
    }


    //-------------------------------------------------------------
    static bool is_numeric(char c)
    {
        return strchr("0123456789+-.eE", c) != 0;
    }

    //-------------------------------------------------------------
    static unsigned parse_transform_args(const char* str, 
                                         double* args, 
                                         unsigned max_na, 
                                         unsigned* na)
    {
        *na = 0;
        const char* ptr = str;
        while(*ptr && *ptr != '(') ++ptr;
        if(*ptr == 0)
        {
            throw exception("parse_transform_args: Invalid syntax");
        }
        const char* end = ptr;
        while(*end && *end != ')') ++end;
        if(*end == 0)
        {
            throw exception("parse_transform_args: Invalid syntax");
        }

        while(ptr < end)
        {
            if(is_numeric(*ptr))
            {
                if(*na >= max_na)
                {
                    throw exception("parse_transform_args: Too many arguments");
                }
                args[(*na)++] = atof(ptr);
                while(ptr < end && is_numeric(*ptr)) ++ptr;
            }
            else
            {
                ++ptr;
            }
        }
        return unsigned(end - str);
    }

    //-------------------------------------------------------------
    unsigned parser::parse_matrix(const char* str)
    {
        double args[6];
        unsigned na = 0;
        unsigned len = parse_transform_args(str, args, 6, &na);
        if(na != 6)
        {
            throw exception("parse_matrix: Invalid number of arguments");
        }
        m_path.transform().premultiply(trans_affine(args[0], args[1], args[2], args[3], args[4], args[5]));
        return len;
    }

    //-------------------------------------------------------------
    unsigned parser::parse_translate(const char* str)
    {
        double args[2];
        unsigned na = 0;
        unsigned len = parse_transform_args(str, args, 2, &na);
        if(na == 1) args[1] = 0.0;
        m_path.transform().premultiply(trans_affine_translation(args[0], args[1]));
        return len;
    }

    //-------------------------------------------------------------
    unsigned parser::parse_rotate(const char* str)
    {
        double args[3];
        unsigned na = 0;
        unsigned len = parse_transform_args(str, args, 3, &na);
        if(na == 1) 
        {
            m_path.transform().premultiply(trans_affine_rotation(deg2rad(args[0])));
        }
        else if(na == 3)
        {
            trans_affine t = trans_affine_translation(-args[1], -args[2]);
            t *= trans_affine_rotation(deg2rad(args[0]));
            t *= trans_affine_translation(args[1], args[2]);
            m_path.transform().premultiply(t);
        }
        else
        {
            throw exception("parse_rotate: Invalid number of arguments");
        }
        return len;
    }

    //-------------------------------------------------------------
    unsigned parser::parse_scale(const char* str)
    {
        double args[2];
        unsigned na = 0;
        unsigned len = parse_transform_args(str, args, 2, &na);
        if(na == 1) args[1] = args[0];
        m_path.transform().premultiply(trans_affine_scaling(args[0], args[1]));
        return len;
    }

    //-------------------------------------------------------------
    unsigned parser::parse_skew_x(const char* str)
    {
        double arg;
        unsigned na = 0;
        unsigned len = parse_transform_args(str, &arg, 1, &na);
        m_path.transform().premultiply(trans_affine_skewing(deg2rad(arg), 0.0));
        return len;
    }

    //-------------------------------------------------------------
    unsigned parser::parse_skew_y(const char* str)
    {
        double arg;
        unsigned na = 0;
        unsigned len = parse_transform_args(str, &arg, 1, &na);
        m_path.transform().premultiply(trans_affine_skewing(0.0, deg2rad(arg)));
        return len;
    }

}
}


