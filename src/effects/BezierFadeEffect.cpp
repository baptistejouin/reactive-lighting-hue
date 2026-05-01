#include "BezierFadeEffect.h"
#include <array>
#include <cmath>

using namespace huestream;

BezierFadeEffect::BezierFadeEffect(std::shared_ptr<HueStream> huestream)
    : _huestream(huestream), _currentEffect(nullptr) {}

BezierFadeEffect::~BezierFadeEffect() { stop(); }

struct BezierCurve {
    float x1;
    float y1;
    float x2;
    float y2;
};

float bezierX(float t, float x1, float x2) {
    return 3 * pow((1 - t), 2) * t * x1 + 3 * (1 - t) * pow(t, 2) * x2 +
           pow(t, 3);
}

float bezierY(float u, float y1, float y2) {
    return 3 * pow((1 - u), 2) * u * y1 + 3 * (1 - u) * pow(u, 2) * y2 +
           pow(u, 3);
}

float solveBezier(float t, BezierCurve curve) {
    // using the binary search
    int steps = 20;
    float low = 0;
    float high = 1;
    float mid = high / 2;

    for (int i = 0; i <= steps; i++) {
        mid = (low + high) / 2;
        float x = bezierX(mid, curve.x1, curve.x2);
        if (x < t) {
            low = mid;
        } else {
            high = mid;
        }
    }

    float intensity = bezierY(mid, curve.y1, curve.y2);
    return intensity;
}

void BezierFadeEffect::play(std::function<bool()> shouldShutdown) {
    // NOTE: https://easings.net/#
    BezierCurve easeOut;
    easeOut.x1 = 0.83;
    easeOut.y1 = 0.00;
    easeOut.x2 = 0.17;
    easeOut.y2 = 1.00;

    std::array<BezierCurve, 1> bezierCurveRegistery = {easeOut};

    _currentEffect = std::make_shared<ManualEffect>("fade-effect", 0);
    auto group = _huestream->GetLoadedBridge()->GetGroups()->at(0);
    auto lights = group->GetLights();

    Color baseColor(1.0, 1.0, 1.0);

    _huestream->LockMixer();
    _huestream->AddEffect(_currentEffect);
    _currentEffect->Enable();
    _huestream->UnlockMixer();

    bool reverseLoop = false;
    const int effectDuration_ms = 8000; // 5s
    const int updateInterval_ms = 20;
    const int steps = effectDuration_ms / updateInterval_ms;

    while (!shouldShutdown()) {
        for (int step = 0; step <= steps && !shouldShutdown(); step++) {
            float t = static_cast<double>(step) / steps;
            float effectiveT = reverseLoop ? 1.0f - t : t;
            double intensity = solveBezier(effectiveT, bezierCurveRegistery[0]);

            Color currentColor(baseColor.GetR() * intensity,
                               baseColor.GetG() * intensity,
                               baseColor.GetB() * intensity);

            // Update all lights
            _huestream->LockMixer();
            for (auto light : *lights) {
                _currentEffect->SetIdToColor(light->GetId(), currentColor);
            }
            _huestream->UnlockMixer();

            // Wait before next update
            std::this_thread::sleep_for(
                std::chrono::milliseconds(updateInterval_ms));
        }
        // reverse the loop for the next iteration
        reverseLoop = !reverseLoop;
    }
    stop();
}

void BezierFadeEffect::stop() {
    if (_currentEffect && _huestream) {
        _huestream->LockMixer();
        _currentEffect->Disable();
        _currentEffect->Finish();
        _huestream->UnlockMixer();
        _currentEffect = nullptr;
    }
}
