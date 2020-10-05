include_guard()

set(Boost_USE_STATIC_LIBS ON)
list(APPEND CMAKE_PREFIX_PATH $ENV{NU_SDK}/boost/1_72_0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(compile_options /W4)

set(compile_definitions
    /DWIN32_LEAN_AND_MEAN
    /D_WIN32_WINNT=0x0601
)

macro(__list_to_string _list)
    string(REPLACE ";" " " ${_list} "${${_list}}")
endmacro()

__list_to_string(compile_definitions)
__list_to_string(compile_options)


set(CMAKE_C_FLAGS "${compile_options} ${compile_definitions}"
    CACHE STRING "Flags used by the C compiler."
)
set(CMAKE_CXX_FLAGS "/EHsc ${compile_options} ${compile_definitions}"
    CACHE STRING "Flags used by the CXX compiler."
)