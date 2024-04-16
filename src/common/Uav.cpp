#include "common/Uav.h"

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

Uav::Uav(const nlohmann::json& json)
    : vis::Renderable(vis::RenderableType::UAV)
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

void Uav::render()
{
    constexpr float radius = 0.5f;
    DrawSphere(toVisFrame(mX, mY, mZ), radius, RED);
}
}
}