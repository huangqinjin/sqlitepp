//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_STRING_HPP_INCLUDED
#define SQLITEPP_STRING_HPP_INCLUDED

#include <string>
#include <cassert>
#include <iosfwd>


//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

struct blob
{
	void const* data;
	std::size_t size;
};

template<typename CharT>
struct basic_text
{
    using char_t = CharT;
    using string_t = std::basic_string<char_t>;

    template<typename P, typename T> struct E;
    template<typename T> struct E<T*, T> { using type = std::size_t; };
    template<typename T> struct E<T const*, T> : E<T*, T> {};

    char_t const* data;
    std::size_t size;

    basic_text() noexcept : data(nullptr), size(0) {}
    basic_text(std::nullptr_t, std::size_t = 0) noexcept : data(nullptr), size(0) {}
    basic_text(string_t const& str) noexcept : data(str.data()), size(str.size()) {}
    operator char_t const*() const noexcept { return data; }

    std::size_t length() noexcept
    {
        using traits = typename string_t::traits_type;
        if (size == -1) size = data ? traits::length(data) : 0;
        return size;
    }

    string_t to_string()
    {
        return string_t(data, length());
    }

    string_t to_string() const
    {
        return size != -1 ? string_t(data, size) : data ? string_t(data) : string_t{};
    }

    template<typename T>
    basic_text(T const& str, typename E<T, char_t>::type n = -1) noexcept : data(str), size(n)
    {
        assert((n == -1) || (!str && !n) || (str[n] == char_t{}));
    }

    template<std::size_t N>
    basic_text(char_t const(&str)[N]) noexcept : data(str), size(N - 1)
    {
        assert(str[N - 1] == char_t{});
    }
};

struct text : basic_text<char> { using basic_text::basic_text; };
struct text16 : basic_text<char16_t> { using basic_text::basic_text; };
using u8string = text::string_t;
using u16string = text16::string_t;

std::ostream& operator<<(std::ostream& os, text const& t);

//////////////////////////////////////////////////////////////////////////////

enum class encoding
{
    unknown,
    utf_8,
    utf_16le,
    utf_16be,
    utf_16,
};

text to_string(encoding e);
encoding parse_encoding(const text& s);

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_STRING_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
