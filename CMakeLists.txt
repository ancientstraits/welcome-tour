cmake_minimum_required(VERSION 3.10)
project(welcome-tour)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)

set(TARGET_RESOURCE ${CMAKE_BINARY_DIR}/resource.c)
add_custom_command(
    OUTPUT ${TARGET_RESOURCE}
    COMMAND glib-compile-resources --target=${TARGET_RESOURCE} --generate-source ${CMAKE_CURRENT_SOURCE_DIR}/assets/gresource.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets
    VERBATIM
)

add_custom_target(
    RESOURCE_FILE
    DEPENDS ${TARGET_RESOURCE}
)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
add_executable(welcome-tour main.cc ${TARGET_RESOURCE})
add_dependencies(welcome-tour RESOURCE_FILE)
target_link_libraries(welcome-tour ${GTKMM_LIBRARIES})

install(TARGETS welcome-tour DESTINATION bin)