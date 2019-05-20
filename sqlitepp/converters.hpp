//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_CONVERTERS_HPP_INCLUDED
#define SQLITEPP_CONVERTERS_HPP_INCLUDED

#include "string.hpp"
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

template<typename T, typename Enable = void>
struct converter;

template<typename T, typename U>
struct converter_base
{
	typedef U base_type;
	static T to(U u) { return static_cast<T>(u); }
	static U from(T t) { return static_cast<U>(t); }
};

template<> struct converter<bool> : converter_base<bool, int>
{
	// resolve Visual C++ warning C4800
	static bool to(int u) { return u != 0; }
	static int from(bool t) { return t; }
};

template<> struct converter<char> : converter_base<char, int> {};
template<> struct converter<signed char> : converter_base<signed char, int> {};
template<> struct converter<unsigned char> : converter_base<unsigned char, int> {};
template<> struct converter<wchar_t> : converter_base<wchar_t, int> {};
template<> struct converter<short> : converter_base<short, int> {};
template<> struct converter<unsigned short> : converter_base<unsigned short, int> {};
template<> struct converter<int> : converter_base<int, int> {};
template<> struct converter<unsigned int> : converter_base<unsigned int, int> {};
template<> struct converter<long> : converter_base<long, int> {};
template<> struct converter<unsigned long> : converter_base<unsigned long, int> {};
template<> struct converter<long long> : converter_base<long long, long long> {};
template<> struct converter<unsigned long long> : converter_base<unsigned long long, long long> {};
template<> struct converter<float> : converter_base<float, double> {};
template<> struct converter<double> : converter_base<double, double> {};
template<> struct converter<blob> : converter_base<blob, blob> {};
template<> struct converter<text> : converter_base<text, text> {};
template<> struct converter<text16> : converter_base<text16, text16> {};

template<typename T>
struct converter<T, typename std::enable_if<std::is_enum<T>::value>::type> : converter_base<T, int> {};

template<>
struct converter<char const*>
{
    typedef text base_type;
    static char const* to(text const& b)
    {
        return b;
    }
    static text from(char const* t)
    {
        return t;
    }
};

template<>
struct converter<u8string>
{
	typedef text base_type;
	static u8string to(text const& b)
	{
		return b.to_string();
	}
	static text from(u8string const& t)
	{
		return t;
	}
};

template<>
struct converter<char16_t const*>
{
    typedef text16 base_type;
    static char16_t const* to(text16 const& b)
    {
        return b;
    }
    static text16 from(char16_t const* t)
    {
        return t;
    }
};

template<>
struct converter<u16string>
{
    typedef text16 base_type;
    static u16string to(text16 const& b)
    {
        return b.to_string();
    }
    static text16 from(u16string const& t)
    {
        return t;
    }
};

//////////////////////////////////////////////////////////////////////////////

} //namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_CONVERTERS_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
