#include <cmath>
#include <hue/Scene.h>
#include <huestream/HueStream.h>
#include <huestream/effect/effects/ManualEffect.h>
#include <huestream/effect/effects/base/Effect.h>
#include <iostream>
#include <memory>

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
                       std::shared_ptr<IEffect> effect, float phaseOffset,
                       float phaseMultiplier, bool pingPong) {

    if (lightIds.empty()) {
        std::cerr << "lightIds param is empty" << std::endl;
        return;
    }

    EffectBinding binding;
    binding.effect = effect;
    binding.pingPong = pingPong;
    binding.phaseMultiplier = phaseMultiplier;
    binding.phaseOffset = phaseOffset;
    binding.lightIds = lightIds;
    _bindings.push_back(binding);
}

// target all lights in a group
void Scene::addBinding(std::string groupId, std::shared_ptr<IEffect> effect,
                       float phaseOffset, float phaseMultiplier,
                       bool pingPong) {

    if (groupId.empty()) {
        std::cerr << "groupId cannot be empty";
        return;
    }

    auto group = _huestream->GetLoadedBridge()->GetGroupById(groupId);
    auto lights = group->GetLights();

    EffectBinding binding;
    binding.effect = effect;
    binding.pingPong = pingPong;
    binding.phaseOffset = phaseOffset;
    binding.phaseMultiplier = phaseMultiplier;

    if (!lights || lights->empty()) {
        std::cerr << "lights cannot be empty" << std::endl;
        return;
    }

    for (auto light : *lights) {
        binding.lightIds.push_back(light->GetId());
    }

    _bindings.push_back(binding);
}

void Scene::tick(float phase) {
    _huestream->LockMixer();

    for (auto &binding : _bindings) {
        float t;

        if (binding.phaseWindow > 0.0f) {
            float patternPhase = fmod(phase * binding.phaseMultiplier, 1.0f);
            if (binding.phaseEnd <= 1.0f) {
                if (patternPhase < binding.phaseOffset ||
                    patternPhase >= binding.phaseEnd) {
                    t = 1.0f;
                } else {
                    t = (patternPhase - binding.phaseOffset) /
                        binding.phaseWindow;
                }
            } else {
                // window wraps around cycle boundary
                if (patternPhase >= binding.phaseOffset) {
                    t = (patternPhase - binding.phaseOffset) /
                        binding.phaseWindow;
                } else if (patternPhase < binding.phaseEnd - 1.0f) {
                    t = (patternPhase + 1.0f - binding.phaseOffset) /
                        binding.phaseWindow;
                } else {
                    t = 1.0f;
                }
            }
        } else {
            t = fmod(phase * binding.phaseMultiplier + binding.phaseOffset,
                     1.0f);
            if (binding.pingPong) {
                t = t < 0.5f ? t * 2.0f : (1.0f - t) * 2.0f;
            }
        }

        for (auto lightId : binding.lightIds) {
            _currentStreamEffect->SetIdToColor(lightId,
                                               binding.effect->compute(t));
        }
    }

    _huestream->UnlockMixer();
}

void Scene::withDistribution(std::vector<std::string> orderedLightIds,
                             std::shared_ptr<IEffect> effect, float cycleBeats,
                             float activeFraction, float overlap) {
    const int N = orderedLightIds.size();

    if (N <= 0) {
        std::cerr << "lightIds cannot be empty" << std::endl;
        return;
    }

    if (cycleBeats <= 0) {
        std::cerr << "cycleBeats cannot be <= 0" << std::endl;
        return;
    }

    if (activeFraction <= 0.0f || activeFraction > 1.0f) {
        std::cerr << "activeFraction must be in (0, 1]" << std::endl;
        return;
    }

    float phaseMultiplier = 1.0f / cycleBeats;
    float stagger = activeFraction / N;

    for (int i = 0; i < N; i++) {
        EffectBinding binding;
        binding.effect = effect;
        binding.phaseMultiplier = phaseMultiplier;
        binding.phaseOffset = stagger * static_cast<float>(i);
        binding.phaseWindow = stagger * (1.0f + overlap);
        binding.phaseEnd = binding.phaseOffset + binding.phaseWindow;
        binding.lightIds = {orderedLightIds.at(i)};
        _bindings.push_back(binding);
    }
}
