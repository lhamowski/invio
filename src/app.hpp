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
    core::logger_manager logger_manager_;
    core::logger logger_;
};

}  // namespace invio