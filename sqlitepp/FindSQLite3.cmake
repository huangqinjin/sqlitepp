find_path(SQLite3_INCLUDE_DIR NAMES sqlite3.h)
find_library(SQLite3_LIBRARY NAMES sqlite3)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3 DEFAULT_MSG SQLite3_INCLUDE_DIR SQLite3_LIBRARY)
if(SQLite3_FOUND)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    find_library(Math_LIBRARY NAMES m)
    if(Math_LIBRARY)
    else()
        unset(Math_LIBRARY CACHE)
    endif()
    add_library(sqlitepp::sqlite3 UNKNOWN IMPORTED)
    set_target_properties(sqlitepp::sqlite3 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SQLite3_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "$<LINK_ONLY:${CMAKE_DL_LIBS};Threads::Threads;${Math_LIBRARY}>"
            IMPORTED_LOCATION "${SQLite3_LIBRARY}")
endif()
