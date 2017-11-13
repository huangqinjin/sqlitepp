cmake_minimum_required(VERSION 3.1)
project(sqlitepp)

file(GLOB HEADER_FILES *.hpp)
file(GLOB SOURCE_FILES *.cpp)

add_library(sqlitepp ${SOURCE_FILES})
add_library(sqlitepp::sqlitepp ALIAS sqlitepp)
target_compile_options(sqlitepp PRIVATE -O3 -Os)
target_link_libraries(sqlitepp PRIVATE sqlitepp::sqlite3)
set_target_properties(sqlitepp PROPERTIES PUBLIC_HEADER "${HEADER_FILES}"
                      INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/.."
                      CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENTIONS OFF)

install(TARGETS sqlitepp DESTINATION lib
        PUBLIC_HEADER DESTINATION include/sqlitepp)