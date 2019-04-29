//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "query.hpp"
#include "exception.hpp"
#include "statement.hpp"
#include "session.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////
//
// query
//

query::query() = default;

query::query(query&& src) noexcept
{
	swap(src);
}

query& query::operator=(query&& src) noexcept
{
	if ( this != &src )
	{
		query().swap(*this);
		swap(src);
	}
	return *this;
}

query::~query() = default;

void query::swap(query& other) noexcept
{
    intos_.swap(other.intos_);
    uses_.swap(other.uses_);
    sql_.swap(other.sql_);
}

string_t query::sql() const
{
    return sql_.str();
}
//----------------------------------------------------------------------------

void query::clear() noexcept
{
	intos_.clear();
	uses_.clear();
    sql_.str(string_t());
    sql_.clear();
}
//----------------------------------------------------------------------------

query& query::put(into_binder_ptr i)
{
	if ( !i )
	{
		throw std::invalid_argument("null into binder");
	}
	intos_.emplace_back(std::move(i));
	return *this;
}
//----------------------------------------------------------------------------

query& query::operator,(into_binder_ptr i)
{
    return put(std::move(i));
}
//----------------------------------------------------------------------------

query& query::put(use_binder_ptr u)
{
	if ( !u )
	{
		throw std::invalid_argument("null use binder");
	}
	uses_.emplace_back(std::move(u));
	return *this;
}
//----------------------------------------------------------------------------

query& query::operator,(use_binder_ptr u)
{
    return put(std::move(u));
}
//----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////
//
// prepare_query
//
prepare_query::prepare_query(statement& st) noexcept
	: st_(&st)
{
}

prepare_query::prepare_query(prepare_query&& src) noexcept
	: query(static_cast<query&&>(src))
	, st_(src.st_)
{
	src.st_ = nullptr;
}

prepare_query::~prepare_query()
{
	if ( st_ )
	{
        st_->finalize(false);
        st_->q().swap(*this);
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// once_query
//

once_query::once_query(session& s) noexcept
	: s_(&s)
{
}
//----------------------------------------------------------------------------

once_query::once_query(once_query&& src) noexcept
	: query(static_cast<query&&>(src))
	, s_(src.s_)
{
	src.s_ = nullptr;
}
//----------------------------------------------------------------------------

once_query::~once_query() noexcept(false)
{
	if ( s_ )
	{
		// execute statement in session.
		statement st(*s_);
		st.q().swap(*this);
		st.exec();
	}
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
