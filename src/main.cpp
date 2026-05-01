#include "effects/FadeEffect.h"
#include "effects/IEffect.h"
#include "hue/HueController.h"
// #include "ndi/NDIController.h"
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

    int choice = -1;

    while (choice != 0) {
        displayMainMenu();

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }

        std::cin.ignore(10000, '\n'); // Clear input buffer

        try {
            switch (choice) {
            case 1: {
                std::cout << "\n=== Starting Hue Debug Mode ===" << std::endl;
                HueController hueController;
                if (hueController.initialize()) {
                    auto huestream = hueController.getHueStream();
                    auto fade = std::make_shared<FadeEffect>(huestream);
                    hueController.runEffect(fade);
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
