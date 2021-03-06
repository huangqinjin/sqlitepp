//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_BINDERS_HPP_INCLUDED
#define SQLITEPP_BINDERS_HPP_INCLUDED

#include <memory>

#include "fwd.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

/// into binder interface
class SQLITEPP_API into_binder
{
public:
    into_binder() = default;
    virtual ~into_binder() = default;
    into_binder(into_binder const&) = delete;
    into_binder& operator=(into_binder const&) = delete;

    // Bind value to statement st in position pos.
    virtual int bind(statement& st, int pos) = 0;

    // Update bound value.
    virtual void update(statement& st) = 0;
};

typedef std::unique_ptr<into_binder> into_binder_ptr;

/// use binder interface
class SQLITEPP_API use_binder
{
public:
    use_binder() = default;
    virtual ~use_binder() = default;
    use_binder(use_binder const&) = delete;
    use_binder& operator=(use_binder const&) = delete;

    /// Bind value to statement st in position pos
    virtual int bind(statement& st, int pos) = 0;
};

typedef std::unique_ptr<use_binder> use_binder_ptr;

//////////////////////////////////////////////////////////////////////////////

} //namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_BINDERS_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
