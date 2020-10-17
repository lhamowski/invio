#pragma once

#include <filesystem>

namespace invio {

void create_app_data_dir();
std::filesystem::path app_data_path();

}  // namespace invio