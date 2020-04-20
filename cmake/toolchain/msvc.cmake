include_guard()

set(Boost_USE_STATIC_LIBS ON)
list(APPEND CMAKE_PREFIX_PATH $ENV{NU_SDK}/boost/1_72_0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_FLAGS 
    "/DWIN32_LEAN_AND_MEAN"
    "/D_UNICODE"
    "/DUNICODE"
    "/W4" 
    CACHE 
    STRING "Flags used by C compiler")

set(CMAKE_CXX_FLAGS 
    "/DWIN32_LEAN_AND_MEAN"
    "/D_UNICODE"
    "/DUNICODE"
    "/W4" 
    CACHE 
    STRING "Flags used by C++ compiler")