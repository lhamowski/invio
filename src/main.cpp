#include "app.hpp"

#include <config.hpp>
#include <core/logger_manager.hpp>
#include <support/error.hpp>
#include <support/path.hpp>

#include <fmt/core.h>

#include <objbase.h>
#include <winerror.h>
#include <filesystem>
#include <iostream>

namespace {

void init_com()
{
    // Initializes the COM library on the current thread and identifies the
    // concurrency model as single-thread apartment (STA).

    const auto result = CoInitialize(nullptr);
    invio::throw_if_error<invio::initialization_failed>(
        result, "Cannot initialize the COM library");
}

void run()
{
    init_com();

    const auto data_path = invio::app_data_path();
    if (!std::filesystem::exists(data_path))
        invio::create_app_data_dir();

    auto config = invio::load_config(data_path / "invio.cfg");

    invio::app app{config};
    app.main_loop();
}

template <typename... Args>
void print_error(const char* msg, const Args&... args)
{
    std::cerr << fmt::format(msg, args...) << std::endl;
}

}  // namespace

int main()
{
    try
    {
        run();
    }
    catch (const invio::initialization_failed& ex)
    {
        print_error("Initialization failed. Details: {}", ex.what());
    }
    catch (const std::exception& ex)
    {
        print_error("Unhandled exception occured. Details: {}", ex.what());
    }

    return 0;
}