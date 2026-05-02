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

struct AddEffectParams {
    std::vector<std::string>
        lightIds; // TODO: make this optional and use all the lights in the
                  // group if not specified
    std::vector<std::string> groupIds;
    std::shared_ptr<IEffect> effect;
    int durationMs = 1000;
    bool pingPong = false;
};

class Scene {
  public:
    Scene(std::shared_ptr<huestream::HueStream> huestream, std::string name,
          int priority = 0);
    ~Scene();

    void addBinding(AddEffectParams);
    void run(std::function<bool()> shouldShutdown);

  private:
    std::vector<EffectBinding> _bindings;
    std::shared_ptr<huestream::HueStream> _huestream;
    std::shared_ptr<huestream::ManualEffect> _currentStreamEffect;
    std::string _name;
    int _priority = 0;
};
