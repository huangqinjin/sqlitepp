cmake_minimum_required(VERSION 3.3)
project(sqlite3 VERSION 3.50.2)

option(BUILD_SHELL "build sqlite3 shell" OFF)

# https://www.sqlite.org/compile.html
set(SQLITE_THREADSAFE "1" CACHE STRING     # https://www.sqlite.org/threadsafe.html
    "SQLite supports three different threading modes:
     0 - Single-thread. All mutexes are disabled and SQLite is unsafe to use in more than a single thread at once.
     1 - Serialized. SQLite can be safely used by multiple threads with no restriction.
     2 - Multi-thread. SQLite can be safely used by multiple threads provided that no single database connection is used simultaneously in two or more threads."
)
set_property(CACHE SQLITE_THREADSAFE PROPERTY STRINGS 0 1 2)

set(SQLITE_DEFAULT_WAL_SYNCHRONOUS "1" CACHE STRING
    "Determines the default value of the `PRAGMA synchronous` setting for database files that open in WAL mode:
     0 - OFF. SQLite continues without syncing as soon as it has handed data off to the operating system. If the application running SQLite crashes, the data will be safe, but the database might become corrupted if the operating system crashes or the computer loses power before that data has been written to the disk surface.
     1 - NORMAL. SQLite database engine will still sync at the most critical moments, but less often than in FULL mode. There is a very small (though non-zero) chance that a power failure at just the wrong time could corrupt the database in NORMAL mode. But in practice, you are more likely to suffer a catastrophic disk failure or some other unrecoverable hardware fault.
     2 - FULL. SQLite database engine will use the xSync method of the VFS to ensure that all content is safely written to the disk surface prior to continuing. This ensures that an operating system crash or power failure will not corrupt the database.
     3 - EXTRA. EXTRA synchronous is like FULL with the addition that the directory containing a rollback journal is synced after that journal is unlinked to commit a transaction in DELETE mode. EXTRA provides additional durability if the commit is followed closely by a power loss."
)
set_property(CACHE SQLITE_DEFAULT_WAL_SYNCHRONOUS PROPERTY STRINGS 0 1 2 3)

set(SQLITE_DQS "0" CACHE STRING "Determines whether or not Double-Quoted Strings are allowed by default in DDL and DML.")
set_property(CACHE SQLITE_DQS PROPERTY STRINGS 0 1 2 3)

set(SQLITE_DEFAULT_MEMSTATUS "0" CACHE STRING "Determines whether or not sqlite3_status() interfaces that track memory usage to be enabled.")
set_property(CACHE SQLITE_DEFAULT_MEMSTATUS PROPERTY STRINGS 0 1)

set(SQLITE_MAX_EXPR_DEPTH "0" CACHE STRING "Determines the maximum expression tree depth. If the value is 0, then no limit is enforced. The current implementation has a default value of 1000.")
option(SQLITE_OMIT_DECLTYPE "By omitting the (seldom-needed) ability to return the declared type of columns from the result set of query, prepared statements can be made to consume less memory." ON)
option(SQLITE_OMIT_DEPRECATED "Omitting deprecated interfaces and features will reduce the library footprint." ON)
option(SQLITE_OMIT_PROGRESS_CALLBACK "The progress handler callback counter must be checked in the inner loop of the bytecode engine. By omitting this interface, a single conditional is removed from the inner loop of the bytecode engine." ON)
option(SQLITE_OMIT_SHARED_CACHE "Omitting the possibility of using shared cache allows many conditionals in performance-critical sections of the code to be eliminated." ON)
option(SQLITE_USE_ALLOCA "Make use of alloca() for dynamically allocating temporary stack space for use within a single function." ON)
option(SQLITE_ENABLE_RTREE "Determines whether or not to include support for the R*Tree index extension." ON)
option(SQLITE_ENABLE_FTS4 "Determines whether or not versions 3 and 4 of the full-text search engine to be added to the build." OFF)
option(SQLITE_ENABLE_FTS5 "Determines whether or not versions 5 of the full-text search engine (fts5) to be added to the build." ON)
option(SQLITE_ENABLE_MATH_FUNCTIONS "Enables built-in SQL math functions." ON)

list(APPEND SQLITE_OPTIONS SQLITE_LIKE_DOESNT_MATCH_BLOBS SQLITE_OMIT_AUTORESET)
get_directory_property(ALL_OPTIONS CACHE_VARIABLES)
foreach(OPTION ${ALL_OPTIONS})
    if("${OPTION}" MATCHES "^SQLITE_(ENABLE|DISABLE|USE|OMIT)_")
        if(${OPTION})
            list(APPEND SQLITE_OPTIONS ${OPTION})
        endif()
    elseif("${OPTION}" MATCHES "^SQLITE_")
        list(APPEND SQLITE_OPTIONS ${OPTION}=${${OPTION}})
    endif()
endforeach()

set(LIBRARY_TYPE ${BUILD_SQLITE3})
if (LIBRARY_TYPE STREQUAL "ON")
    if(BUILD_SHARED_LIBS)
        set(LIBRARY_TYPE SHARED)
    else()
        set(LIBRARY_TYPE STATIC)
    endif()
endif()

set(HEADER_FILES sqlite3.h sqlite3ext.h)
set(SOURCE_FILES sqlite3.c)

add_library(${PROJECT_NAME} ${LIBRARY_TYPE} ${SOURCE_FILES} ${HEADER_FILES})
add_library(${PACKAGE_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>)
set_target_properties(${PROJECT_NAME} PROPERTIES
        PUBLIC_HEADER "${HEADER_FILES}"
        POSITION_INDEPENDENT_CODE ON
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
)

if(BUILD_SHARED_LIBS OR LIBRARY_TYPE STREQUAL "SHARED")
    if(WIN32)
        list(APPEND SQLITE_OPTIONS "SQLITE_API=__declspec(dllexport)")
        target_compile_definitions(${PROJECT_NAME} INTERFACE "SQLITE_API=__declspec(dllimport)")
    else()
        list(APPEND SQLITE_OPTIONS "SQLITE_API=__attribute__((visibility(\"default\")))")
        target_compile_definitions(${PROJECT_NAME} INTERFACE "SQLITE_API=__attribute__((visibility(\"default\")))")
    endif()
endif()

message(STATUS "Build ${LIBRARY_TYPE} SQLite with options: ${SQLITE_OPTIONS}")
target_compile_definitions(${PROJECT_NAME} PRIVATE ${SQLITE_OPTIONS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
list(APPEND SQLITE_DEPS Threads::Threads ${CMAKE_DL_LIBS})
find_library(Math_LIBRARY NAMES m)
if(Math_LIBRARY)
    list(APPEND SQLITE_DEPS ${Math_LIBRARY})
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE ${SQLITE_DEPS})

list(APPEND ALL_TARGETS ${PROJECT_NAME})

if(BUILD_SHELL)
    list(REMOVE_ITEM SQLITE_OPTIONS "SQLITE_THREADSAFE=1")
    list(REMOVE_ITEM SQLITE_OPTIONS "SQLITE_THREADSAFE=2")
    list(APPEND SQLITE_OPTIONS SQLITE_THREADSAFE=0)
    list(APPEND SQLITE_OPTIONS SQLITE_ENABLE_EXPLAIN_COMMENTS)

    find_path(Readline_INCLUDE_DIR NAMES readline/readline.h)
    find_library(Readline_LIBRARY NAMES readline)
    if(Readline_INCLUDE_DIR AND Readline_LIBRARY)
        list(APPEND SQLITE_OPTIONS HAVE_READLINE)
    else()
        unset(Readline_INCLUDE_DIR CACHE)
        unset(Readline_LIBRARY CACHE)
    endif()

    add_executable(shell shell.c ${SOURCE_FILES})
    set_target_properties(shell PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
    target_compile_definitions(shell PRIVATE ${SQLITE_OPTIONS})
    target_include_directories(shell PRIVATE ${Readline_INCLUDE_DIR})
    target_link_libraries(shell PRIVATE ${Readline_LIBRARY} ${SQLITE_DEPS})
    list(APPEND ALL_TARGETS shell)
endif()



include(GNUInstallDirs)

install(TARGETS ${ALL_TARGETS} EXPORT ${PACKAGE_NAME}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}        
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
