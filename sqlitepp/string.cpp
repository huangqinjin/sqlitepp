//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "string.hpp"
#include <ostream>

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, text const& t)
{
    if (t.size == -1) os << t.data;
    else os.write(t.data, t.size);
    return os;
}

//////////////////////////////////////////////////////////////////////////////

static const text encodings[] = {
        "UNKNOWN",
        "UTF-8",
        "UTF-16LE",
        "UTF-16BE",
        "UTF-16",
};

text to_string(encoding e)
{
    auto i = static_cast<int>(e);
    return encodings[(i < 1 || i > 4) ? 0 : i];
}

encoding parse_encoding(const text& s)
{
    if (s.data)
    {
        for (int i = 1; i < 5; ++i)
        {
            const char* a = encodings[i].data;
            const char* b = s.data;
            while (*a && std::toupper(*b) == *a) ++a, ++b;
            if (*a == *b) return static_cast<encoding>(i);
        }
    }
    return static_cast<encoding>(0);
}

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
