#include "path.hpp"
#include "error.hpp"

#include <cstdlib>
#include <system_error>

namespace invio {

void create_app_data_dir() 
{
    const auto path = app_data_path();
    std::error_code err{};
    std::filesystem::create_directory(path, err);

    invio::throw_if_error<initialization_failed>(
        err, "Error when creating app data dir: {}", path.string());
}

std::filesystem::path app_data_path()
{
    const auto* app_data_env = std::getenv("APPDATA");
    if (!app_data_env)
    {
        throw initialization_failed{
            "Cannot obtain APPDATA environment variable"};
    }

    return app_data_env + std::string{"/invio"};
}

}  // namespace invio
