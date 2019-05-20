cmake_minimum_required(VERSION 3.1)

project(ex1)
add_executable(ex1 ex1.cpp)
target_link_libraries(ex1 sqlitepp::sqlitepp)

project(ex2)
add_executable(ex2 ex2.cpp)
target_link_libraries(ex2 sqlitepp::sqlitepp)
