#pragma once

#include "raylib.h"

namespace swarmgame
{
namespace vis
{
class CameraController
{
public:
    CameraController() = default;
    CameraController(const Vector3& position, const Vector3& target, const float fovy);

    void init(const Vector3& position, const Vector3& target, const float fovy);
    void update();
    Camera3D getCamera();

private:
    Camera3D mCamera = { 0 };
};
}
}