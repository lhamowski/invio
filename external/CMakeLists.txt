include(FetchContent)

FetchContent_Declare(
    catch
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.11.1
)

FetchContent_GetProperties(catch)
if(NOT catch_POPULATED)
    FetchContent_Populate(catch)
    add_library(Catch INTERFACE IMPORTED GLOBAL)
    set(include_directories ${catch_SOURCE_DIR}/single_include)
    set_target_properties(Catch PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${include_directories}"
    )
endif()