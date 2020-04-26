include(FetchContent)

FetchContent_Declare(
    catch2
    URL https://github.com/catchorg/Catch2/archive/v2.11.3.zip
)

FetchContent_Declare(
    fmt
    URL https://github.com/fmtlib/fmt/archive/6.2.0.zip
)

FetchContent_GetProperties(catch2)
if(NOT catch2_POPULATED)
    FetchContent_Populate(catch2)
    add_library(Catch2 INTERFACE IMPORTED GLOBAL)
    set(include_directories ${catch2_SOURCE_DIR}/single_include)
    set_target_properties(Catch2 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${include_directories}"
    )
endif()

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
    set_target_properties(fmt PROPERTIES FOLDER external)
endif()