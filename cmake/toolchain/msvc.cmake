include_guard()

set(CMAKE_C_FLAGS 
    "/DWIN32_LEAN_AND_MEAN /W4" 
    CACHE 
    STRING "Flags used by C compiler")

set(CMAKE_CXX_FLAGS 
    "/DWIN32_LEAN_AND_MEAN /W4" 
    CACHE 
    STRING "Flags used by C++ compiler")