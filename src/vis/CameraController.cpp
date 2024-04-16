#include "vis/CameraController.h"

#include "rcamera.h"

#include <iostream>

#define CAMERA_MOVE_SPEED 0.2f
#define CAMERA_MOUSE_SCROLL_SENSITIVITY 1.5f

namespace swarmgame
{
namespace vis
{
CameraController::CameraController(const Vector3& position, const Vector3& target, const float fovy)
{
    init(position, target, fovy);
}

void CameraController::init(const Vector3& position, const Vector3& target, const float fovy)
{
    mCamera.position = position;
    mCamera.target = target;
    mCamera.up = { 0.0f, 10.0f, 0.0f };
    mCamera.fovy = fovy;
    mCamera.projection = CAMERA_PERSPECTIVE;
}

void CameraController::update()
{
    // Camera movement
    if (IsKeyDown(KEY_W)) CameraMoveForward(&mCamera, CAMERA_MOVE_SPEED, true);
    if (IsKeyDown(KEY_A)) CameraMoveRight(&mCamera, -CAMERA_MOVE_SPEED, true);
    if (IsKeyDown(KEY_S)) CameraMoveForward(&mCamera, -CAMERA_MOVE_SPEED, true);
    if (IsKeyDown(KEY_D)) CameraMoveRight(&mCamera, CAMERA_MOVE_SPEED, true);

    CameraMoveUp(&mCamera, -GetMouseWheelMove() * CAMERA_MOUSE_SCROLL_SENSITIVITY);
}

Camera3D CameraController::getCamera()
{
    return mCamera;
}
}
}