#pragma once

#include "raylib.h"

namespace swarmgame
{
namespace vis
{
enum class CameraControllerMode
{
    UNKNOWN,
    TOP_DOWN,
    FREE
};

class CameraController
{
public:
    CameraController() = default;
    CameraController(const CameraControllerMode mode, const Vector3& position, const Vector3& target, const float fovy);

    void init(const CameraControllerMode mode, const Vector3& position, const Vector3& target, const float fovy);
    void update();
    void switchCameraMode();
    Camera3D getCamera();
    CameraControllerMode getCameraMode();

private:
    CameraControllerMode mMode { CameraControllerMode::UNKNOWN };
    Camera3D mCamera;
};
}
}