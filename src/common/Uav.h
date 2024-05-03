#pragma once

#include "common/Renderable.h"

#include "common/JsonWrapper.h"

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
        const float vx, const float vy, const float vz,
        const float radius);

    void render() const override;

    Vector3 getPos() const;
    Vector3 getVel() const;

    void fromJson(const nlohmann::json& json); // for reading from config file
    nlohmann::json toJson() const; // for sending to controllers

    int32_t mId { 0 };
    float mX { 0.0f };
    float mY { 0.0f };
    float mZ { 0.0f };
    float mVx { 0.0f };
    float mVy { 0.0f };
    float mVz { 0.0f };
    float mRadius { 0.0f };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        Uav,
        mId,
        mX, mY, mZ,
        mVx, mVy, mVz,
        mRadius);
};
}
}