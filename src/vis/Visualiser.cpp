#pragma once

#include "vis/Visualiser.h"

#include "common/Uav.h"
#include "common/MessageTopics.h"

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
        { 0.0f, 40.0f, 0.1f }, // top down view, avoid ambiguity in camera right direction with small z-value
        { 0.0f, 0.0f, 0.0f },
        20.0f
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
            DrawRectangle( 10, 10, 320, 80, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines( 10, 10, 320, 80, BLUE);

            DrawText("Camera controls:", 20, 20, 10, BLACK);
            DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, DARKGRAY);
            DrawText("- WASD to Pan", 40, 60, 10, DARKGRAY);

            // Reset button
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            Rectangle resetButtonBounds { 25, mParams.mScreenHeight - 50, 125, 30 };
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
    std::lock_guard<std::mutex> guard(mRenderablesMutex);

    for (const auto& renderable : mRenderables)
    {
        renderable->render();
    }

    mRenderables.clear();
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
    subscriber.connect("tcp://localhost:5555");

    subscriber.set(zmq::sockopt::subscribe, topic::UavState);
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
    if (topic == topic::UavState)
    {
        std::lock_guard<std::mutex> guard(mRenderablesMutex);
        mRenderables.emplace_back(
            std::make_unique<common::Uav>(nlohmann::json::parse(message))
        );
    }
    else
    {
        std::cout << "Invalid topic received" << std::endl;
    }
}
}
}