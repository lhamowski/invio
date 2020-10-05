#include "core/audio_manager.hpp"

#include "support/assert.hpp"
#include "support/error.hpp"

#include <fmt/format.h>

namespace invio {

audio_manager::audio_manager()
{
    create_device_enumerator();
    init_default_device();
    init_audio_client();
}

void audio_manager::create_device_enumerator()
{
    constexpr GUID device_enumerator_guid = __uuidof(MMDeviceEnumerator);
    const auto error =
        device_enumerator_.create_instance(device_enumerator_guid);
    invio::throw_if_error<invio::initialization_failed>(
        error, "Cannot create device enumerator");
}

void audio_manager::init_default_device()
{
    INVIO_ASSERT(device_enumerator_, "Device enumerator is needed");
    const auto error = device_enumerator_->GetDefaultAudioEndpoint(
        eRender, eMultimedia, default_device_);
    invio::throw_if_error<invio::initialization_failed>(
        error, "Cannot obtain default device.");
}

void audio_manager::init_audio_client()
{
    INVIO_ASSERT(default_device_, "Default device is needed");
    constexpr GUID audio_client_guid = __uuidof(IAudioClient);
    auto error = default_device_->Activate(audio_client_guid, CLSCTX_ALL,
                                           nullptr, audio_client_);
    invio::throw_if_error<invio::initialization_failed>(
        error, "Cannot create audio client");

    WAVEFORMATEX* format{};
    error = audio_client_->GetMixFormat(&format);
    invio::throw_if_error<invio::initialization_failed>(
        error, "Cannot get mix format");

    // Creating an audio stream
    constexpr REFERENCE_TIME requested_duration = 10000000;  // 1 sec
    audio_client_->Initialize(AUDCLNT_SHAREMODE_SHARED,
                              AUDCLNT_STREAMFLAGS_LOOPBACK, 0,
                              requested_duration, format, nullptr);
    invio::throw_if_error<invio::initialization_failed>(
        error, "Cannot create an audio stream");
}

void audio_manager::initialize_capture_client()
{
    constexpr GUID capture_client_guid = __uuidof(IAudioCaptureClient);

    const auto error =
        audio_client_->GetService(capture_client_guid, capture_client_);
    invio::throw_if_error<invio::initialization_failed>(
        error, "Cannot get a capture audio client");
}

}  // namespace invio