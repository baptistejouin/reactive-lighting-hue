#pragma once

#include "beat/BeatClock.h"
#include "hue/Scene.h"
#include <functional>
#include <memory>
#include <vector>

class Runner {
  public:
    Runner(std::shared_ptr<BeatClock> clock);

    void addScene(std::shared_ptr<Scene> scene);
    void run(std::function<bool()> shouldShutdown);

  private:
    std::shared_ptr<BeatClock> _clock;
    std::vector<std::shared_ptr<Scene>> _scenes;
};
