#include <string>

#include <Processing.NDI.Lib.h>

class NDIController {
  public:
    NDIController();
    ~NDIController();

    bool initialize(const std::string &sourceName);
    void runDebugMode();
    void shutdown();

  private:
    bool _findSource(const std::string &sourceName);
    void _processFrame();
    void _displayFrameWithAverageColor();

    std::string _sourceName;

    NDIlib_find_instance_t _findInstance;
    NDIlib_recv_instance_t _recvInstance;
    bool _ndiInitialized;
};
