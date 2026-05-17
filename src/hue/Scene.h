#pragma once

#include <effects/IEffect.h>
#include <huestream/HueStream.h>
#include <huestream/effect/effects/ManualEffect.h>
#include <memory>
#include <string>
#include <vector>

struct EffectBinding {
    std::vector<std::string> lightIds;
    std::shared_ptr<IEffect> effect;
    float phaseOffset = 0.0f;
    float phaseEnd = 0.0f;    // explicit end — avoids float accumulation error
    float phaseWindow = 0.0f; // >0 = windowed one-shot mode, 0 = continuous fmod mode
    float phaseMultiplier = 1.0f;
    bool pingPong = false;
};

class Scene {
  public:
    Scene(std::shared_ptr<huestream::HueStream> huestream, std::string name,
          int priority = 0);
    ~Scene();

    // target specific lights
    void addBinding(std::vector<std::string> lightIds,
                    std::shared_ptr<IEffect> effect, float phaseOffset = 0.0,
                    float phaseMultiplier = 1.0, bool pingPong = false);

    // target all lights in a group
    void addBinding(std::string groupId, std::shared_ptr<IEffect> effect,
                    float phaseOffset = 0.0, float phaseMultiplier = 1.0,
                    bool pingPong = false);

    void withDistribution(std::vector<std::string> orderedLightIds,
                          std::shared_ptr<IEffect> effect,
                          float cycleBeats = 1.0f, float activeFraction = 1.0f,
                          float overlap = 0.0f);

    void tick(float phase);

  private:
    std::vector<EffectBinding> _bindings;
    std::shared_ptr<huestream::HueStream> _huestream;
    std::shared_ptr<huestream::ManualEffect> _currentStreamEffect;
    std::string _name;
    int _priority = 0;
};
