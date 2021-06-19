cmake_minimum_required(VERSION 3.16)
project(invio VERSION 1.0 LANGUAGES CXX)

set(Boost_USE_STATIC_LIBS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(compile_options /W4 /wd4127 /wd4100 /wd5040) 

set(compile_definitions
    /DWIN32_LEAN_AND_MEAN
    /D_WIN32_WINNT=0x0601
    /D_CRT_SECURE_NO_WARNINGS
    /D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
)

macro(to_string _list)
    string(REPLACE ";" " " ${_list} "${${_list}}")
endmacro()

to_string(compile_definitions)
to_string(compile_options)

set(CMAKE_C_FLAGS "${compile_options} ${compile_definitions}"
    CACHE STRING "Flags used by the C compiler." FORCE
)
set(CMAKE_CXX_FLAGS "/EHsc ${compile_options} ${compile_definitions}"
    CACHE STRING "Flags used by the CXX compiler." FORCE
)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(INVIO_TEST ON)

find_package(Boost 1.73.0 REQUIRED
    COMPONENTS
        system
)

add_subdirectory(src)
add_subdirectory(external)

if(INVIO_TEST)
    include(Catch)
    enable_testing()
    add_subdirectory(test)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT invio)