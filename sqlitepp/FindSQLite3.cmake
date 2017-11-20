find_path(SQLite3_INCLUDE_DIR NAMES sqlite3.h)
find_library(SQLite3_LIBRARY NAMES sqlite3)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3 DEFAULT_MSG SQLite3_INCLUDE_DIR SQLite3_LIBRARY)
if(SQLite3_FOUND)
    add_library(sqlitepp::sqlite3 UNKNOWN IMPORTED)
    set_target_properties(sqlitepp::sqlite3 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SQLite3_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${CMAKE_DL_LIBS}"
            IMPORTED_LOCATION "${SQLite3_LIBRARY}")
endif()