#ifndef HUE_CONTROLLER_H
#define HUE_CONTROLLER_H

#include <huestream/HueStream.h>
#include <huestream/config/Config.h>
#include <memory>

using namespace huestream;

class HueController {
  public:
    HueController();
    ~HueController();

    bool initialize();
    void runFadeEffect();
    void shutdown();

  private:
    void connectToBridge();
    void setupFeedbackCallback();

    std::shared_ptr<HueStream> m_huestream;
    std::shared_ptr<Config> m_config;
};

#endif // HUE_CONTROLLER_H