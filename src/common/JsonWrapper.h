#pragma once

#include "nlohmann/json.hpp"
#include "raylib.h"

#include <memory>
#include <vector>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <typename T>
struct adl_serializer<std::shared_ptr<T>>
{
    static void to_json(json& j, const std::shared_ptr<T>& ptr)
    {
        if (ptr)
        {
            j = *ptr;
        }
        else
        {
            j = nullptr;
        }
    }

    static void from_json(const json& j, std::shared_ptr<T>& ptr)
    {
        if (j.is_null())
        {
            ptr = nullptr;
        }
        else
        {
            ptr = j.get<T>();
        }
    }
};

NLOHMANN_JSON_NAMESPACE_END

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector3, x, y, z);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Color, r, g, b, a);