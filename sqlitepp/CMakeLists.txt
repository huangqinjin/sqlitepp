cmake_minimum_required(VERSION 3.14)

file(GLOB HEADER_FILES *.hpp)
file(GLOB SOURCE_FILES *.cpp)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
add_library(${PACKAGE_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PACKAGE_NAME}::sqlite3)
set_target_properties(${PROJECT_NAME} PROPERTIES
        PUBLIC_HEADER "${HEADER_FILES}"
        CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENTIONS OFF
        POSITION_INDEPENDENT_CODE ON
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
)

get_target_property(PROJECT_LIBRARY_TYPE ${PROJECT_NAME} TYPE)
string(TOUPPER ${PROJECT_NAME} PROJECT_UPPER_NAME)
string(MAKE_C_IDENTIFIER ${PROJECT_UPPER_NAME} PROJECT_UPPER_NAME)
target_compile_definitions(${PROJECT_NAME} INTERFACE ${PROJECT_UPPER_NAME}_${PROJECT_LIBRARY_TYPE})


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
    find_dependency(SQLite3)
    if(TARGET SQLite::SQLite3)
        add_library(sqlitepp::sqlite3 ALIAS SQLite::SQLite3)
    endif()
endif()
check_required_components(@PACKAGE_NAME@)
")

configure_package_config_file("${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake"
    INSTALL_DESTINATION ${ConfigPackageLocation})

write_basic_package_version_file(
    "${PACKAGE_NAME}-config-version.cmake"
    COMPATIBILITY SameMinorVersion)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake"
    DESTINATION ${ConfigPackageLocation})
