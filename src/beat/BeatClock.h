#pragma once

class BeatClock {
  public:
    BeatClock(float bpm = 120.0f);

    void update(float deltaMs);
    float currentPhase() const;  // [0, 1) — fmod of absolutePhase
    float absolutePhase() const; // raw accumulated beats
    void setBPM(float bpm);
    float getBPM() const;

  private:
    float _bpm;
    float _absolutePhase; // accumulates in beats
};
