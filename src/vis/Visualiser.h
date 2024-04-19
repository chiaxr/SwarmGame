#pragma once

#include "common/Module.h"
#include "common/Renderable.h"
#include "common/Scenario.h"
#include "vis/CameraController.h"
#include "vis/VisualisationParams.h"

#include <memory>
#include <mutex>

namespace zmq
{
class context_t;
}

namespace swarmgame
{
namespace vis
{
class Visualiser : public common::Module
{
public:
    Visualiser(const VisualisationParams& params, const common::Scenario& scenario, zmq::context_t* ctx)
        : common::Module(ctx), mParams(params)
    {}

    void init() override;

private:
    void render();
    void renderModelRenderables();

    void handleInput();
    void processMessage(const std::string& topic, const std::string& message);
    void sendResetSignal();
    void sendShutdownSignal();

    void run() override;
    void runSubscriber() override;

    CameraController mCameraController;
    VisualisationParams mParams;

    std::mutex mModelRenderablesMutex;
    std::vector<std::shared_ptr<Renderable>> mModelRenderables;
};
}
}