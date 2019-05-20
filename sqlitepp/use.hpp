//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_USE_HPP_INCLUDED
#define SQLITEPP_USE_HPP_INCLUDED

#include "binders.hpp"
#include "converters.hpp"
#include "string.hpp"
#include "statement.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

/// Anonymous parameter use binder, for ?
template<typename T>
class use_nameless_binder : public use_binder
{
public:
    explicit use_nameless_binder(T&& value, bool copy)
        : value_(std::forward<T>(value)), copy_(copy) {}

    int bind(statement& st, int pos) override
    {
        st.use_value(pos, converter<typename std::decay<T>::type>::from(this->value_), copy_);
        return pos;
    }

protected:
    T value_;
    bool copy_;
};

/// Positional use binder, for ?NNN
template<typename T>
class use_pos_binder : public use_nameless_binder<T>
{
public:
    use_pos_binder(T&& value, int pos, bool copy)
        : use_nameless_binder<T>(std::forward<T>(value), copy)
        , pos_(pos) {}

    int bind(statement& st, int) override
    {
        return use_nameless_binder<T>::bind(st, pos_);
    }

protected:
    int pos_;
};

/// Named use binder, for :AAAA, @AAAA, $AAAA
template<typename T>
class use_name_binder : public use_pos_binder<T>
{
public:
	use_name_binder(T&& value, text const& name, bool copy)
		: use_pos_binder<T>(std::forward<T>(value), -1, copy)
		, name_(name.to_string()) {}

	int bind(statement& st, int) override
	{
	    if (this->pos_ < 0) this->pos_ = st.use_pos(this->name_);
	    return use_pos_binder<T>::bind(st, this->pos_);
	}

protected:
    u8string const name_;
};

// Create anonymous parameter use binding for reference t.
template<typename T>
inline use_binder_ptr use(T&& t, bool copy = false)
{
	return use_binder_ptr(new use_nameless_binder<T>(std::forward<T>(t), copy));
}
//----------------------------------------------------------------------------

// Create positional use binding for reference t.
template<typename T>
inline use_binder_ptr use(T&& t, int pos, bool copy = false)
{
    return use_binder_ptr(new use_pos_binder<T>(std::forward<T>(t), pos, copy));
}
//----------------------------------------------------------------------------

// Create named use binding for reference t.
template<typename T>
inline use_binder_ptr use(T&& t, text const& name, bool copy = false)
{
	return use_binder_ptr(new use_name_binder<T>(std::forward<T>(t), name, copy));
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

} //namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_USE_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
