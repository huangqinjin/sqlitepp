//////////////////////////////////////////////////////////////////////////////
// binders.hpp
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_BINDERS_HPP_INCLUDED
#define SQLITEPP_BINDERS_HPP_INCLUDED

#include <memory>

#include "string.hpp"
#include "converters.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

class statement;

/// into binder interface
class into_binder
{
public:
	virtual ~into_binder();
	into_binder* clone() const;

	int bind(statement& st, int pos);
	void update(statement& st);
protected:
	into_binder();
	into_binder(into_binder const&);
	into_binder& operator=(into_binder const&);
private:
	virtual into_binder* do_clone() const = 0;
	virtual void do_bind(statement& st, int pos) = 0;
	virtual void do_update(statement& st) = 0;
};

template<typename T>
class into_pos_binder : public into_binder
{
public:
	into_pos_binder(T& value) 
		: pos_(-1)
		, value_(value)
	{
	}
protected:
	int pos_;
private:
	into_binder* do_clone() const
	{
		return new into_pos_binder<T>(*this);
	}

	void do_bind(statement&, int pos)
	{
		pos_ = pos;
	}

	void do_update(statement& st)
	{
		typename converter<T>::base_type t;
		st.column_value(pos_, t);
		value_ = converter<T>::to(t);
	}

	T& value_;
};

template<typename T>
class into_name_binder : public into_pos_binder<T>
{
public:
	into_name_binder(T& value, string_t const& name) 
		: into_pos_binder<T>(value)
		, name_(name)
	{
	}
private:
	into_binder* do_clone() const
	{
		return new into_name_binder<T>(*this);
	}

	void do_bind(statement& st, int)
	{
		if ( pos_ < 0 )
		{
			pos_ = st.column_index(name_);
		}
	}

	string_t name_;
};

/// use binder interface
class use_binder
{
public:
	virtual ~use_binder();
	use_binder* clone() const;

	int bind(statement& st, int pos);
protected:
	use_binder();
	use_binder(use_binder const&);
	use_binder& operator=(use_binder const&);
private:
	virtual use_binder* do_clone() const = 0;
	virtual void do_bind(statement& st, int pos) = 0;
};

template<typename T>
class use_pos_binder : public use_binder
{
public:
	use_pos_binder(T& value)
		: value_(value)
	{
	}
private:
	use_binder* do_clone() const
	{
		return new use_pos_binder<T>(*this);
	}
	
	void do_bind(statement& st, int pos)
	{
		st.use_value(pos, converter<T>::from(value_));
	}
protected:
	T& value_;
};

template<typename T>
class use_name_binder : public use_pos_binder<T>
{
public:
	use_name_binder(T& value, string_t const& name)
		: use_pos_binder<T>(value)
		, name_(name)
	{
	}
private:
	use_binder* do_clone() const
	{
		return new use_name_binder<T>(*this);
	}
	
	void do_bind(statement& st, int)
	{
		st.use_value(st.use_pos(name_), converter<T>::from(value_));
	}

	string_t name_;
};

typedef std::auto_ptr<into_binder> into_binder_ptr;
typedef std::auto_ptr<use_binder> use_binder_ptr;

// Create position into binding for reference t.
template<typename T>
inline into_binder_ptr into(T& t)
{
	return into_binder_ptr(new into_pos_binder<T>(t));
}
//----------------------------------------------------------------------------

// Create named into binding for reference t.
template<typename T>
inline into_binder_ptr into(T& t, string_t const& name)
{
	return into_binder_ptr(new into_name_binder<T>(t, name));
}
//----------------------------------------------------------------------------

// Create position use binding for reference t.
template<typename T>
inline use_binder_ptr use(T& t)
{
	return use_binder_ptr(new use_pos_binder<T>(t));
}
//----------------------------------------------------------------------------

// Create named use binding for reference t.
template<typename T>
inline use_binder_ptr use(T& t, string_t const& name)
{
	return use_binder_ptr(new use_name_binder<T>(t, name));
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

} //namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_BINDERS_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////