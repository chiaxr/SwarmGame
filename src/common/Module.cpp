#include "common/Module.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include <iostream>

namespace swarmgame
{
namespace common
{
Module::Module(zmq::context_t* ctx)
    : mCtx(ctx)
{
    mRunning = true;
    mFutureRunShutdownSubscriber = std::async(std::launch::async, &Module::runShutdownSubscriber, this);
}

Module::~Module()
{
    stop();
}

void Module::stop()
{
    mRunning = false;
}

bool Module::isRunning()
{
    return mRunning;
}

void Module::runShutdownSubscriber()
{
    zmq::socket_t subscriber(*mCtx, zmq::socket_type::sub);
    subscriber.connect("inproc://" + topic::Shutdown);

    subscriber.set(zmq::sockopt::subscribe, topic::Shutdown);

    while (mRunning)
    {
        std::vector<zmq::message_t> received;
        zmq::recv_result_t result = zmq::recv_multipart(subscriber, std::back_inserter(received)); // blocks until message received
        
        if (result && *result == 2)
        {
            std::cout << "Shutting down..." << std::endl;
            mRunning = false;
        }
    }
}
}
}