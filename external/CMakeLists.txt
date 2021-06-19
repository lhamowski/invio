include(FetchContent)

FetchContent_Declare(
    catch2
    URL https://github.com/catchorg/Catch2/archive/v2.11.3.zip
)
FetchContent_Declare(
    fmt
    URL https://github.com/fmtlib/fmt/archive/7.1.3.zip
)
FetchContent_Declare(
    spdlog
    URL https://github.com/gabime/spdlog/archive/v1.8.1.zip
)
FetchContent_Declare(
    kl
    URL https://github.com/k0zmo/kl/archive/546886d4162f0067e5a0f8df2d451aa34b286bd0.zip
)

FetchContent_GetProperties(catch2)
if(NOT catch2_POPULATED)
    FetchContent_Populate(catch2)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/contrib")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
endif()

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
    set_target_properties(fmt PROPERTIES FOLDER external)
endif()

FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    set(SPDLOG_FMT_EXTERNAL ON)
    set(SPDLOG_ENABLE_PCH ON)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
    target_compile_definitions(spdlog INTERFACE SPDLOG_NO_THREAD_ID)
    set_target_properties(spdlog PROPERTIES FOLDER external)
endif()

FetchContent_GetProperties(kl)
if(NOT kl_POPULATED)
    FetchContent_Populate(kl)
    set(KL_ENABLE_YAML ON)
    set(KL_ENABLE_JSON OFF)
    add_subdirectory(${kl_SOURCE_DIR} ${kl_BINARY_DIR})
    set_target_properties(kl kl-yaml PROPERTIES FOLDER external)
endif()