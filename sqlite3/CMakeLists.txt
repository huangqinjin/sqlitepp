cmake_minimum_required(VERSION 3.1)
project(sqlite3 VERSION 3.22.0)

option(BUILD_SHELL "build sqlite3 shell" OFF)

set(HEADER_FILES sqlite3.h sqlite3ext.h)
set(SOURCE_FILES sqlite3.c)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
add_library(sqlitepp::sqlite3 ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADER_FILES}"
		              INTERFACE_LINK_LIBRARIES "${CMAKE_DL_LIBS}")

list(APPEND SQLITE_OPTIONS
            -DSQLITE_ENABLE_JSON1
			-DSQLITE_ENABLE_FTS5
            -DSQLITE_ENABLE_RTREE)

target_compile_options(${PROJECT_NAME} PRIVATE ${SQLITE_OPTIONS})

list(APPEND ALL_TARGETS ${PROJECT_NAME})

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



include(GNUInstallDirs)

install(TARGETS ${ALL_TARGETS} EXPORT sqlitepp-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}        
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
