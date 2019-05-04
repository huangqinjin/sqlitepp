//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_INTO_HPP_INCLUDED
#define SQLITEPP_INTO_HPP_INCLUDED

#include "binders.hpp"
#include "converters.hpp"
#include "string.hpp"
#include "statement.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

/// Positional into binder.
template<typename T>
class into_pos_binder : public into_binder
{
public:
	explicit into_pos_binder(T& value, int pos)
		: pos_(pos)
		, value_(value)
	{
	}

    int bind(statement&, int pos) override
    {
	    if (this->pos_ < 0) this->pos_ = pos;
	    return this->pos_;
    }

    void update(statement& st) override
    {
        typename converter<T>::base_type t;
        st.column_value(this->pos_, t);
        this->value_ = converter<T>::to(t);
    }

protected:
	int pos_;
	T& value_;
};

/// Named into binder.
template<typename T>
class into_name_binder : public into_pos_binder<T>
{
public:
	into_name_binder(T& value, string_t const& name)
		: into_pos_binder<T>(value, -1)
		, name_(name) {}

	int bind(statement& st, int) override
	{
		if ( this->pos_ < 0 )
		{
			this->pos_ = st.column_index(this->name_);
		}
        return this->pos_;
	}

protected:
	string_t const name_;
};

// Create position into binding for reference t.
template<typename T>
inline into_binder_ptr into(T& t, int pos = -1)
{
	return into_binder_ptr(new into_pos_binder<T>(t, pos));
}
//----------------------------------------------------------------------------

// Create named into binding for reference t.
template<typename T>
inline into_binder_ptr into(T& t, string_t const& name)
{
	return into_binder_ptr(new into_name_binder<T>(t, name));
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

} //namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_INTO_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
