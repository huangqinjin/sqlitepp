cmake_minimum_required(VERSION 3.1)
project(sqlite3)

set(HEADER_FILES sqlite3.h sqlite3ext.h)
set(SOURCE_FILES sqlite3.c)

add_library(static STATIC ${SOURCE_FILES})
add_library(shared SHARED ${SOURCE_FILES})
add_executable(exe shell.c ${SOURCE_FILES})

set_target_properties(static PROPERTIES OUTPUT_NAME sqlite3)
set_target_properties(shared PROPERTIES OUTPUT_NAME sqlite3)
set_target_properties(exe PROPERTIES OUTPUT_NAME sqlite3 PUBLIC_HEADER "${HEADER_FILES}")

list(APPEND SQLITE_OPTIONS -O4 -Os
            -DSQLITE_ENABLE_JSON1
            -DSQLITE_ENABLE_RTREE)

target_compile_options(static PUBLIC ${SQLITE_OPTIONS})
target_compile_options(shared PUBLIC ${SQLITE_OPTIONS})
target_compile_options(exe PUBLIC ${SQLITE_OPTIONS}
                       -DSQLITE_THREADSAFE=0
                       -DSQLITE_ENABLE_EXPLAIN_COMMENTS
                       -DHAVE_READLINE)

target_link_libraries(exe dl readline)

install(TARGETS static shared exe
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include)
