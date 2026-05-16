#include "beat/BeatClock.h"
#include <cmath>

BeatClock::BeatClock(float bpm) : _bpm(bpm), _absolutePhase(0.0f) {}

void BeatClock::update(float deltaMs) {
    _absolutePhase +=
        deltaMs * _bpm / 60000.0f; // 60 * 1000 -> bpm -> beat per ms
}

float BeatClock::currentPhase() const {
    return std::fmod(_absolutePhase, 1.0f);
}

float BeatClock::absolutePhase() const { return _absolutePhase; }

void BeatClock::setBPM(float bpm) { _bpm = bpm; }

float BeatClock::getBPM() const { return _bpm; }
