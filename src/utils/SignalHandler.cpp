#include "SignalHandler.h"

std::atomic<bool> SignalHandler::s_shutdownRequested(false);

void SignalHandler::setup() {
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // No SA_RESTART: let blocking cin calls be interrupted
    sigaction(SIGINT, &sa, nullptr);
}

bool SignalHandler::isShutdownRequested() { return s_shutdownRequested.load(); }

void SignalHandler::requestShutdown() { s_shutdownRequested = true; }

void SignalHandler::signalHandler(int /*signum*/) {
    s_shutdownRequested = true;
}