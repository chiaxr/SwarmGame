#pragma once

#include "common/MessageTopics.h"

#include <atomic>
#include <future>

namespace zmq
{
class context_t;
}

namespace swarmgame
{
namespace common
{
class Module
{
public:
    Module(zmq::context_t* ctx);
    ~Module();

    virtual void init() = 0;
    virtual void stop();
    bool isRunning();

protected:
    virtual void run() = 0;
    virtual void runPublisher() {};
    virtual void runSubscriber() {};

    std::atomic_bool mRunning { false };
    zmq::context_t* mCtx;

    std::future<void> mFutureRun;
    std::future<void> mFutureRunPublisher;
    std::future<void> mFutureRunSubscriber;
    std::future<void> mFutureRunShutdownSubscriber;

private:
    void runShutdownSubscriber();
};
}
}