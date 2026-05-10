#include "Scene.h"
#include "huestream/HueStream.h"
#include "huestream/effect/effects/ManualEffect.h"
#include "huestream/effect/effects/base/Effect.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

const int REFRESH_RATE_MS = 20;

Scene::Scene(std::shared_ptr<huestream::HueStream> huestream, std::string name,
             int priority)
    : _huestream(huestream), _name(name), _priority(priority) {

    auto effect = std::make_shared<huestream::ManualEffect>(name, priority);
    _currentStreamEffect = effect;

    _huestream->LockMixer();
    _huestream->AddEffect(effect);
    _currentStreamEffect->Enable();
    _huestream->UnlockMixer();
}

Scene::~Scene() {
    _huestream->LockMixer();
    _currentStreamEffect->Disable();
    _currentStreamEffect->Finish();
    _huestream->UnlockMixer();
}

// target specific lights
void Scene::addBinding(std::vector<std::string> lightIds,
                       std::shared_ptr<IEffect> effect, int durationMs,
                       bool pingPong) {

    EffectBinding binding;
    binding.effect = effect;
    binding.durationMs = durationMs;
    binding.pingPong = pingPong;

    if (lightIds.empty()) {
        std::cerr << "lightIds param is empty" << std::endl;
        return;
    }

    binding.lightIds = lightIds;

    _bindings.push_back(binding);
}

// target all lights in a group
void Scene::addBinding(std::string groupId, std::shared_ptr<IEffect> effect,
                       int durationMs, bool pingPong) {

    if (groupId.empty()) {
        std::cerr << "groupId param is empty ";
    }

    auto group = _huestream->GetLoadedBridge()->GetGroupById(groupId);
    auto lights = group->GetLights();

    EffectBinding binding;
    binding.effect = effect;
    binding.durationMs = durationMs;
    binding.pingPong = pingPong;

    if (!lights || lights->empty()) {
        std::cerr << "The lights is empty or not valid" << std::endl;
        return;
    }

    for (auto light : *lights) {
        binding.lightIds.push_back(light->GetId());
    }

    _bindings.push_back(binding);
}

void Scene::run(std::function<bool()> shouldShutdown) {
    std::vector<BindingState> states(_bindings.size());

    while (!shouldShutdown()) {
        _huestream->LockMixer();

        for (size_t i = 0; i < _bindings.size(); i++) {
            BindingState &state = states[i];
            EffectBinding &binding = _bindings[i];

            state.elapsedMs += REFRESH_RATE_MS;

            float t = state.elapsedMs / binding.durationMs;

            if (t > 1.0) {
                if (binding.pingPong) {
                    state.reverse = !state.reverse;
                }
                state.elapsedMs = 0;
                t = 0;
            }

            if (state.reverse) {
                t = 1.0 - t;
            }

            for (auto lightId : binding.lightIds) {
                _currentStreamEffect->SetIdToColor(
                    lightId,
                    binding.effect->compute(t)); // TODO: compute "t" based on
                                                 // time and effect duration
            }
        }

        _huestream->UnlockMixer();

        std::this_thread::sleep_for(std::chrono::milliseconds(REFRESH_RATE_MS));
    }
}
