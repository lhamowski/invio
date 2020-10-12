#include "app.hpp"
#include "config.hpp"
#include "support/error.hpp"

#include <objbase.h>
#include <winerror.h>

namespace {

void init_com()
{
    const auto result = CoInitialize(nullptr);
    invio::throw_if_error<invio::initialization_failed>(
        result, "Cannot initialize the COM library");
}

void run(const invio::config& cfg)
{
    try
    {
        // Initializes the COM library on the current thread and identifies the
        // concurrency model as single-thread apartment (STA).
        init_com();

        invio::app app_;
        app_.main_loop();
    }
    catch (const invio::initialization_failed& ex)
    {
    }
    catch (const std::exception& ex)
    {
        // Unhandled exceptions
        // TODO print error
    }
}
}  // namespace

int main()
{
    const auto cfg = invio::load_config("");

    run(cfg);
    return 0;
}