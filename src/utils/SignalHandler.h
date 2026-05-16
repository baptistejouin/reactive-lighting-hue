#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <atomic>
#include <csignal>
#include <sys/signal.h>

class SignalHandler {
  public:
    static void setup();
    static bool isShutdownRequested();
    static void requestShutdown();

  private:
    static std::atomic<bool> _shutdownRequested;
    static void _signalHandler(int signum);
};

#endif // SIGNAL_HANDLER_H
