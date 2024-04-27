#include "vis/CameraController.h"

#include "rcamera.h"

#include <iostream>

namespace swarmgame
{
namespace vis
{
CameraController::CameraController(const CameraControllerMode mode, const Vector3& position, const Vector3& target, const float fovy)
{
    init(mode, position, target, fovy);
}

void CameraController::init(const CameraControllerMode mode, const Vector3& position, const Vector3& target, const float fovy)
{
    mMode = mode;

    mCamera.position = position;
    mCamera.target = target;
    mCamera.up = { 0.0f, 1.0f, 0.0f };
    mCamera.fovy = fovy;
    mCamera.projection = CAMERA_PERSPECTIVE;
}

void CameraController::update()
{
    constexpr float CAMERA_MOVE_SPEED = 0.2f;
    constexpr float CAMERA_MOUSE_SCROLL_SENSITIVITY = 1.8f;
    constexpr float CAMERA_ROTATION_SPEED = 0.03f; // in radians
    constexpr float CAMERA_MOUSE_TRANSLATION_SENSITIVITY = 0.05f;
    constexpr float CAMERA_MOUSE_ROTATION_SENSITIVITY = 0.003f; // in radians

    if (mMode == CameraControllerMode::TOP_DOWN)
    {
        // Horizontal movement
        if (IsKeyDown(KEY_W)) CameraMoveForward(&mCamera, CAMERA_MOVE_SPEED, true);
        if (IsKeyDown(KEY_A)) CameraMoveRight(&mCamera, -CAMERA_MOVE_SPEED, true);
        if (IsKeyDown(KEY_S)) CameraMoveForward(&mCamera, -CAMERA_MOVE_SPEED, true);
        if (IsKeyDown(KEY_D)) CameraMoveRight(&mCamera, CAMERA_MOVE_SPEED, true);

        // Vertical movement
        CameraMoveUp(&mCamera, -GetMouseWheelMove() * CAMERA_MOUSE_SCROLL_SENSITIVITY);

        // Mouse panning
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePositionDelta = GetMouseDelta();
            CameraMoveForward(&mCamera, mousePositionDelta.y * CAMERA_MOUSE_TRANSLATION_SENSITIVITY, true);
            CameraMoveRight(&mCamera, -mousePositionDelta.x * CAMERA_MOUSE_TRANSLATION_SENSITIVITY, true);
        }
    }
    else
    {
        // Horizontal movement
        if (IsKeyDown(KEY_W)) CameraMoveForward(&mCamera, CAMERA_MOVE_SPEED, true);
        if (IsKeyDown(KEY_A)) CameraMoveRight(&mCamera, -CAMERA_MOVE_SPEED, true);
        if (IsKeyDown(KEY_S)) CameraMoveForward(&mCamera, -CAMERA_MOVE_SPEED, true);
        if (IsKeyDown(KEY_D)) CameraMoveRight(&mCamera, CAMERA_MOVE_SPEED, true);

        // Vertical movement
        if (IsKeyDown(KEY_SPACE)) CameraMoveUp(&mCamera, CAMERA_MOVE_SPEED);
        if (IsKeyDown(KEY_LEFT_CONTROL)) CameraMoveUp(&mCamera, -CAMERA_MOVE_SPEED);

        // Yaw rotation
        if (IsKeyDown(KEY_Q)) CameraYaw(&mCamera, CAMERA_ROTATION_SPEED, false);
        if (IsKeyDown(KEY_E)) CameraYaw(&mCamera, -CAMERA_ROTATION_SPEED, false);
    
        // Mouse panning
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePositionDelta = GetMouseDelta();
            CameraYaw(&mCamera, -mousePositionDelta.x * CAMERA_MOUSE_ROTATION_SENSITIVITY, false);
            CameraPitch(&mCamera, -mousePositionDelta.y * CAMERA_MOUSE_ROTATION_SENSITIVITY, true, false, false);
        }

        // Mouse zoom
        CameraMoveForward(&mCamera, GetMouseWheelMove() * CAMERA_MOUSE_SCROLL_SENSITIVITY, false);
    }
}

void CameraController::switchCameraMode()
{
    constexpr float OFFSET_DIST = 20.0f;
    constexpr float TOP_DOWN_HEIGHT = 30.0f;

    // Reset up direction
    mCamera.up = { 0.0f, 1.0f, 0.0f };

    if (mMode == CameraControllerMode::TOP_DOWN)
    {
        mMode = CameraControllerMode::FREE;

        // Target ground at initial position
        mCamera.target = mCamera.position;
        mCamera.target.y = 0.0f;

        // Move camera lower and angled at an offset
        mCamera.position.y = OFFSET_DIST;
        mCamera.position.z += OFFSET_DIST;
    }
    else
    {
        mMode = CameraControllerMode::TOP_DOWN;

        // Set high camera height
        mCamera.position.y = TOP_DOWN_HEIGHT;

        // Undo offset in z-axis, to prevent camera movement when switching modes
        mCamera.position.z -= OFFSET_DIST;

        // Look directly downwards, avoid ambiguity in camera direction with small z-value offset
        mCamera.target = mCamera.position;
        mCamera.target.y = 0.0f;
        mCamera.target.z -= 0.01f;
    }
}

Camera3D CameraController::getCamera()
{
    return mCamera;
}

CameraControllerMode CameraController::getCameraMode()
{
    return mMode;
}
}
}