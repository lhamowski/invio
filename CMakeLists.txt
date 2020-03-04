cmake_minimum_required(VERSION 3.16)
project(invio VERSION 1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${invio_SOURCE_DIR}/cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(INVIO_TEST ON)

add_subdirectory(src)
add_subdirectory(external)

if(INVIO_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT invio)