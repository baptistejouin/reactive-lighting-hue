#include "effects/BezierFadeEffect.h"
#include "hue/HueController.h"
// #include "ndi/NDIController.h"
#include "hue/Scene.h"
#include "utils/SignalHandler.h"
#include <csignal>
#include <iostream>
#include <memory>

void displayMainMenu() {
    std::cout << "\n================================" << std::endl;
    std::cout << "  Hue Light Control System" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "Choose debug mode:" << std::endl;
    std::cout << "1. Hue Debug (Light Fading)" << std::endl;
    // std::cout << "2. NDI Debug (TouchDesigner)" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "\nEnter your choice: ";
}

int main(int argc, char *argv[]) {
    // Register signal handler for Ctrl+C
    SignalHandler::setup();
    HueController hueController;
    bool initialized = false;

    int choice = -1;

    while (choice != 0) {
        displayMainMenu();

        if (!(std::cin >> choice)) {
            std::cin.clear();
            if (SignalHandler::isShutdownRequested())
                break;
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }

        std::cin.ignore(10000, '\n'); // Clear input buffer

        try {
            switch (choice) {
            case 1: {
                std::cout << "\n=== Starting Hue Debug Mode ===" << std::endl;
                if (!initialized) {
                    initialized = hueController.initialize();
                }

                if (initialized) {
                    auto huestream = hueController.getHueStream();
                    auto bezierEffect = std::make_shared<BezierFadeEffect>();
                    Scene scene(huestream, "Debug Scene", 0);
                    auto mainGroup =
                        huestream->GetLoadedBridge()->GetGroups()->at(0);
                    auto lights = mainGroup->GetLights();
                    if (!lights || lights->empty()) {
                        std::cerr << "The lights is empty or not valid"
                                  << std::endl;
                        break;
                    }
                    // get randomly 5 lights from the group (not need to be a
                    // solid random selection, just for testing)
                    std::srand(static_cast<unsigned int>(std::time(nullptr)));

                    std::vector<std::string> lightIds;

                    for (size_t i = 0; i < lights->size() && i < 5; i++) {
                        int randomIndex = std::rand() % lights->size();
                        lightIds.push_back(lights->at(randomIndex)->GetId());
                    }
                    scene.addBinding(lightIds, bezierEffect, 8000, true);
                    scene.run(
                        []() { return SignalHandler::isShutdownRequested(); });
                }

                break;
            }

                // case 2: {
                //     std::cout << "\n=== Starting NDI Debug Mode ===" <<
                //     std::endl; NDIController ndiController; if
                //     (ndiController.initialize("TouchDesigner")) {
                //         ndiController.runDebugMode();
                //     }
                //     break;
                // }

            case 0:
                std::cout << "\nExiting application..." << std::endl;
                break;

            default:
                std::cout << "Invalid choice. Please select 1, 2, or 0."
                          << std::endl;
                break;
            }

            if (choice != 0 && !SignalHandler::isShutdownRequested()) {
                std::cout << "\nPress Enter to return to main menu...";
                std::cin.get();
            }

        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }

        if (SignalHandler::isShutdownRequested()) {
            std::cout << "\nShutdown requested. Exiting..." << std::endl;
            break;
        }
    }

    std::cout << "Application terminated." << std::endl;
    return 0;
}
