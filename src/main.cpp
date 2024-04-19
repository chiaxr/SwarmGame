#include "common/FileUtils.h"
#include "common/JsonWrapper.h"
#include "common/Scenario.h"
#include "sim/Model.h"
#include "vis/VisualisationParams.h"
#include "vis/Visualiser.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include <chrono>
#include <iostream>
#include <thread>

using namespace swarmgame;

int main(void)
{
    zmq::context_t ctx(1);

    nlohmann::json paramsJson = util::parseJsonFile("config/config.json");
    
    std::string scenarioFile = paramsJson.at("common").at("scenarioConfigFile");
    nlohmann::json scenarioJson = util::parseJsonFile("config/" + scenarioFile);
    common::Scenario scenario(scenarioJson);

    nlohmann::json simParamsJson = util::mergeJson(paramsJson.at("common"), paramsJson.at("sim"));
    sim::SimulationParams simParams(simParamsJson);
    sim::Model model(simParams, scenario, &ctx);
    model.init();

    nlohmann::json visParamsJson = util::mergeJson(paramsJson.at("common"), paramsJson.at("vis"));
    vis::VisualisationParams visParams(visParamsJson);
    vis::Visualiser visualiser(visParams, scenario, &ctx);
    visualiser.init();

    bool running = true;
    while (running)
    {
        running = model.isRunning() || visualiser.isRunning();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}