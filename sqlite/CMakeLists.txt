cmake_minimum_required(VERSION 3.1)
project(sqlite)

option(BUILD_SHELL "build sqlite3 shell" OFF)

set(HEADER_FILES sqlite3.h sqlite3ext.h)
set(SOURCE_FILES sqlite3.c)

add_library(sqlite ${SOURCE_FILES})
add_library(sqlitepp::sqlite3 ALIAS sqlite)
set_target_properties(sqlite PROPERTIES OUTPUT_NAME sqlite3 PUBLIC_HEADER "${HEADER_FILES}"
		              INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}"
		              INTERFACE_LINK_LIBRARIES "${CMAKE_DL_LIBS}")

list(APPEND SQLITE_OPTIONS -O3 -Os
            -DSQLITE_ENABLE_JSON1
			-DSQLITE_ENABLE_FTS5
            -DSQLITE_ENABLE_RTREE)

target_compile_options(sqlite PRIVATE ${SQLITE_OPTIONS})

list(APPEND ALL_TARGETS sqlite)

if(BUILD_SHELL)
add_executable(shell shell.c ${SOURCE_FILES})
set_target_properties(shell PROPERTIES OUTPUT_NAME sqlite3)
target_compile_options(shell PRIVATE ${SQLITE_OPTIONS}
                       -DSQLITE_THREADSAFE=0
                       -DSQLITE_ENABLE_EXPLAIN_COMMENTS
                       -DHAVE_READLINE)
target_link_libraries(shell m dl readline)
list(APPEND ALL_TARGETS shell)
endif()


install(TARGETS ${ALL_TARGETS}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include)
