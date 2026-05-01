#ifndef IEFFECT_H
#define IEFFECT_H

#include <functional>

class IEffect {
  public:
    virtual ~IEffect() = default;
    virtual void play(std::function<bool()> shouldShutdown) = 0;
    virtual void stop() = 0;
};

#endif // IEFFECT_H
