#include "SignalHandler.h"
#include <iostream>

std::atomic<bool> SignalHandler::s_shutdownRequested(false);

void SignalHandler::setup() { std::signal(SIGINT, signalHandler); }

bool SignalHandler::isShutdownRequested() { return s_shutdownRequested.load(); }

void SignalHandler::requestShutdown() { s_shutdownRequested = true; }

void SignalHandler::signalHandler(int signum) {
    std::cout << "\n\nCtrl+C detected! Initiating graceful shutdown..."
              << std::endl;
    s_shutdownRequested = true;
}