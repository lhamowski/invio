#pragma once

#include "config.hpp"
#include "core/logger_manager.hpp"

namespace invio {

class app final
{
public:
    app(config& cfg);
    void main_loop();

private:
    config& cfg_;
    logger_manager logger_manager_;
    logger logger_;
};

}  // namespace invio