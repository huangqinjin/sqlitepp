//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <numeric>
#include <type_traits>

#include <sqlite3.h>

#include "statement.hpp"
#include "exception.hpp"
#include "binders.hpp"
#include "session.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

statement::statement(session& s) noexcept
	: s_(s)
	, impl_(nullptr)
{
}
//----------------------------------------------------------------------------

statement::statement(session& s, string_t const& sql)
	: statement(s)
{
    q_ << sql;
}
//----------------------------------------------------------------------------

statement::statement(statement&& src) noexcept
	: s_(src.s_)
	, q_(std::move(src.q_))
	, impl_(src.impl_)
{
	src.impl_ = nullptr;
}
//----------------------------------------------------------------------------

statement::~statement()
{
	finalize(false);
}
//----------------------------------------------------------------------------

void statement::prepare()
{
	try
	{
		typedef std::conditional<sizeof(char_t) == sizeof(utf8_char), char const*, void const*>::type tail_type;
		char_t const* tail = nullptr;
		string_t const sql = q_.sql();
		s_.check_error(
			aux::select(sqlite3_prepare_v2, sqlite3_prepare16_v2)(s_.impl(), sql.c_str(),
                static_cast<int>((sql.size() + 1) * sizeof(char_t)), // nByte is the number of bytes in the input string including the nul-terminator.
				&impl_, reinterpret_cast<tail_type*>(&tail)) );
		if ( tail && *tail )
		{
			throw multi_stmt_not_supported();
		}

        int index = 0;
		// bind into binders
        for (auto& i : q_.intos_) index = 1 + i->bind(*this, index);

		// bind use binders
        index = 1;
		for (auto& u : q_.uses_) index = 1 + u->bind(*this, index);
	}
	catch (...)
	{
		// statement stays not prepared
		finalize(false);
		throw;
	}
}
//----------------------------------------------------------------------------

bool statement::exec()
{
	if ( !is_prepared() )
	{
		prepare();
	}
	try
	{
		int const r = sqlite3_step(impl_);
		switch ( r )
		{
		case SQLITE_ROW:
            s_.last_exec_ = true;
            // statement has result (select for ex.) - update into holders
            for (auto& i : q_.intos_) i->update(*this);
            break;
		case SQLITE_DONE:
            s_.last_exec_ = false;
            break;
		default:
            s_.last_exec_ = false;
			s_.check_error(r);
			break;
		}
		return s_.last_exec_;
	}
	catch (...)
	{
		finalize(false);
		throw;
	}
}
//----------------------------------------------------------------------------

void statement::reset(bool rebind)
{
	if ( is_prepared() )
	{
		s_.check_error( sqlite3_reset(impl_) );
		if ( rebind )
		{
            int index = 1;
            for (auto& u : q_.uses_) index = 1 + u->bind(*this, index);
		}
	}
}
//----------------------------------------------------------------------------

query const& statement::q() const noexcept
{
    return q_;
}
//----------------------------------------------------------------------------

query& statement::q() noexcept
{
    return q_;
}
//----------------------------------------------------------------------------

void statement::finalize(bool check_error) // throw
{
	if ( is_prepared() )
	{
		int const r = sqlite3_finalize(impl_);
		impl_ = nullptr;
		if ( check_error )
		{
			s_.check_error(r);
		}
	}
}
//----------------------------------------------------------------------------

bool statement::is_prepared() const noexcept
{
    return impl_ != nullptr;
}
//----------------------------------------------------------------------------

sqlite3_stmt* statement::impl() const noexcept
{
    return impl_;
}
//----------------------------------------------------------------------------

int statement::column_count() const
{
	int count = sqlite3_column_count(impl_);
    s_.check_last_error();
	return count;
}
//----------------------------------------------------------------------------

string_t statement::column_name(int column) const
{
	char_t const* name = reinterpret_cast<char_t const*>
		(aux::select(sqlite3_column_name, sqlite3_column_name16)(impl_, column));
    s_.check_last_error();
	return name;
}
//----------------------------------------------------------------------------

int statement::column_index(string_t const& name) const
{
	for (int c = 0, cc = column_count(); c < cc; ++c)
	{
		if ( name == column_name(c) )
		{
			return c;
		}
	}
	throw no_such_column(name);
}

statement::type statement::column_type(int column) const
{
	int const t = sqlite3_column_type(impl_, column);
    s_.check_last_error();
	return static_cast<type>(t);
}
//----------------------------------------------------------------------------

void statement::column_value(int column, int& value) const
{
	auto v = sqlite3_column_int(impl_, column);
    s_.check_last_error();
    value = v;
}
//----------------------------------------------------------------------------

void statement::column_value(int column, long long& value) const
{
	auto v = sqlite3_column_int64(impl_, column);
    s_.check_last_error();
    value = v;
}
//----------------------------------------------------------------------------

void statement::column_value(int column, double& value) const
{
	auto v = sqlite3_column_double(impl_, column);
    s_.check_last_error();
    value = v;
}
//----------------------------------------------------------------------------

void statement::column_value(int column, string_t& value) const
{
	char_t const* str = reinterpret_cast<char_t const*>(
		aux::select(sqlite3_column_text, sqlite3_column_text16)(impl_, column));
    s_.check_last_error();
	if ( str )
	{
		value = str;
	}
	else
	{
		value.clear();
	}
}
//----------------------------------------------------------------------------

void statement::column_value(int column, struct blob& value) const
{
    // Call sqlite3_column_blob() first to force the result into the desired format,
    // then invoke sqlite3_column_bytes() to find the size of the result.
    // https://sqlite.org/c3ref/column_blob.html
    struct blob b;

    b.data = sqlite3_column_blob(impl_, column);
	s_.check_last_error();

	b.size = sqlite3_column_bytes(impl_, column);
    s_.check_last_error();
}
//----------------------------------------------------------------------------

int statement::use_pos(string_t const& name) const
{
	int pos = sqlite3_bind_parameter_index(impl_, utf8(name).c_str());
	if ( pos <= 0 )
	{
		throw no_such_column(name);
	}
	return pos;
}
//----------------------------------------------------------------------------

int statement::use_count() const
{
    return sqlite3_bind_parameter_count(impl_);
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, std::nullptr_t)
{
    s_.check_error( sqlite3_bind_null(impl_, pos) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, int value)
{
	s_.check_error( sqlite3_bind_int(impl_, pos, value) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, double value)
{
	s_.check_error( sqlite3_bind_double(impl_, pos, value) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, long long value)
{
	s_.check_error( sqlite3_bind_int64(impl_, pos, value) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, utf8_char const* value, bool copy)
{
	s_.check_error( sqlite3_bind_text(impl_, pos, value, -1, copy ? SQLITE_TRANSIENT : SQLITE_STATIC) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, utf16_char const* value, bool copy)
{
	s_.check_error( sqlite3_bind_text16(impl_, pos, value, -1, copy ? SQLITE_TRANSIENT : SQLITE_STATIC) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, string_t const& value, bool copy)
{
	s_.check_error( aux::select(sqlite3_bind_text, sqlite3_bind_text16)
		(impl_, pos, value.empty()? nullptr : value.c_str(),
		static_cast<int>(value.size() * sizeof(char_t)), copy ? SQLITE_TRANSIENT : SQLITE_STATIC) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, struct blob const& value, bool copy)
{
    if (value.data == nullptr && value.size > 0)
    {
        s_.check_error( sqlite3_bind_zeroblob64(impl_, pos, value.size) );
    }
    else
    {
        s_.check_error( sqlite3_bind_blob(impl_, pos, value.data,
            static_cast<int>(value.size), copy ? SQLITE_TRANSIENT : SQLITE_STATIC) );
    }
}

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
