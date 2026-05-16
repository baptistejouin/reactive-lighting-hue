#include "SignalHandler.h"

std::atomic<bool> SignalHandler::_shutdownRequested(false);

void SignalHandler::setup() {
    struct sigaction sa;
    sa.sa_handler = _signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // No SA_RESTART: let blocking cin calls be interrupted
    sigaction(SIGINT, &sa, nullptr);
}

bool SignalHandler::isShutdownRequested() { return _shutdownRequested.load(); }

void SignalHandler::requestShutdown() { _shutdownRequested = true; }

void SignalHandler::_signalHandler(int /*signum*/) {
    _shutdownRequested = true;
}
