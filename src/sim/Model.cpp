#pragma once

#include "sim/Model.h"
#include "common/MessageTopics.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include <chrono>
#include <iostream>

namespace swarmgame
{
namespace sim
{
void Model::init()
{
    resetScenario();

    mFutureRun = std::async(std::launch::async, &Model::run, this);
    mFutureRunPublisher = std::async(std::launch::async, &Model::runPublisher, this);
    mFutureRunSubscriber = std::async(std::launch::async, &Model::runSubscriber, this);
}

void Model::run()
{
    int64_t prevTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

    while (mRunning)
    {
        int64_t currTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

        int64_t dtMillis = currTimeMs - prevTimeMs;
        update(dtMillis);
        prevTimeMs = currTimeMs;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Model::update(const int64_t dtMillis)
{
    std::lock_guard<std::mutex> guard(mUavsMutex);
    
    const float dt = dtMillis / 1000.0f;
    for (auto& [id, uav] : mUavs)
    {
        uav.mX += uav.mVx * dt;
        uav.mY += uav.mVy * dt;
        uav.mZ += uav.mVz * dt;
    }
}

void Model::runSubscriber()
{
    zmq::socket_t subscriber(*mCtx, zmq::socket_type::sub);

    subscriber.bind("tcp://localhost:5556");
    subscriber.set(zmq::sockopt::subscribe, topic::UavCommand);

    subscriber.bind("inproc://" + topic::Reset);
    subscriber.set(zmq::sockopt::subscribe, topic::Reset);

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

void Model::runPublisher()
{
    zmq::socket_t publisher(*mCtx, zmq::socket_type::pub);
    publisher.bind("tcp://localhost:5555");
    publisher.connect("inproc://" + topic::Renderables);

    // Give the subscribers a chance to connect, so they don't lose any messages
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    while (mRunning)
    {

        std::map<vis::RenderableType, std::vector<nlohmann::json>> renderablesSerialised;

        // Send UAV states to controllers
        {
            std::lock_guard<std::mutex> guard(mUavsMutex);
            for (const auto& [id, uav] : mUavs)
            {
                publisher.send(zmq::buffer(topic::UavState), zmq::send_flags::sndmore);
                publisher.send(zmq::buffer(uav.toJson().dump()));

                renderablesSerialised[vis::RenderableType::UAV].emplace_back(uav);
            }
        }

        // Send to visualiser for rendering
        publisher.send(zmq::buffer(topic::Renderables), zmq::send_flags::sndmore);
        publisher.send(zmq::buffer(nlohmann::json(renderablesSerialised).dump()));
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // should be faster than visualiser target FPS
    }
}

void Model::processMessage(const std::string& topic, const std::string& message)
{
    if (topic == topic::UavCommand)
    {
        nlohmann::json cmd = nlohmann::json::parse(message);
        int32_t id = static_cast<int32_t>(cmd.at("id"));
        float vx = static_cast<float>(cmd.at("vx"));
        float vy = static_cast<float>(cmd.at("vy"));
        float vz = static_cast<float>(cmd.at("vz"));

        std::lock_guard<std::mutex> guard(mUavsMutex);

        // Assume perfect velocity command tracking
        // Update UAV velocity directly
        if (mUavs.contains(id))
        {
            mUavs.at(id).mVx = vx;
            mUavs.at(id).mVy = vy;
            mUavs.at(id).mVz = vz;
        }        
    }
    else if (topic == topic::Reset)
    {
        resetScenario();
    }
    else
    {
        std::cout << "Invalid topic received" << std::endl;
    }
}

void Model::resetScenario()
{
    std::lock_guard<std::mutex> guard(mUavsMutex);

    // Clear all existing UAVs
    mUavs.clear();

    // Add initial UAVs
    for (const common::Uav& uav : mInitialScenario.mInitialUavs)
    {
        mUavs[uav.mId] = uav;
    }
}
}
}