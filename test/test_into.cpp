#include <tut.h>

#include <sqlitepp/exception.hpp>
#include <sqlitepp/into.hpp>
#include "statement_data.hpp"

using namespace sqlitepp;

namespace tut {

struct into_data : statement_data {};

typedef test_group<into_data> into_test_group;
typedef into_test_group::object object;

into_test_group into_g("7. into");

// into by pos binding
template<>template<>
void object::test<1>()
{
    record r1(1, utf("Ignat"), 345.2);
    r1.insert(se);

    record r2;
    st << utf("select * from some_table where id = 1"),
        into(r2.id), into(r2.name), into(r2.salary);

    ensure("select completed", st.exec());
    ensure("row", se.last_exec());
    ensure_equals(r1, r2);
    ensure( "single row", !st.exec() );
}

// into by name binding
template<>template<>
void object::test<2>()
{
    record r1(1, utf("Egor"), 345.2);
    r1.insert(se);

    record r2;
    st << utf("select id, name, salary from some_table"),
        into(r2.salary, utf("salary")), into(r2.id, utf("id")), into(r2.name, utf("name"));

    ensure( "row selected", st.exec() );
    ensure("row", se.last_exec());
    ensure_equals(r1, r2);
    ensure("single row", !st.exec() );
}

// into by invalid name binding
template<>template<>
void object::test<3>()
{
    try
    {
        int id;
        st << utf("select id from some_table"), into(id, utf("id_ZZZ"));
        st.exec();
        fail( "exception expected" );
    }
    catch (sqlitepp::exception const&)
    {
        ensure( "statement not prepared", !st.is_prepared() );
    }
}

// into BLOB
template<>template<>
void object::test<4>()
{
    record r1(1, utf("Gosha"), 634.4);
    r1.insert(se);

    record r2;
    st << utf("select * from some_table where id = 1"),
        into(r2.id), into(r2.name), into(r2.salary), into(r2.data);

    ensure("select completed", st.exec());
    ensure("row", se.last_exec());
    ensure_equals(r1, r2);
    ensure("empty blob", r2.data.empty());
    ensure( "single row", !st.exec() );
}

} // namespace tut {
