#pragma once

#include <string>

#include "common/JsonWrapper.h"

namespace swarmgame
{
namespace util
{
// Searches for filename recursively upwards, up till a max number of levels
std::string upwardsFileSearch(const std::string& filename);

// Searches for JSON file and reads it
nlohmann::json parseJsonFile(const std::string& filename);

// Merges two JSON objects, overwriting values in a with b
nlohmann::json mergeJson(const nlohmann::json& a, const nlohmann::json& b);
}
}