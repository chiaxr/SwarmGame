#include "common/FileUtils.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>

#define MAX_LEVELS 5

namespace swarmgame
{
namespace util
{
std::string upwardsFileSearch(const std::string& filename)
{
    std::filesystem::path cwd = std::filesystem::current_path();
    for (int i = 0; i < MAX_LEVELS; i++)
    {
        const std::filesystem::path filenamePath(cwd / filename);

        // Found file
        if (std::filesystem::exists(filenamePath))
        {
            return filenamePath.string();
        }
        // Current directory has no parent
        else if (!cwd.has_parent_path())
        {
            break;
        }
        // Go up a directory
        else
        {
            cwd = cwd.parent_path();
        }
    }

    // File not found
    throw std::runtime_error(std::format("File {} not found.", filename));
}

nlohmann::json parseJsonFile(const std::string& filename)
{
    std::string filePath = upwardsFileSearch(filename);
    std::ifstream f(filePath);
    return nlohmann::json::parse(f);
}

nlohmann::json mergeJson(const nlohmann::json& a, const nlohmann::json& b)
{
    nlohmann::json merged = a;
    merged.update(b, true);
    return merged;
}
}
}