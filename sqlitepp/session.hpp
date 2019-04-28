//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_SESSION_HPP_INCLUDED
#define SQLITEPP_SESSION_HPP_INCLUDED

#include "string.hpp"
#include "query.hpp"

struct sqlite3;

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

class transaction;

// Database session. Noncopyable.
class session
{
	friend class transaction; // access to active_txn_
	friend class statement;   // access to last_exec_

public:
    enum : unsigned
    {
        read = 1,
        write = 2,
        create = 4,
    };

	// Create a session.
	session() noexcept;
	
	// Create and open session.
	// Optional parameter flags for file open operations
	// (see SQLite reference at http://sqlite.org/c3ref/c_open_autoproxy.html)
	explicit session(string_t const& file_name, unsigned flags = read | write | create);

    session(session const&) = delete;
    session& operator=(session const&) = delete;

	// Close session on destroy.
	~session();

	// Open database session. Previous one will be closed.
	// Optional parameter flags for file open operations
	// (see SQLite reference at http://sqlite.org/c3ref/c_open_autoproxy.html)
	void open(string_t const& file_name, unsigned flags = read | write | create);
	
	// Close database session.
	void close(bool force = false);

	// Is session opened?
	bool is_open() const noexcept;

	// Is there an active transaction?
	// Currently SQLite 3 doesn't support nested transactions.
	// So we can test, is there any transaction in session.
	// If we have the transaction, we get it or null otherwise.
	transaction* active_txn() const noexcept;

    // Last statement::exec result
    bool last_exec() const noexcept;

	/// SQLite implementation for native sqlite3 functions.
	sqlite3* impl() const noexcept;

    /// Check the most recent sqlite3_* API call's error code. If code is not ok, throws exception.
    void check_error(int code) const;

    /// If last error is not ok, throws exception, equivalent to check_error(last_error).
    void check_last_error() const;

	// Last session error
	int last_error() const noexcept;

	// Last insert row ID
	long long last_insert_rowid() const noexcept;
	
	// The number of rows that were changed (or inserted or deleted)
	// by the most recent SQL statement
	size_t last_changes() const noexcept;

	// The number of rows that were changed (or inserted or deleted)
	// since the database was opened
	size_t total_changes() const noexcept;

	// Execute SQL query immediately.
	// It might be useful for resultless statements like INSERT, UPDATE etc.
	// T is any output-stream-shiftable type.
	template<typename T>
	once_query operator<<(T const& t)
	{
		once_query q(*this);
		q << t;
		return q;
	}

private:
	sqlite3* impl_;
	transaction* active_txn_;
	bool last_exec_;
};

//////////////////////////////////////////////////////////////////////////////

} // end of namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_SESSION_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
