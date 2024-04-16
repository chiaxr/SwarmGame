#pragma once

#include "common/Renderable.h"

#include "nlohmann/json.hpp"

#include <cstdint>

namespace swarmgame
{
namespace common
{
struct Uav : public vis::Renderable
{
    Uav();

    Uav(const int32_t id,
        const float x, const float y, const float z,
        const float vx, const float vy, const float vz);

    Uav(const nlohmann::json& json);

    nlohmann::json toJson() const;
    void render() override;

    int32_t mId { 0 };
    float mX { 0.0f };
    float mY { 0.0f };
    float mZ { 0.0f };
    float mVx { 0.0f };
    float mVy { 0.0f };
    float mVz { 0.0f };
};
}
}