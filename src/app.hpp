#pragma once

#include "core/audio_manager.hpp"

namespace invio {

class app final
{
public:
    app();
    void main_loop();

private:
    audio_manager audio_manager_;
};

}  // namespace invio