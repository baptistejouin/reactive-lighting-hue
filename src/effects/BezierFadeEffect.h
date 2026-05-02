#ifndef BEZIER_FADE_EFFECT_H
#define BEZIER_FADE_EFFECT_H

#include "IEffect.h"
#include <huestream/HueStream.h>
#include <huestream/common/data/Color.h>
#include <huestream/effect/effects/ManualEffect.h>

class BezierFadeEffect : public IEffect {
  public:
    huestream::Color compute(float t) const; // t in [0,1]
};

#endif // BEZIER_FADE_EFFECT_H
