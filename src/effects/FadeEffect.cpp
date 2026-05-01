#include "FadeEffect.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace huestream;

FadeEffect::FadeEffect(std::shared_ptr<HueStream> huestream)
    : _huestream(huestream), _currentEffect(nullptr) {}

FadeEffect::~FadeEffect() { stop(); }

void FadeEffect::play(std::function<bool()> shouldShutdown) {
    std::cout
        << "\n🌟 Starting fade effect (0% → 100% → 0% in 4 seconds, looping)..."
        << std::endl;
    std::cout << "   Press Ctrl+C to stop gracefully..." << std::endl;

    if (!_huestream->IsStreamableBridgeLoaded() &&
        !_huestream->IsBridgeStreaming()) {
        std::cout << "❌ Cannot play effect - no streamable bridge loaded"
                  << std::endl;
        return;
    }

    // Create a manual effect
    _currentEffect = std::make_shared<ManualEffect>("fade-effect", 0);

    // Get lights from the entertainment group
    auto group = _huestream->GetLoadedBridge()->GetGroups()->at(0);
    auto lights = group->GetLights();

    // Base color (white)
    Color baseColor(1.0, 1.0, 1.0);

    // Add the effect to the mixer
    _huestream->LockMixer();
    _huestream->AddEffect(_currentEffect);
    _currentEffect->Enable();
    _huestream->UnlockMixer();

    // Fade parameters
    const int fadeDuration_ms = 1000; // 1 second
    const int updateInterval_ms = 20; // Update every 20ms for smooth fade
    const int steps = fadeDuration_ms / updateInterval_ms; // 100 steps

    // Loop fade effect until Ctrl+C
    while (!shouldShutdown()) {
        // Fade UP from 0 to 1 over 2 seconds
        for (int step = 0; step <= steps && !shouldShutdown(); step++) {
            double intensity = static_cast<double>(step) / steps;

            // Apply intensity to the color
            Color fadedColor(baseColor.GetR() * intensity,
                             baseColor.GetG() * intensity,
                             baseColor.GetB() * intensity);

            // Update all lights
            _huestream->LockMixer();
            for (auto light : *lights) {
                _currentEffect->SetIdToColor(light->GetId(), fadedColor);
            }
            _huestream->UnlockMixer();

            // Wait before next update
            std::this_thread::sleep_for(
                std::chrono::milliseconds(updateInterval_ms));
        }

        // Fade DOWN from 1 to 0 over 2 seconds
        for (int step = steps; step >= 0 && !shouldShutdown(); step--) {
            double intensity = static_cast<double>(step) / steps;

            // Apply intensity to the color
            Color fadedColor(baseColor.GetR() * intensity,
                             baseColor.GetG() * intensity,
                             baseColor.GetB() * intensity);

            // Update all lights
            _huestream->LockMixer();
            for (auto light : *lights) {
                _currentEffect->SetIdToColor(light->GetId(), fadedColor);
            }
            _huestream->UnlockMixer();

            // Wait before next update
            std::this_thread::sleep_for(
                std::chrono::milliseconds(updateInterval_ms));
        }
    }

    std::cout << "\n⏸️  Stopping fade effect..." << std::endl;
    stop();
    std::cout << "✅ Fade effect stopped!" << std::endl;
}

void FadeEffect::stop() {
    if (_currentEffect && _huestream) {
        _huestream->LockMixer();
        _currentEffect->Disable();
        _currentEffect->Finish();
        _huestream->UnlockMixer();
        _currentEffect = nullptr;
    }
}
