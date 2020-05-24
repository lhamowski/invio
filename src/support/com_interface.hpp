#pragma once

#include <comdef.h>
#include <mmdeviceapi.h>
#include <winerror.h>

#include <string>
#include <system_error>

namespace invio::support {

template <typename Intf>
class com_interface final
{
public:
    com_interface() = default;
    ~com_interface() { release(); }

    com_interface(const com_interface&) = delete;
    com_interface& operator=(const com_interface&) = delete;

    Intf* operator->() const { return intf_; }
    operator void**() { return reinterpret_cast<void**>(&intf_); }
    operator Intf**() { return &intf_; }
    operator bool() { return intf_ != nullptr; }

    std::error_code create_instance(
        const GUID& obj_guid,
        CLSCTX context = static_cast<CLSCTX>(CLSCTX_ALL),
        IUnknown* aggregate = nullptr)
    {
        if (intf_)
            release();

        const auto result = CoCreateInstance(obj_guid, aggregate, context,
                                             __uuidof(Intf), *this);
        return {result, std::system_category()};
    }

    void release()
    {
        if (intf_)
        {
            intf_->Release();
            intf_ = nullptr;
        }
    }

private:
    Intf* intf_{nullptr};
};

}  // namespace invio::support