#ifndef IEFFECT_H
#define IEFFECT_H
#include <huestream/common/data/Color.h>

class IEffect {
  public:
    virtual ~IEffect() = default;
    virtual huestream::Color compute(float t) const = 0; // t in [0,1]
};

#endif // IEFFECT_H
