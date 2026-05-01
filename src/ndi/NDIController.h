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
    bool findSource(const std::string &sourceName);
    void processFrame();
    void displayFrameWithAverageColor();

    std::string m_sourceName;

    NDIlib_find_instance_t m_findInstance;
    NDIlib_recv_instance_t m_recvInstance;
    bool m_ndiInitialized;
};
