#pragma once

#include "support/com_interface.hpp"

#include <Audioclient.h>
#include <mmdeviceapi.h>

namespace invio {

class audio_manager final
{
public:
    audio_manager();

private:
    void create_device_enumerator();
    void init_default_device();
    void init_audio_client();
    void init_capture_client();

    invio::com_interface<IMMDeviceEnumerator> device_enumerator_;
    invio::com_interface<IMMDevice> default_device_;
    invio::com_interface<IAudioClient> audio_client_;
    invio::com_interface<IAudioCaptureClient> capture_client_;
};

}  // namespace invio
