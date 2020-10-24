//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <sqlite3.h>

#include <cassert>

#include "session.hpp"
#include "exception.hpp"
#include "transaction.hpp"

#define SQLITEPP_SQL(s) #s, sizeof(#s)

#define SQLITEPP_EXEC(sql, action) { \
sqlite3_stmt* st = nullptr; \
if (sqlite3_prepare(impl_, SQLITEPP_SQL(sql), &st, nullptr) == 0 && \
    sqlite3_step(st) == SQLITE_ROW) action; sqlite3_finalize(st);  }

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

// Create an empty session.
session::session() noexcept
    : impl_(nullptr)
    , active_txn_(nullptr)
    , last_exec_(false)
{
}
//----------------------------------------------------------------------------

session::session(text const& filename, unsigned flags)
    : session()
{
    open(filename, flags);
}
//----------------------------------------------------------------------------

session::session(text const& filename, enum encoding encoding, unsigned flags)
      : session()
{
    open(filename, encoding, flags);
}
//----------------------------------------------------------------------------

session::~session()
{
    close(true);

}
//----------------------------------------------------------------------------

void session::open(text const& filename, unsigned flags)
{
    // close previous session
    close(true);

    if ( flags & write )
    {
        flags &= ~read;
    }

    flags |= SQLITE_OPEN_URI;

    // The new database connection will use the "multi-thread" threading mode.
    // This means that separate threads are allowed to use SQLite at the same time,
    // as long as each thread is using a different database connection.
    flags |= SQLITE_OPEN_NOMUTEX;

    int const r = sqlite3_open_v2(filename, &impl_, flags, nullptr);
    if ( r != SQLITE_OK )
    {
        struct closer
        {
            session* const self;
            ~closer() { self->close(true); }
        } _ {this}; // session should be closed anyway
        // If NULL is passed, we assume a malloc() failed during sqlite3_open().
        // @see sqlite3_errcode()
        throw exception(sqlite3_extended_errcode(impl_), sqlite3_errmsg(impl_));
    }
}
//----------------------------------------------------------------------------

void session::open(text const& filename, enum encoding encoding, unsigned flags)
{
    open(filename, flags);
    switch (encoding)
    {
    case encoding::utf_8: SQLITEPP_EXEC(pragma encoding = "UTF-8",); break;
    case encoding::utf_16le: SQLITEPP_EXEC(pragma encoding = "UTF-16LE",); break;
    case encoding::utf_16be: SQLITEPP_EXEC(pragma encoding = "UTF-16BE",); break;
    case encoding::utf_16: SQLITEPP_EXEC(pragma encoding = "UTF-16",); break;
    default: break;
    }
}
//----------------------------------------------------------------------------

enum encoding session::encoding() const noexcept
{
    enum encoding e = encoding::unknown;
    SQLITEPP_EXEC(pragma encoding, e = parse_encoding((const char*)sqlite3_column_text(st, 0)));
    return e;
}
//----------------------------------------------------------------------------

void session::close(bool force)
{
    if (is_open())
    {
        // http://sqlite.org/c3ref/close.html
        // Call with a NULL pointer argument is a harmless no-op.
        int const r = (force ? sqlite3_close_v2 : sqlite3_close)(impl_);
        check_error(r);
        impl_ = nullptr;
        active_txn_ = nullptr;
        last_exec_ = false;
    }
}
//----------------------------------------------------------------------------

bool session::is_open() const noexcept
{
    return impl_ != nullptr;
}
//----------------------------------------------------------------------------

transaction* session::active_txn() const noexcept
{
    return active_txn_;
}
//----------------------------------------------------------------------------

bool session::last_exec() const noexcept
{
    return last_exec_;
}
//----------------------------------------------------------------------------

sqlite3* session::impl() const noexcept
{
    return impl_;
}
//----------------------------------------------------------------------------

int session::last_error() const noexcept
{
    return impl_ ? sqlite3_extended_errcode(impl_) : SQLITE_OK;
}
//----------------------------------------------------------------------------

long long session::last_insert_rowid() const noexcept
{
    return impl_ ? sqlite3_last_insert_rowid(impl_) : 0;
}
//----------------------------------------------------------------------------

std::size_t session::last_changes() const noexcept
{
    return impl_ ? sqlite3_changes(impl_) : 0;
}
//----------------------------------------------------------------------------

std::size_t session::total_changes() const noexcept
{
    return impl_ ? sqlite3_total_changes(impl_) : 0;
}
//----------------------------------------------------------------------------

void session::check_error(int code) const
{
    if (!is_open())
    {
        throw session_not_open();
    }

    // http://sqlite.org/c3ref/errcode.html
    switch (code & 0xff)
    {
    case SQLITE_OK:
    case SQLITE_ROW:
    case SQLITE_DONE:
        break;

    // If an interface fails with SQLITE_MISUSE, that means the interface was invoked incorrectly by the application.
    // In that case, the error code and message may or may not be set.
    // e.g.: API called with finalized prepared statement. @see sqlite3_finalize()
    case SQLITE_MISUSE:
        throw exception(code, sqlite3_errstr(code));

    default:
        throw exception(code, sqlite3_errmsg(impl_));
    }
}
//----------------------------------------------------------------------------

void session::check_last_error() const
{
    return check_error(last_error());
}
//----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
