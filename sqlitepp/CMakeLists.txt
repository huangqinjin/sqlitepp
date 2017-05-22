cmake_minimum_required(VERSION 3.1)
project(sqlitepp)

set(CMAKE_CXX_STANDARD 11)

file(GLOB HEADER_FILES *.hpp)
file(GLOB SOURCE_FILES *.cpp)

add_library(sqlitepp ${SOURCE_FILES})

target_compile_options(sqlitepp PUBLIC -O4 -Os)
set_target_properties(sqlitepp PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

install(TARGETS sqlitepp DESTINATION lib
        PUBLIC_HEADER DESTINATION include/sqlitepp)