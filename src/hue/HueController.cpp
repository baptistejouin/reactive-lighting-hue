#include <cstdlib>
#include <hue/HueController.h>
#include <iostream>
#include <memory>
#include <support/network/NetworkConfiguration.h>
#include <utils/SignalHandler.h>

HueController::HueController() {}

HueController::~HueController() { shutdown(); }

bool HueController::initialize() {
    try {
        std::cout << "Setting up HueStream library..." << std::endl;

        _config = std::make_shared<huestream::Config>(
            "LightEffects", "MacBookARM",
            huestream::PersistenceEncryptionKey(
                std::getenv("HUE_ENCRYPTION_KEY")));
        _config->SetStreamingMode(huestream::STREAMING_MODE_UDP);

        _huestream = std::make_shared<huestream::HueStream>(_config);

        _setupFeedbackCallback();

        std::cout << "HueStream library initialized" << std::endl;

        _connectToBridge();

        return _huestream->IsStreamableBridgeLoaded() ||
               _huestream->IsBridgeStreaming();

    } catch (const std::exception &e) {
        std::cerr << "Failed to initialize Hue controller: " << e.what()
                  << std::endl;
        return false;
    }
}

void HueController::_setupFeedbackCallback() {
    _huestream->RegisterFeedbackCallback(
        [](const huestream::FeedbackMessage &message) {
            std::cout << "[" << message.GetId() << "] " << message.GetTag()
                      << std::endl;

            if (message.GetId() ==
                huestream::FeedbackMessage::ID_DONE_COMPLETED) {
                std::cout << "Connected to bridge with ip: "
                          << message.GetBridge()->GetIpAddress() << std::endl;
            }

            if (message.GetMessageType() ==
                huestream::FeedbackMessage::FEEDBACK_TYPE_USER) {
                std::cout << message.GetUserMessage() << std::endl;
            }
        });
}

void HueController::_connectToBridge() {
    std::cout << "\nConnecting to bridge..." << std::endl;

    auto bridge =
        std::make_shared<huestream::Bridge>(_config->GetBridgeSettings());
    bridge->SetIpAddress("127.0.0.1");
    bridge->SetUser("aSimulatedUser");
    bridge->SetClientKey("01234567890123456789012345678901");
    _huestream->ConnectManualBridgeInfo(bridge);

    while (!_huestream->IsBridgeStreaming() &&
           !SignalHandler::isShutdownRequested()) {

        auto loadedBridge = _huestream->GetLoadedBridge();
        std::cout << "Bridge status: " << loadedBridge->GetStatusTag()
                  << std::endl;

        if (loadedBridge->GetStatus() ==
            huestream::BRIDGE_INVALID_GROUP_SELECTED) {
            std::cout << "Selecting first entertainment group..." << std::endl;
            _huestream->SelectGroup(loadedBridge->GetGroups()->at(0));

        } else if (loadedBridge->GetStatus() != huestream::BRIDGE_READY &&
                   loadedBridge->GetStatus() != huestream::BRIDGE_STREAMING) {
            std::cout << "No streamable bridge configured: "
                      << loadedBridge->GetStatusTag() << std::endl;
            std::cout << "Press Enter to retry..." << std::endl;
            std::cin.get();
            if (SignalHandler::isShutdownRequested())
                break;
            _huestream->ConnectManualBridgeInfo(bridge);
        }
    }

    if (!SignalHandler::isShutdownRequested()) {
        std::cout << "Bridge connection completed!" << std::endl;
    }
}

std::shared_ptr<huestream::HueStream> HueController::getHueStream() {
    return _huestream;
};

void HueController::shutdown() {
    if (_huestream) {
        std::cout << "\nShutting down HueStream library..." << std::endl;
        _huestream->ShutDown();
        std::cout << "Shutdown completed!" << std::endl;
    }
}
