
cmake_minimum_required(VERSION 3.1)
project(BLEMedical 
        VERSION 0.0.1
        LANGUAGES C)
include(GNUInstallDirs)
set(CMAKE_VERBOSE_MAKEFILE on)
add_executable(blemed main.c ble_medical_data.c ble_medical_filebrowsing.c ble_medical_bluetooth.c ble_medical_plot.c gtkchart.c)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)
#target_compile_definitions(blemed PRIVATE ${GTK_CFLAGS_OTHER})
target_compile_definitions(blemed PUBLIC DEBUG)
target_link_libraries(blemed PUBLIC pthread glib-2.0 gio-2.0 gobject-2.0)
target_include_directories(blemed PUBLIC ${GTK_INCLUDE_DIRS})
target_link_directories(blemed PUBLIC ${GTK_LIBRARY_DIRS})
target_link_libraries(blemed PUBLIC ${GTK_LIBRARIES})
#include(FetchContent)
#FetchContent_Declare(
#        simpleble
#        GIT_REPOSITORY https://github.com/OpenBluetoothToolbox/SimpleBLE
#        GIT_TAG  v0.6.0
#        GIT_SHALLOW YES
#)
#FetchContent_GetProperties(simpleble)
#if(NOT simpleble_POPULATED)
#        FetchContent_Populate(simpleble)
#        list (APPEND CMAKE_MODULE_PATH "${simpleble_SOURCE_DIR}/cmake/find")
#       add_subdirectory("${simpleble_SOURCE_DIR}/simpleble" "${simpleble_BINARY_DIR}")
#endif()

#set(simpleble_FOUND 1)
find_package(simpleble REQUIRED CONFIG)
#target_link_libraries(blemed PUBLIC simpleble::simpleble)
target_link_libraries(blemed PUBLIC simpleble-c)

