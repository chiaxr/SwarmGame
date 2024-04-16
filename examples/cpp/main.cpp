#include "Controller.h"

#include "nlohmann/json.hpp"
#include "zmq.hpp"
#include "zmq_addon.hpp"

#include <chrono>
#include <iostream>
#include <unordered_map>
#include <thread>

int main(void)
{
    zmq::context_t ctx(1);

    Controller controller(&ctx);
    controller.init();

    return 0;
}