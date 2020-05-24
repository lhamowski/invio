#pragma once

#include "support/com_interface.hpp"

#include <mmdeviceapi.h>
#include <Audioclient.h>

namespace invio::core {

class audio_manager final
{
public:
    audio_manager();

private:
    void create_device_enumerator();
    void initialize_default_device();
    void initialize_audio_client();
    void initialize_capture_client();

    invio::support::com_interface<IMMDeviceEnumerator> device_enumerator_;
    invio::support::com_interface<IMMDevice> default_device_;
    invio::support::com_interface<IAudioClient> audio_client_;
    invio::support::com_interface<IAudioCaptureClient> capture_client_;
};

}  // namespace invio::core
