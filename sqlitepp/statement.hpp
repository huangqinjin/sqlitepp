//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_STATEMENT_HPP_INCLUDED
#define SQLITEPP_STATEMENT_HPP_INCLUDED

#include <vector>

#include "string.hpp"
#include "query.hpp"
#include "converters.hpp"

struct sqlite3_stmt;

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

class session;

// Database statement, noncopyable
class statement
{
public:
	// Create an empty statement
	explicit statement(session& s) noexcept;
	
	// Create statement with SQL query text.
	statement(session& s, struct text const& sql);

    statement(statement&& src) noexcept;
    statement(statement const&) = delete;
    statement& operator=(statement const&) = delete;

	// Finalize statement on destroy.
	~statement();

	// Execute statement. Return true if result exists.
	bool exec();

	// Prepare statement.
	void prepare();

	// Finalize statement.
	void finalize(bool check_error = true);

	// Is statement prepared.
	bool is_prepared() const noexcept;

	/// SQLite statement implementation for sqlite3 functions
	sqlite3_stmt* impl() const noexcept;

	// Reset statement. Return to prepared state. Optionally rebind values
	void reset(bool rebind = false);

	// Start statement preparing.
	template<typename T>
	prepare_query operator<<(T const& t)
	{
		prepare_query pq(*this);
		pq << t;
		return pq;
	}

	// Statement query.
	query const& q() const noexcept;

	// Statement query.
	query& q() noexcept;

	// Number of columns in result set of prepared statement.
	int column_count() const;
	// Column name in result set.
	struct text column_name(int column) const;
	// Column index in result set.
	int column_index(struct text const& name) const;
	// Column type of result set in prepared statement.
	enum type { integer = 1, real = 2, text = 3, blob = 4, null = 5 };
	// Column type in result set.
	type column_type(int column) const;

	// Column value as int.
	void column_value(int column, int& value) const;
	// Column value as 64-bit int.
	void column_value(int column, long long& value) const;
	// Column value as double.
	void column_value(int column, double& value) const;
    // Column value as BLOB.
    void column_value(int column, struct blob& value) const;
	// Column value as UTF-8 string.
	void column_value(int column, struct text& value) const;
    // Column value as UTF-16 string.
    void column_value(int column, struct text16& value) const;

	// Get column value as type T.
	template<typename T>
	T get(int column) const
	{
		typename converter<T>::base_type t;
		column_value(column, t);
		return converter<T>::to(t);
	}

    // Get use position by name in query.
    int use_pos(struct text const& name) const;
    // Get use parameter count in query.
    int use_count() const;

    // Use null value in query.
    void use_value(int pos, std::nullptr_t);
	// Use int value in query.
	void use_value(int pos, int value);
	// Use 64-bit int value in query.
	void use_value(int pos, long long value);
	// Use double value in query.
	void use_value(int pos, double value);
    // Use BLOB value in query.
    void use_value(int pos, struct blob const& value, bool copy = false);
	// Use UTF-8 string value in query.
	void use_value(int pos, struct text const& value, bool copy = false);
    // Use UTF-16 string value in query.
    void use_value(int pos, struct text16 const& value, bool copy = false);

private:
	session& s_;
	query q_;
	sqlite3_stmt* impl_;
};

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_STATEMENT_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
