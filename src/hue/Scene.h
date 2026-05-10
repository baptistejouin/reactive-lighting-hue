#pragma once

#include "effects/IEffect.h"
#include "huestream/effect/effects/ManualEffect.h"
#include <functional>
#include <huestream/HueStream.h>
#include <memory>
#include <string>
#include <vector>

struct BindingState {
    float elapsedMs = 0;
    bool reverse = false;
};

struct EffectBinding {
    std::vector<std::string> lightIds;
    std::shared_ptr<IEffect> effect;
    int durationMs;
    bool pingPong; // if true, the effect will play forward and then
                   // backward in a loop
};

struct AddEffectParams;

class Scene {
  public:
    Scene(std::shared_ptr<huestream::HueStream> huestream, std::string name,
          int priority = 0);
    ~Scene();

    // target specific lights
    void addBinding(std::vector<std::string> lightIds,
                    std::shared_ptr<IEffect> effect, int durationMs = 1000,
                    bool pingPong = false);

    // target all lights in a group
    void addBinding(std::string groupId, std::shared_ptr<IEffect> effect,
                    int durationMs = 1000, bool pingPong = false);
    void run(std::function<bool()> shouldShutdown);

  private:
    std::vector<EffectBinding> _bindings;
    std::shared_ptr<huestream::HueStream> _huestream;
    std::shared_ptr<huestream::ManualEffect> _currentStreamEffect;
    std::string _name;
    int _priority = 0;
};
