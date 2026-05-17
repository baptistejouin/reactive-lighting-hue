#include <beat/BeatClock.h>
#include <csignal>
#include <effects/BezierFadeEffect.h>
#include <hue/HueController.h>
#include <hue/Scene.h>
#include <iostream>
#include <memory>
#include <runner/Runner.h>
#include <set>
#include <utils/SignalHandler.h>
// #include "ndi/NDIController.h"

int main(int argc, char *argv[]) {
    // Register signal handler for Ctrl+C
    SignalHandler::setup();
    HueController hueController;
    bool initialized = hueController.initialize();

    auto huestream = hueController.getHueStream();
    if (!initialized) {
        std::cerr << "Something goes wrong with the hue initialization!"
                  << std::endl;
        return -1;
    }

    auto scene = std::make_shared<Scene>(huestream, "Main Scene", 0);
    auto clock = std::make_shared<BeatClock>(120.0f); // bpm
    auto mainGroup = huestream->GetLoadedBridge()->GetGroups()->at(0);
    auto lights = mainGroup->GetLights();
    Runner runner(clock);

    auto bezierEffect = std::make_shared<BezierFadeEffect>();

    if (!lights || lights->empty()) {
        std::cerr << "The lights is empty or "
                     "not valid"
                  << std::endl;
        return -1;
    }

    std::vector<std::string> lightIds;
    const int nbLight = 2;
    // std::set<int> usedIndices;

    // std::srand(static_cast<unsigned int>(std::time(nullptr)));
    // while (lightIds.size() < nbLight && usedIndices.size() < lights->size())
    // {
    //     int randomIndex = std::rand() % lights->size();
    //     if (usedIndices.insert(randomIndex).second) {
    //         lightIds.push_back(lights->at(randomIndex)->GetId());
    //     }
    // }

    for (int i = 0; i < nbLight; i++) {
        lightIds.push_back(lights->at(i)->GetId());
    }
    scene->withDistribution(lightIds, bezierEffect,
                            static_cast<float>(nbLight) / 2, 1.0f, 2.0f);

    runner.addScene(scene);

    // internal while loop
    runner.run([]() { return SignalHandler::isShutdownRequested(); });

    return 0;
}
