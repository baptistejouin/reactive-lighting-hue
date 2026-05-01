#include "HueController.h"
#include "../effects/FadeEffect.h"
#include "../utils/SignalHandler.h"
#include <iostream>

HueController::HueController() {}

HueController::~HueController() { shutdown(); }

bool HueController::initialize() {
    try {
        std::cout << "Setting up HueStream library..." << std::endl;

        m_config = std::make_shared<Config>(
            "LightEffects", "MacBook", PersistenceEncryptionKey("secret_key"));

        m_huestream = std::make_shared<HueStream>(m_config);

        setupFeedbackCallback();

        std::cout << "HueStream library initialized" << std::endl;

        connectToBridge();

        return m_huestream->IsStreamableBridgeLoaded() ||
               m_huestream->IsBridgeStreaming();

    } catch (const std::exception &e) {
        std::cerr << "Failed to initialize Hue controller: " << e.what()
                  << std::endl;
        return false;
    }
}

void HueController::setupFeedbackCallback() {
    m_huestream->RegisterFeedbackCallback([](const FeedbackMessage &message) {
        std::cout << "[" << message.GetId() << "] " << message.GetTag()
                  << std::endl;

        if (message.GetId() == FeedbackMessage::ID_DONE_COMPLETED) {
            std::cout << "Connected to bridge with ip: "
                      << message.GetBridge()->GetIpAddress() << std::endl;
        }

        if (message.GetMessageType() == FeedbackMessage::FEEDBACK_TYPE_USER) {
            std::cout << message.GetUserMessage() << std::endl;
        }
    });
}

void HueController::connectToBridge() {
    std::cout << "\nConnecting to bridge..." << std::endl;
    m_huestream->ConnectBridge();

    while (!m_huestream->IsStreamableBridgeLoaded() &&
           !m_huestream->IsBridgeStreaming() &&
           !SignalHandler::isShutdownRequested()) {

        auto bridge = m_huestream->GetLoadedBridge();
        std::cout << "Bridge status: " << bridge->GetStatusTag() << std::endl;

        if (bridge->GetStatus() == BRIDGE_INVALID_GROUP_SELECTED) {
            std::cout << "Selecting first entertainment group..." << std::endl;
            m_huestream->SelectGroup(bridge->GetGroups()->at(0));

        } else if (bridge->GetStatus() != BRIDGE_READY &&
                   bridge->GetStatus() != BRIDGE_STREAMING) {
            std::cout << "No streamable bridge configured: "
                      << bridge->GetStatusTag() << std::endl;
            std::cout << "Press Enter to retry..." << std::endl;
            std::cin.get();
            m_huestream->ConnectBridge();
        }
    }

    if (!SignalHandler::isShutdownRequested()) {
        std::cout << "Bridge connection completed!" << std::endl;
    }
}

void HueController::runFadeEffect() {
    if (m_huestream->IsStreamableBridgeLoaded() ||
        m_huestream->IsBridgeStreaming()) {
        std::cout << "\nStarting Fade In/Out effect..." << std::endl;
        FadeEffect fadeEffect(m_huestream);

        // Pass a lambda that checks the shutdown status
        fadeEffect.play([]() { return SignalHandler::isShutdownRequested(); });
    } else {
        std::cout << "\nNo streamable bridge available" << std::endl;
        std::cout << "Configure an entertainment area in the Philips Hue app"
                  << std::endl;
    }
}

void HueController::shutdown() {
    if (m_huestream) {
        std::cout << "\nShutting down HueStream library..." << std::endl;
        m_huestream->ShutDown();
        std::cout << "Shutdown completed!" << std::endl;
    }
}