//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>

#include "transaction.hpp"
#include "exception.hpp"
#include "session.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

transaction::transaction(session& s, type t)
	: s_(&s)
{
	if ( s_->active_txn() )
	{
		throw nested_txn_not_supported();
	}

	char const* begin_cmd;
	switch ( t )
	{
	case deferred:
		begin_cmd = "begin deferred";
		break;
	case immediate:
		begin_cmd = "begin immediate";
		break;
	case exclusive:
		begin_cmd = "begin exclusive";
		break;
	default:
		assert(!"unknown transaction type");
		begin_cmd = "begin";
		break;
	}

	*s_ << utf(begin_cmd);
	s_->active_txn_ = this;
}
//----------------------------------------------------------------------------

transaction::~transaction()
{
	try
	{
		rollback();
	}
	catch (...)
	{
		assert(false);
	}
}
//----------------------------------------------------------------------------

void transaction::rollback()
{
	if ( s_ )
	{
		*s_ << utf("rollback");
        s_->active_txn_ = nullptr;
        s_ = nullptr;
	}
}
//----------------------------------------------------------------------------

void transaction::commit()
{
	if ( s_ )
	{
		*s_ << utf("commit");
        s_->active_txn_ = nullptr;
		s_ = nullptr;
	}
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
