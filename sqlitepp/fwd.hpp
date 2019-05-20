//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2019 huangqinjin
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_FWD_HPP_INCLUDED
#define SQLITEPP_FWD_HPP_INCLUDED

#if defined(_WIN32)
#  define SQLITEPP_EXPORT __declspec(dllexport)
#  define SQLITEPP_IMPORT __declspec(dllimport)
#else
#  define SQLITEPP_EXPORT __attribute__((visibility("default")))
#  define SQLITEPP_IMPORT __attribute__((visibility("default")))
#endif

#if defined(sqlitepp_EXPORTS) // defined by cmake DEFINE_SYMBOL property
#  define SQLITEPP_API SQLITEPP_EXPORT
#elif SQLITEPP_SHARED_LIBRARY
#  define SQLITEPP_API SQLITEPP_IMPORT
#else
#  define SQLITEPP_API
#endif

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

class session;
class query;
class prepare_query;
class once_query;
class statement;
class transaction;
class exception;
struct blob;
struct text;
struct text16;

//////////////////////////////////////////////////////////////////////////////

} // end of namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_FWD_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
