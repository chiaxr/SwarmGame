#include "Controller.h"

#include <random>

std::map<int32_t, nlohmann::json> Controller::mUavs;

void Controller::init()
{
    mFutureRun = std::async(std::launch::async, &Controller::run, this);
    mFutureRunSubscriber = std::async(std::launch::async, &Controller::runSubscriber, this);
}

void Controller::run()
{
    // Initialise socket
    zmq::socket_t publisher(*mCtx, zmq::socket_type::pub);
    publisher.connect("tcp://localhost:5556");

    // Give the subscribers a chance to connect, so they don't lose any messages
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    // Initialise random generator
    constexpr float maxAccel = 0.5f;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-maxAccel, maxAccel);

    while (true)
    {
        std::lock_guard<std::mutex> guard(mUavsMutex);
        if (!mUavs.empty())
        {
            for (auto& [id, uav] : mUavs)
            {
                // Generate random command based on current velocity
                float cmdVx = static_cast<float>(uav.at("vx")) + dis(gen);
                float cmdVy = static_cast<float>(uav.at("vy")) + dis(gen);
                float cmdVz = static_cast<float>(uav.at("vz")); // maintain current altitude

                // Command must be issued as a desired velocity
                // It consists of:
                // id, vx, vy, vz
                nlohmann::json command;
                command.emplace("id", id);
                command.emplace("vx", cmdVx);
                command.emplace("vy", cmdVy);
                command.emplace("vz", cmdVz);

                // Publish command
                publisher.send(zmq::buffer(mUavCommandTopic), zmq::send_flags::sndmore);
                publisher.send(zmq::buffer(command.dump()));
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Controller::runSubscriber()
{
    // Initialise socket
    zmq::socket_t subscriber(*mCtx, zmq::socket_type::sub);
    subscriber.connect("tcp://localhost:5555");

    // Subscribe to topics
    subscriber.set(zmq::sockopt::subscribe, mUavStateTopic);

    while (true)
    {
        std::vector<zmq::message_t> received;

        // Blocks until message received
        zmq::recv_result_t result = zmq::recv_multipart(subscriber, std::back_inserter(received));
        
        // Check for valid message
        if (result && result == 2)
        {
            // Parse message
            nlohmann::json uavState = nlohmann::json::parse(received.at(1).to_string());
            int32_t id = static_cast<int32_t>(uavState.at("id"));

            // Add state to map
            std::lock_guard<std::mutex> guard(mUavsMutex);
            mUavs[id] = uavState;
        }
    }
}
