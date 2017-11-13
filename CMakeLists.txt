cmake_minimum_required(VERSION 3.1)

option(BUILD_SQLITE3 "build sqlite3 shipped with sqlitepp" ON)

if(BUILD_SQLITE3)
    add_subdirectory(sqlite)
else()
    find_path(SQLITE3_INCLUDE_DIR NAMES sqlite3.h)
    find_library(SQLITE3_LIBRARY NAMES sqlite3)
    if(SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
        message(STATUS "Found installed sqlite3: ${SQLITE3_INCLUDE_DIR}, ${SQLITE3_LIBRARY}")
        add_library(sqlitepp::sqlite3 UNKNOWN IMPORTED)
        set_target_properties(sqlitepp::sqlite3 PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${SQLITE3_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "${CMAKE_DL_LIBS}"
                IMPORTED_LOCATION "${SQLITE3_LIBRARY}")
    else()
        message(FATAL_ERROR "Can't found sqlite3")
    endif()
endif()

add_subdirectory(sqlitepp)