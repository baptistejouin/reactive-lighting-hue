#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <atomic>
#include <csignal>

class SignalHandler {
  public:
    static void setup();
    static bool isShutdownRequested();
    static void requestShutdown();

  private:
    static std::atomic<bool> s_shutdownRequested;
    static void signalHandler(int signum);
};

#endif // SIGNAL_HANDLER_H