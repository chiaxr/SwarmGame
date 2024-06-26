#include "sim/Model.h"

#include "sim/Lidar2d.h"
#include "sim/SensorData.h"
#include "common/MessageTopics.h"
#include "vis/VisPrimitives.h"

#include "raymath.h"
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

    mEnvTriangles.clear();
    for (const auto& obstacle : mInitialScenario.mObstacles)
    {
        mEnvTriangles.insert(mEnvTriangles.end(), obstacle.mTriangles.begin(), obstacle.mTriangles.end());
    }

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
    const std::vector<common::Uav> snapshot = getUavsSnapshot();

    const float dt = std::max(dtMillis / 1000.0f, EPSILON); // avoid dt == 0
    for (auto& [id, uav] : mUavs)
    {
        const Vector3 pos = uav.getPos();
        const Vector3 vel = uav.getVel();

        // Limit velocity command based on max speed   
        Vector3 velCmd = mVelCmds.contains(id) ? mVelCmds.at(id) : vel; // maintain current velocity if no command
        const float cmdSpeed = Vector3Length(velCmd);
        if (cmdSpeed > mParams.mUavMaxSpeed)
        {
            velCmd = Vector3Scale(velCmd, mParams.mUavMaxSpeed / cmdSpeed);
        }

        // Compute next position, assuming perfect velocity tracking
        const Vector3 nextPos = Vector3Add(pos, Vector3Scale(velCmd, dt));

        // Next position results in collision, remain in current position
        if (hasCollisionWithUavs(nextPos, id, uav.mRadius, snapshot) ||
            hasCollisionWithObstacles(nextPos, mInitialScenario.mObstacles))
        {
            continue;
        }

        // Next position is safe, update position
        uav.mX = nextPos.x;
        uav.mY = nextPos.y;
        uav.mZ = nextPos.z;

        // Compute own velocity based on actual distance moved
        uav.mVx = (nextPos.x - pos.x) / dt;
        uav.mVy = (nextPos.y - pos.y) / dt;
        uav.mVz = (nextPos.z - pos.z) / dt;
    }
}

bool Model::hasCollisionWithUavs(const Vector3& pos, const int32_t id, const float radius, const std::vector<common::Uav> uavs)
{
    bool collision = false;

    for (const auto& uav : uavs)
    {
        if (id == uav.mId)
        {
            continue;
        }

        if (Vector3Distance(pos, uav.getPos()) < radius + uav.mRadius)
        {
            collision = true;
            break;
        }
    }

    return collision;
}

bool Model::hasCollisionWithObstacles(const Vector3& pos, const std::vector<common::Obstacle>& obstacles)
{
    bool collision = false;

    for (const auto& obstacle : obstacles)
    {
        if (common::pointInExtrudedPolygon(obstacle.mPolygon, obstacle.mMinHeight, obstacle.mMaxHeight, pos))
        {
            collision = true;
            break;
        }
    }

    return collision;
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

        // Send UAV states and sensor data to controllers
        {
            std::lock_guard<std::mutex> guard(mUavsMutex);
            const std::vector<common::Uav> snapshot = getUavsSnapshot();

            for (const auto& [id, uav] : mUavs)
            {
                Vector3 pos = uav.getPos();
                
                // Simulate sensors
                SensorData sensorData;
                sensorData.id = uav.mId;
                sensorData.pointcloud = Lidar2d::simulate(
                    id,
                    pos,
                    snapshot,
                    mEnvTriangles,
                    mParams.mAngleStep,
                    mParams.mMaxRange
                );

                publisher.send(zmq::buffer(topic::UavState), zmq::send_flags::sndmore);
                publisher.send(zmq::buffer(uav.toJson().dump()));

                publisher.send(zmq::buffer(topic::SensorData), zmq::send_flags::sndmore);
                publisher.send(zmq::buffer(nlohmann::json(sensorData).dump()));

                renderablesSerialised[vis::RenderableType::UAV].emplace_back(uav);
                for (const Vector3& point : sensorData.pointcloud)
                {
                    vis::Line line(pos, Vector3Add(pos, point), RED);
                    renderablesSerialised[vis::RenderableType::LINE].emplace_back(line);
                }
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

        // Only accept command if UAV exists
        if (mUavs.contains(id))
        {
            mVelCmds[id] = {vx, vy, vz};
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
    for (common::Uav& uav : mInitialScenario.mInitialUavs)
    {
        uav.mRadius = mParams.mUavRadius;
        mUavs[uav.mId] = uav;
    }
}

std::vector<common::Uav> Model::getUavsSnapshot()
{
    std::vector<common::Uav> snapshot;
    snapshot.reserve(mUavs.size());
    for (const auto& [id, uav] : mUavs)
    {
        snapshot.emplace_back(uav);
    }
    return snapshot;
}
}
}