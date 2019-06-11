//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_EXCEPTION_HPP_INCLUDED
#define SQLITEPP_EXCEPTION_HPP_INCLUDED

#include <stdexcept>

#include "fwd.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

// SQLite exception
class SQLITEPP_API exception : public std::runtime_error
{
public:
    // create exception with UTF-8 encoded message
    exception(int code, text const& msg) noexcept;

    // SQLite library error code.
    int code() const noexcept;

private:
    int code_;
};

struct SQLITEPP_API nested_txn_not_supported : exception
{
    nested_txn_not_supported() noexcept;
};

struct SQLITEPP_API no_such_column : exception
{
    explicit no_such_column(text const& column) noexcept;
};

struct SQLITEPP_API session_not_open : exception
{
    session_not_open() noexcept;
};

struct SQLITEPP_API multi_stmt_not_supported : exception
{
    multi_stmt_not_supported() noexcept;
};

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_EXCEPTION_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
