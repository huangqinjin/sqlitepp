#ifndef SQLITEPP_TEST_STATEMENT_DATA_HPP_INCLUDED
#define SQLITEPP_TEST_STATEMENT_DATA_HPP_INCLUDED

#include <vector>

#include <sqlitepp/string.hpp>
#include <sqlitepp/statement.hpp>

#include "session_data.hpp"

namespace sqlitepp
{
    template<typename T>
    struct converter<std::vector<T> >
    {
        typedef blob base_type;
        static std::vector<T> to(blob const& b)
        {
            T const* begin = reinterpret_cast<T const*>(b.data);
            T const* end = begin + b.size / sizeof(T);
            return std::vector<T>(begin, end);
        }
        static blob from(std::vector<T> const& t)
        {
            blob b;
            b.data = t.empty()? nullptr : &t[0];
            b.size = t.size() * sizeof(T);
            return b;
        }
    };
}

using sqlitepp::string_t;

struct statement_data : session_data
{
	sqlitepp::statement st;

	statement_data();
	~statement_data();

	struct record
	{
		int id;
		string_t name;
		double salary;
		
		typedef std::vector<char> blob_data;
		blob_data data;

		record() {}

		record(int id, string_t const& name, double salary)
			: id(id), name(name), salary(salary)
		{
		}

		void insert(sqlitepp::session& se);
	};
};

namespace tut {

void ensure_equals(statement_data::record const& r1, statement_data::record const& r2);

} // namespace tut {


template<typename T, size_t N>
inline size_t dimof( T(&)[N] ) { return N; }

#endif // SQLITEPP_TEST_STATEMENT_DATA_HPP_INCLUDED
