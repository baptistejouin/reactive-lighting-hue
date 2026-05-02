#include "Scene.h"
#include "huestream/HueStream.h"
#include "huestream/effect/effects/ManualEffect.h"
#include "huestream/effect/effects/base/Effect.h"
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>

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

void Scene::addBinding(AddEffectParams effectParams) {

    EffectBinding binding;
    binding.effect = effectParams.effect;
    binding.durationMs = effectParams.durationMs;
    binding.pingPong = effectParams.pingPong;

    if (effectParams.lightIds.empty()) {
        // if no light id is specified, use all the lights in the group
        for (const auto &groupId : effectParams.groupIds) {
            auto group = _huestream->GetLoadedBridge()->GetGroupById(groupId);
            auto lights = group->GetLights();
            if (!lights || lights->empty()) {
                std::cerr << "The lights is empty or not valid" << std::endl;
                continue;
            }
            for (auto light : *lights) {
                binding.lightIds.push_back(light->GetId());
            }
        }
    } else {
        binding.lightIds = effectParams.lightIds;
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
