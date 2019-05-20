cmake_minimum_required(VERSION 3.1)
project(sqlitepp VERSION 1.0.0)
set(PACKAGE_NAME ${PROJECT_NAME})

option(BUILD_SQLITE3 "build sqlite3 shipped with sqlitepp" OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)
endif()

if(BUILD_SQLITE3)
    add_subdirectory(sqlite3)
else()
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/sqlitepp)
    find_package(SQLite3 MODULE REQUIRED)
endif()

add_subdirectory(sqlitepp)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
