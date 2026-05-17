#include "runner/Runner.h"
#include <chrono>
#include <thread>

const int REFRESH_RATE_MS = 20;

Runner::Runner(std::shared_ptr<BeatClock> clock) : _clock(clock) {}

void Runner::addScene(std::shared_ptr<Scene> scene) {
    _scenes.push_back(scene);
}

void Runner::run(std::function<bool()> shouldShutdown) {
    while (!shouldShutdown()) {
        _clock->update(REFRESH_RATE_MS);
        for (auto &scene : _scenes) {
            scene->tick(_clock->absolutePhase());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(REFRESH_RATE_MS));
    }
}
