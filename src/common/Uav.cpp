#include "common/Uav.h"

#include "common/Geometry.h"

#include "raymath.h"
#include "rlgl.h"

#include <iostream>

namespace swarmgame
{
namespace common
{
Uav::Uav()
    : vis::Renderable(vis::RenderableType::UAV)
{}

Uav::Uav(const int32_t id,
    const float x, const float y, const float z,
    const float vx, const float vy, const float vz)
    :
    vis::Renderable(vis::RenderableType::UAV),
    mId(id),
    mX(x), mY(y), mZ(z),
    mVx(vx), mVy(vy), mVz(vz)
{}

void Uav::render() const
{
    constexpr float radius = 0.5f;
    DrawSphere(toVisFrame(mX, mY, mZ), radius, RED);

    // Velocity
    Vector3 v = toVisFrame(mVx, mVy, mVz);
    float speed = Vector3Length(v);
    constexpr float minSpeedVisThreshold = 0.1f;
    if (speed > minSpeedVisThreshold)
    {
        Vector3 t = toVisFrame(mX, mY, mZ);
        Vector3 up = toVisFrame(0.0f, 0.0f, 1.0f);
        auto [angle, axis] = getAxisAngle(v, up);

        rlPushMatrix();
            rlTranslatef(t.x, t.y, t.z);
            rlRotatef(angle, axis.x, axis.y, axis.z);
            DrawCylinder(Vector3Zero(), 0.1f, 0.1f, 1.0f, 8, BLUE);
        rlPopMatrix();
    }
}

void Uav::fromJson(const nlohmann::json& json)
{
    mId = static_cast<int32_t>(json.at("id"));
    mX = static_cast<float>(json.at("x"));
    mY = static_cast<float>(json.at("y"));
    mZ = static_cast<float>(json.at("z"));
    mVx = static_cast<float>(json.at("vx"));
    mVy = static_cast<float>(json.at("vy"));
    mVz = static_cast<float>(json.at("vz"));
}

nlohmann::json Uav::toJson() const
{
    nlohmann::json json;
    json.emplace("id", mId);
    json.emplace("x", mX);
    json.emplace("y", mY);
    json.emplace("z", mZ);
    json.emplace("vx", mVx);
    json.emplace("vy", mVy);
    json.emplace("vz", mVz);
    return json;
}
}
}