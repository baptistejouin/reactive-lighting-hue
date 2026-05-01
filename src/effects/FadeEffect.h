#ifndef FADE_EFFECT_H
#define FADE_EFFECT_H

#include "IEffect.h"
#include <huestream/HueStream.h>
#include <huestream/common/data/Color.h>
#include <huestream/effect/effects/ManualEffect.h>
#include <memory>

class FadeEffect : public IEffect {
  private:
    std::shared_ptr<huestream::HueStream> _huestream;
    std::shared_ptr<huestream::ManualEffect> _currentEffect;

  public:
    FadeEffect(std::shared_ptr<huestream::HueStream> huestream);
    ~FadeEffect();

    void play(std::function<bool()> shouldShutdown);
    void stop();
};

#endif // FADE_EFFECT_H
