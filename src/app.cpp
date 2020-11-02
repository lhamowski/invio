#include "app.hpp"

namespace invio {

app::app(config& cfg) :
    cfg_{cfg},
    logger_manager_{cfg.log_cfg},
    logger_{logger_manager_.new_logger("app")}
{
    LOG_INFO(logger_, "App started");
}

void app::main_loop()
{
    while (true)
    {
        // TODO
    }
}

}  // namespace invio
