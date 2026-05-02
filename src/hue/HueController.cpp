#include "HueController.h"
#include "../effects/IEffect.h"
#include "../utils/SignalHandler.h"
#include <support/network/NetworkConfiguration.h>
#include <iostream>
#include <memory>

HueController::HueController() {}

HueController::~HueController() { shutdown(); }

bool HueController::initialize() {
    try {
        std::cout << "Setting up HueStream library..." << std::endl;

        m_config = std::make_shared<Config>(
            "LightEffects", "MacBookARM",
            PersistenceEncryptionKey("secret_key")); // TODO
        m_config->SetStreamingMode(STREAMING_MODE_UDP);

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
        // std::cout << "[" << message.GetId() << "] " << message.GetTag()
        //           << std::endl;

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

    auto bridge = std::make_shared<Bridge>(m_config->GetBridgeSettings());
    // bridge->SetIpAddress("192.168.0.132");
    bridge->SetIpAddress("127.0.0.1");
    bridge->SetUser("aSimulatedUser");
    bridge->SetClientKey("01234567890123456789012345678901");
    m_huestream->ConnectManualBridgeInfo(bridge);

    while (!m_huestream->IsBridgeStreaming() &&
           !SignalHandler::isShutdownRequested()) {

        auto loadedBridge = m_huestream->GetLoadedBridge();
        std::cout << "Bridge status: " << loadedBridge->GetStatusTag() << std::endl;

        if (loadedBridge->GetStatus() == BRIDGE_INVALID_GROUP_SELECTED) {
            std::cout << "Selecting first entertainment group..." << std::endl;
            m_huestream->SelectGroup(loadedBridge->GetGroups()->at(0));

        } else if (loadedBridge->GetStatus() != BRIDGE_READY &&
                   loadedBridge->GetStatus() != BRIDGE_STREAMING) {
            std::cout << "No streamable bridge configured: "
                      << loadedBridge->GetStatusTag() << std::endl;
            std::cout << "Press Enter to retry..." << std::endl;
            std::cin.get();
            if (SignalHandler::isShutdownRequested()) break;
            m_huestream->ConnectManualBridgeInfo(bridge);
        }
    }

    if (!SignalHandler::isShutdownRequested()) {
        std::cout << "Bridge connection completed!" << std::endl;
    }
}

std::shared_ptr<HueStream> HueController::getHueStream() {
    return m_huestream;
};

void HueController::runEffect(std::shared_ptr<IEffect> effect) {
    effect->play([]() { return SignalHandler::isShutdownRequested(); });
}

void HueController::shutdown() {
    if (m_huestream) {
        std::cout << "\nShutting down HueStream library..." << std::endl;
        m_huestream->ShutDown();
        std::cout << "Shutdown completed!" << std::endl;
    }
}
