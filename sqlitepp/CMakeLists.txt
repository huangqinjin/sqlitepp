cmake_minimum_required(VERSION 3.1)

file(GLOB HEADER_FILES *.hpp)
file(GLOB SOURCE_FILES *.cpp)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
add_library(${PACKAGE_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PACKAGE_NAME}::sqlite3)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADER_FILES}"
                      CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENTIONS OFF
                      WINDOWS_EXPORT_ALL_SYMBOLS ON)


include(GNUInstallDirs)
set(ConfigPackageLocation ${CMAKE_INSTALL_DATADIR}/${PACKAGE_NAME})
#set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME})

install(TARGETS ${PROJECT_NAME} EXPORT ${PACKAGE_NAME}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PACKAGE_NAME})


install(EXPORT ${PACKAGE_NAME}-targets DESTINATION ${ConfigPackageLocation} NAMESPACE ${PACKAGE_NAME}::)

export(EXPORT ${PACKAGE_NAME}-targets NAMESPACE ${PACKAGE_NAME}::)


include(CMakePackageConfigHelpers)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake.in"
"@PACKAGE_INIT@
include(\"\${CMAKE_CURRENT_LIST_DIR}/@PACKAGE_NAME@-targets.cmake\")
if(NOT TARGET sqlitepp::sqlite3)
    include(CMakeFindDependencyMacro)
    list(APPEND CMAKE_MODULE_PATH \${CMAKE_CURRENT_LIST_DIR})
    find_dependency(SQLite3)
endif()
check_required_components(@PACKAGE_NAME@)
")

configure_package_config_file("${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake"
    INSTALL_DESTINATION ${ConfigPackageLocation})

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake"
    COMPATIBILITY AnyNewerVersion)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake"
    "${CMAKE_CURRENT_LIST_DIR}/FindSQLite3.cmake"
    DESTINATION ${ConfigPackageLocation})
