#include "vis/Visualiser.h"

#include "common/Uav.h"
#include "common/JsonWrapper.h"
#include "common/MessageTopics.h"
#include "vis/VisPrimitives.h"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "zmq.hpp"
#include "zmq_addon.hpp"

#include <iostream>

namespace swarmgame
{
namespace vis
{
void Visualiser::init()
{
    mFutureRun = std::async(std::launch::async, &Visualiser::run, this);
    mFutureRunSubscriber = std::async(std::launch::async, &Visualiser::runSubscriber, this);
}

void Visualiser::run()
{
    // Init
    InitWindow(mParams.mScreenWidth, mParams.mScreenHeight, "SwarmGame");

    mCameraController.init(
        CameraControllerMode::TOP_DOWN,
        { 0.0f, 30.0f, 0.01f }, // top down view, avoid ambiguity in camera right direction with small z-value
        { 0.0f, 0.0f, 0.0f },
        40.0f
    );

    SetTargetFPS(30);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Handle inputs
        handleInput();

        // Draw
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(mCameraController.getCamera());

                DrawGrid(100, 1.0f);

                render();

            EndMode3D();

            // Tooltip window
            DrawRectangle( 10, 10, 320, 120, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines( 10, 10, 320, 120, BLUE);

            DrawText("Camera controls:", 20, 20, 10, BLACK);
            if (mCameraController.getCameraMode() == CameraControllerMode::TOP_DOWN)
            {
                DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, DARKGRAY);
                DrawText("- WASD to Pan", 40, 60, 10, DARKGRAY);
            }
            else
            {
                DrawText("- Mouse L-Click to Pan", 40, 40, 10, DARKGRAY);
                DrawText("- Mouse Wheel to Zoom in-out", 40, 60, 10, DARKGRAY);
                DrawText("- WASD to Move horizontally", 40, 80, 10, DARKGRAY);
                DrawText("- Space/L-Ctrl to Move vertically", 40, 100, 10, DARKGRAY);
            }

            // Camera switch button
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            Rectangle cameraSwitchButtonBounds { static_cast<float>(mParams.mScreenWidth - 150), static_cast<float>(mParams.mScreenHeight - 50), 125.0f, 30.0f };
            const int cameraSwitchButtonIcon = (mCameraController.getCameraMode() == CameraControllerMode::TOP_DOWN) ? ICON_MODE_2D : ICON_MODE_3D;
            if (GuiButton(cameraSwitchButtonBounds, GuiIconText(cameraSwitchButtonIcon, "Switch Camera")))
            {
                mCameraController.switchCameraMode();
            }

            // Reset button
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            Rectangle resetButtonBounds { 25.0f, static_cast<float>(mParams.mScreenHeight - 50), 125.0f, 30.0f };
            if (GuiButton(resetButtonBounds, GuiIconText(ICON_UNDO_FILL, "Reset")))
            {
                sendResetSignal();
            }

        EndDrawing();
    }

    // Clean up
    CloseWindow(); // Close window and OpenGL context
    
    // Close all other threads
    sendShutdownSignal();
}

void Visualiser::render()
{
    renderEnvironment();
    renderModelRenderables();
}

void Visualiser::renderEnvironment()
{
    for (const auto& obstacle : mObstacles)
    {
        obstacle.render();
    }
}

void Visualiser::renderModelRenderables()
{
    std::lock_guard<std::mutex> guard(mModelRenderablesMutex);

    for (const auto& renderable : mModelRenderables)
    {
        if (renderable)
        {
            renderable->render();
        }
    }
}

void Visualiser::handleInput()
{
    mCameraController.update();
}

void Visualiser::sendResetSignal()
{
    zmq::socket_t publisher(*mCtx, zmq::socket_type::pub);
    publisher.connect("inproc://" + topic::Reset);

    // Give the subscribers a chance to connect, so they don't lose any messages
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    publisher.send(zmq::buffer(topic::Reset), zmq::send_flags::sndmore);
    publisher.send(zmq::str_buffer("zz"));
}

void Visualiser::sendShutdownSignal()
{
    zmq::socket_t publisher(*mCtx, zmq::socket_type::pub);
    publisher.bind("inproc://" + topic::Shutdown);

    // Give the subscribers a chance to connect, so they don't lose any messages
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    while (mRunning)
    {
        
        publisher.send(zmq::buffer(topic::Shutdown), zmq::send_flags::sndmore);
        publisher.send(zmq::str_buffer("gg"));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void Visualiser::runSubscriber()
{
    zmq::socket_t subscriber(*mCtx, zmq::socket_type::sub);
    subscriber.bind("inproc://" + topic::Renderables);

    subscriber.set(zmq::sockopt::subscribe, topic::Renderables);
    subscriber.set(zmq::sockopt::rcvtimeo, mParams.mMessageTimeoutMs);

    while (mRunning)
    {
        std::vector<zmq::message_t> received;
        zmq::recv_result_t result = zmq::recv_multipart(subscriber, std::back_inserter(received)); // blocks until message received
        
        if (result && result == 2)
        {
            processMessage(received.at(0).to_string(), received.at(1).to_string());
        }
    }
}

void Visualiser::processMessage(const std::string& topic, const std::string& message)
{
    if (topic == topic::Renderables)
    {
        std::vector<std::shared_ptr<Renderable>> renderables;
        std::map<vis::RenderableType, std::vector<nlohmann::json>> renderablesMap = nlohmann::json::parse(message);
        for (const auto& [renderType, serialised] : renderablesMap)
        {
            if (renderType == vis::RenderableType::UAV)
            {
                for (const auto& renderable : serialised)
                {
                    renderables.emplace_back(std::make_shared<common::Uav>(renderable));
                }
            }
            else if (renderType == vis::RenderableType::LINE)
            {
                for (const auto& renderable : serialised)
                {
                    renderables.emplace_back(std::make_shared<vis::Line>(renderable));
                }
            }
            else
            {
                std::cout << "Invalid render type received" << std::endl;
            }
        }

        std::lock_guard<std::mutex> guard(mModelRenderablesMutex);
        mModelRenderables.swap(renderables);
    }
    else
    {
        std::cout << "Invalid topic received" << std::endl;
    }
}
}
}