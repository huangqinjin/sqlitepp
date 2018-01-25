cmake_minimum_required(VERSION 3.1)
project(sqlitepp VERSION 3.22.0)

file(GLOB HEADER_FILES *.hpp)
file(GLOB SOURCE_FILES *.cpp)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
add_library(sqlitepp::sqlitepp ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/..>)
target_link_libraries(sqlitepp PRIVATE sqlitepp::sqlite3)
set_target_properties(sqlitepp PROPERTIES PUBLIC_HEADER "${HEADER_FILES}"
                      CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENTIONS OFF)


include(GNUInstallDirs)
set(ConfigPackageLocation ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})
#set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})


install(EXPORT ${PROJECT_NAME}-targets DESTINATION ${ConfigPackageLocation} NAMESPACE sqlitepp::)

export(EXPORT ${PROJECT_NAME}-targets)   


include(CMakePackageConfigHelpers)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in"
"@PACKAGE_INIT@
include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}-targets.cmake\")
if(NOT TARGET sqlitepp::sqlite3)
    list(APPEND CMAKE_MODULE_PATH \${CMAKE_CURRENT_LIST_DIR})
    include(CMakeFindDependencyMacro)
    find_dependency(SQLite3)
endif()
")

configure_package_config_file("${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION ${ConfigPackageLocation})

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    COMPATIBILITY AnyNewerVersion)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    "${CMAKE_CURRENT_LIST_DIR}/FindSQLite3.cmake"
    DESTINATION ${ConfigPackageLocation})
