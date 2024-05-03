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
    const float vx, const float vy, const float vz,
    const float radius)
    :
    vis::Renderable(vis::RenderableType::UAV),
    mId(id),
    mX(x), mY(y), mZ(z),
    mVx(vx), mVy(vy), mVz(vz),
    mRadius(radius)
{}

void Uav::render() const
{
    constexpr float minVisRadius = 0.3f;
    const float visRadius = std::max(minVisRadius, mRadius);
    DrawSphere(toVisFrame(mX, mY, mZ), visRadius, RED);

    // Velocity
    Vector3 v = toVisFrame(mVx, mVy, mVz);
    float speed = Vector3Length(v);
    constexpr float minSpeedVisThreshold = 0.1f;
    if (speed > minSpeedVisThreshold)
    {
        const Vector3 t = toVisFrame(mX, mY, mZ);
        const Vector3 up = toVisFrame(0.0f, 0.0f, 1.0f);
        const auto [angle, axis] = getAxisAngle(v, up);
        const float velVisLength = visRadius * 1.75f;
        
        rlPushMatrix();
            rlTranslatef(t.x, t.y, t.z);
            rlRotatef(angle, axis.x, axis.y, axis.z);
            DrawCylinder(Vector3Zero(), 0.05f, 0.05f, velVisLength, 8, BLUE);
        rlPopMatrix();
    }
}

Vector3 Uav::getPos() const
{
    return { mX, mY, mZ };
}

Vector3 Uav::getVel() const
{
    return { mVx, mVy, mVz };
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