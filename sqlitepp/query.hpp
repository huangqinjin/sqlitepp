//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_PROXIES_HPP_INCLUDED
#define SQLITEPP_PROXIES_HPP_INCLUDED

#include <vector>
#include <sstream>

#include "binders.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

// SQL query base class. Noncopyable.
class SQLITEPP_API query
{
    friend class statement; // access to intos_ and uses_

public:
    // Create an empty query.
    query();
    query(query&& src) noexcept;
    query& operator=(query&& src) noexcept;

    query(query const&) = delete;
    query& operator=(query const&) = delete;

    // Clear query on destroy.
    ~query();

    void swap(query& other) noexcept;

    // Current SQL statement.
    std::string sql() const;

    // Clear sql text, into and use bindings.
    void clear() noexcept;

    // Collect SQL text.
    template<typename T>
    query& operator<<(T const& t)
    {
        sql_ << t;
        return *this;
    }

    // Add into binder.
    query& put(into_binder_ptr i);
    query& operator,(into_binder_ptr i);
    
    // Add use binder.
    query& put(use_binder_ptr i);
    query& operator,(use_binder_ptr u);

private:
    std::vector<into_binder_ptr> intos_;
    std::vector<use_binder_ptr> uses_;

    std::ostringstream sql_;
};

// Statement preparing proxy.
class SQLITEPP_API prepare_query : public query
{
    friend class statement; // access to ctor

public:
    // Transfer execution responsibility from src to this object.
    prepare_query(prepare_query&& src) noexcept;

    prepare_query(prepare_query const&) = delete;
    prepare_query& operator=(prepare_query const&) = delete;

    // Move query to statement on destroy.
    ~prepare_query();

private:
    // Create preparing proxy for statement.
    explicit prepare_query(statement& st) noexcept;

    statement* st_;
};

// Immediately executed query proxy, the result can be queried using session.last_exec().
class SQLITEPP_API once_query : public query
{
    friend class session; // access to ctor
public:
    // Transfer execution responsibility from src to this object.
    once_query(once_query&& src) noexcept;

    once_query(once_query const&) = delete;
    once_query& operator=(once_query const&) = delete;

    // Execute statement on destroy.
    ~once_query() noexcept(false);

private:
    // Create proxy for session.
    explicit once_query(session& s) noexcept;

    session* s_;
};

//////////////////////////////////////////////////////////////////////////////

} //namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_PROXIES_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
