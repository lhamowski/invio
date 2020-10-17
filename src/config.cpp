#include "config.hpp"
#include "support/error.hpp"

#include <fstream>

#include <kl/yaml.hpp>

namespace invio {

namespace {

config read_config(std::ifstream& file)
{
    try
    {
        auto root = YAML::Load(file);
        return kl::yaml::deserialize<config>(root);
    }
    catch (const std::exception& ex)
    {
        throw invio::initialization_failed{
            "Cannot deserialize config file:\n{}", ex.what()};
    }
}

config generate_config(const std::filesystem::path& filepath)
{
    const config cfg{};
    std::ofstream file{filepath, std::ios::out | std::ios::trunc};
    if (file.good())
        file << kl::yaml::serialize(cfg);

    return cfg;
}

}  // namespace

config load_config(const std::filesystem::path& filepath)
{
    std::ifstream file{filepath};
    if (file.good())
        return read_config(file);

    // If file does not exist or it is empty
    return generate_config(filepath);
}

}  // namespace invio
