//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "exception.hpp"
#include "string.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

exception::exception(int code, text const& msg) noexcept
    : std::runtime_error(msg)
    , code_(code)
{
}

int exception::code() const noexcept
{
    return code_;
}

nested_txn_not_supported::nested_txn_not_supported() noexcept
    : exception(-1, "nested transactions are not supported")
{
}

no_such_column::no_such_column(text const& column) noexcept
    : exception(-2, "no such column '" + column.to_string() + "'")
{
}

session_not_open::session_not_open() noexcept
    : exception(-3, "session not open")
{
}

multi_stmt_not_supported::multi_stmt_not_supported() noexcept
    : exception(-4, "only one statement is supported")
{
}

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
