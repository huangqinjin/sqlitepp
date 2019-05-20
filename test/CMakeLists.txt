cmake_minimum_required(VERSION 3.1)
project(sqlitepp_test)

file(GLOB SRC_FILES *.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE tut)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlitepp::sqlitepp sqlitepp::sqlite3)
target_compile_definitions(${PROJECT_NAME} PRIVATE utf=u8string utf8=u8string string_t=u8string)
