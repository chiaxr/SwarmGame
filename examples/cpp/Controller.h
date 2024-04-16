#pragma once

#include "nlohmann/json.hpp"
#include "zmq.hpp"
#include "zmq_addon.hpp"

#include <future>
#include <mutex>
#include <string>
#include <map>

class Controller
{
public:
    Controller(zmq::context_t* ctx)
        : mCtx(ctx)
    {}

    void init();

private:
    // Generates and publishes UAV commands
    void run();

    // Subscribes and updates current UAV states
    void runSubscriber();

    const std::string mUavCommandTopic = "uavcommand";
    const std::string mUavStateTopic = "uavstate";

    zmq::context_t* mCtx;

    std::future<void> mFutureRun;
    std::future<void> mFutureRunSubscriber;

    std::mutex mUavsMutex;
    // Declare as static to avoid access by pub/sub threads before construction
    static std::map<int32_t, nlohmann::json> mUavs;
};