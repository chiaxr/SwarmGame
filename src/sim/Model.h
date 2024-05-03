#pragma once

#include "common/Module.h"

#include "common/Scenario.h"
#include "common/Uav.h"
#include "sim/SimulationParams.h"

#include <mutex>
#include <map>

namespace zmq
{
class context_t;
}

namespace swarmgame
{
namespace sim
{
class Model : public common::Module
{
public:
    Model(const SimulationParams& params, const common::Scenario& scenario, zmq::context_t* ctx)
        : common::Module(ctx), mParams(params), mInitialScenario(scenario)
    {}

    void init() override;

private:
    void run() override;
    void update(const int64_t dtMillis);
    bool hasCollisionWithUavs(const Vector3& pos, const int32_t id, const float radius, const std::vector<common::Uav> uavs);
    bool hasCollisionWithObstacles(const Vector3& pos, const std::vector<common::Obstacle>& obstacles);

    void runSubscriber() override;
    void runPublisher() override;
    void processMessage(const std::string& topic, const std::string& message);
    void resetScenario();
    std::vector<common::Uav> getUavsSnapshot();

    SimulationParams mParams;
    common::Scenario mInitialScenario;
    std::vector<common::Triangle<Vector3>> mEnvTriangles;

    std::mutex mUavsMutex;
    std::map<int32_t, common::Uav> mUavs;
    std::map<int32_t, Vector3> mVelCmds;
};
}
}