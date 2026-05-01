#ifndef BEZIER_FADE_EFFECT_H
#define BEZIER_FADE_EFFECT_H

#include "IEffect.h"
#include <huestream/HueStream.h>
#include <huestream/common/data/Color.h>
#include <huestream/effect/effects/ManualEffect.h>
#include <memory>

class BezierFadeEffect : public IEffect {
  private:
    std::shared_ptr<huestream::HueStream> _huestream;
    std::shared_ptr<huestream::ManualEffect> _currentEffect;

  public:
    BezierFadeEffect(std::shared_ptr<huestream::HueStream> huestream);
    ~BezierFadeEffect();

    void play(std::function<bool()> shouldShutdown);
    void stop();
};

#endif // BEZIER_FADE_EFFECT_H
