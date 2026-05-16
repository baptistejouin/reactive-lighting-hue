#ifndef HUE_CONTROLLER_H
#define HUE_CONTROLLER_H

#include "effects/IEffect.h"
#include <huestream/HueStream.h>
#include <huestream/config/Config.h>
#include <memory>

class HueController {
  public:
    HueController();
    ~HueController();

    bool initialize();
    void runEffect(std::shared_ptr<IEffect> effect);
    void shutdown();
    std::shared_ptr<huestream::HueStream> getHueStream();

  private:
    void _connectToBridge();
    void _setupFeedbackCallback();

    std::shared_ptr<huestream::HueStream> _huestream;
    std::shared_ptr<huestream::Config> _config;
};

#endif // HUE_CONTROLLER_H
