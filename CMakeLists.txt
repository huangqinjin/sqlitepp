cmake_minimum_required(VERSION 3.14)
project(sqlitepp VERSION 1.13.2)
set(PACKAGE_NAME ${PROJECT_NAME})

set(BUILD_SQLITE3 OFF CACHE STRING "build sqlite3 shipped with sqlitepp")
set_property(CACHE BUILD_SQLITE3 PROPERTY STRINGS ON OFF STATIC SHARED OBJECT)

if(BUILD_SQLITE3)
    add_subdirectory(sqlite3)
else()
    find_package(SQLite3 REQUIRED)
    add_library(sqlitepp::sqlite3 ALIAS SQLite::SQLite3)
endif()

add_subdirectory(sqlitepp)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
