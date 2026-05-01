#include "NDIController.h"
#include "../utils/SignalHandler.h"
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>

NDIController::NDIController()
    : m_findInstance(nullptr), m_recvInstance(nullptr),
      m_ndiInitialized(false) {}

NDIController::~NDIController() { shutdown(); }

bool NDIController::initialize(const std::string &sourceName) {
    m_sourceName = sourceName;

    std::cout << "Initializing NDI..." << std::endl;

    if (!NDIlib_initialize()) {
        std::cerr << "Failed to initialize NDI library" << std::endl;
        return false;
    }

    m_ndiInitialized = true;
    std::cout << "NDI library initialized successfully" << std::endl;

    return findSource(sourceName);
}

bool NDIController::findSource(const std::string &sourceName) {
    std::cout << "Searching for NDI sources..." << std::endl;

    NDIlib_find_create_t findDesc = {};
    findDesc.show_local_sources = true;
    findDesc.p_groups = nullptr;

    m_findInstance = NDIlib_find_create_v2(&findDesc);
    if (!m_findInstance) {
        std::cerr << "Failed to create NDI finder" << std::endl;
        return false;
    }

    // Wait for sources
    std::cout << "Waiting for NDI sources..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    uint32_t numSources = 0;
    const NDIlib_source_t *sources =
        NDIlib_find_get_current_sources(m_findInstance, &numSources);

    if (numSources == 0) {
        std::cerr << "No NDI sources found." << std::endl;
        return false;
    }

    std::cout << "Found " << numSources << " NDI source(s):" << std::endl;
    for (uint32_t i = 0; i < numSources; i++) {
        std::cout << "  [" << i << "] " << sources[i].p_ndi_name << std::endl;
    }

    // Let the user select a source
    int selectedIndex = -1;
    while (true) {
        std::cout << "\nEnter the number of the source to connect to (0-"
                  << numSources - 1 << "): ";
        if (!(std::cin >> selectedIndex) || selectedIndex < 0 ||
            selectedIndex >= static_cast<int>(numSources)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a valid number."
                      << std::endl;
        } else {
            break;
        }
    }

    const NDIlib_source_t *targetSource = &sources[selectedIndex];

    std::cout << "Connecting to: " << targetSource->p_ndi_name << std::endl;

    // Create receiver
    NDIlib_recv_create_v3_t recvDesc = {};
    recvDesc.source_to_connect_to = *targetSource;
    recvDesc.color_format = NDIlib_recv_color_format_BGRX_BGRA;
    recvDesc.bandwidth = NDIlib_recv_bandwidth_highest;

    m_recvInstance = NDIlib_recv_create_v3(&recvDesc);
    if (!m_recvInstance) {
        std::cerr << "Failed to create NDI receiver" << std::endl;
        return false;
    }

    std::cout << "Successfully connected to NDI source" << std::endl;
    return true;
}

void NDIController::processFrame() {
    NDIlib_video_frame_v2_t videoFrame;
    NDIlib_audio_frame_v2_t audioFrame;
    NDIlib_metadata_frame_t metadataFrame;

    switch (NDIlib_recv_capture_v2(m_recvInstance, &videoFrame, &audioFrame,
                                   &metadataFrame, 100)) {
    case NDIlib_frame_type_video:
        std::cout << "Video frame: " << videoFrame.xres << "x"
                  << videoFrame.yres << " @ " << videoFrame.frame_rate_N << "/"
                  << videoFrame.frame_rate_D << " fps" << std::endl;
        NDIlib_recv_free_video_v2(m_recvInstance, &videoFrame);
        break;

    case NDIlib_frame_type_audio:
        std::cout << "Audio frame: " << audioFrame.no_samples << " samples"
                  << std::endl;
        NDIlib_recv_free_audio_v2(m_recvInstance, &audioFrame);
        break;

    case NDIlib_frame_type_metadata:
        std::cout << "Metadata: " << metadataFrame.p_data << std::endl;
        NDIlib_recv_free_metadata(m_recvInstance, &metadataFrame);
        break;

    case NDIlib_frame_type_none:
        // No data
        break;

    case NDIlib_frame_type_error:
        std::cerr << "Error receiving frame" << std::endl;
        break;
    }
}

void NDIController::runDebugMode() {
    std::cout << "\n=== NDI Debug Mode Active ===" << std::endl;
    std::cout << "Source: " << m_sourceName << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    int frameCount = 0;
    while (!SignalHandler::isShutdownRequested()) {
        displayFrameWithAverageColor(); // Call the new function
        frameCount++;
        if (frameCount % 30 == 0) {
            std::cout << "Processed " << frameCount << " frames..."
                      << std::endl;
        }
    }
    std::cout << "\nTotal frames processed: " << frameCount << std::endl;
}

void NDIController::displayFrameWithAverageColor() {
    NDIlib_video_frame_v2_t videoFrame;
    switch (NDIlib_recv_capture_v2(m_recvInstance, &videoFrame, nullptr,
                                   nullptr, 100)) {
    case NDIlib_frame_type_video: {
        // Convert NDI frame to OpenCV Mat
        cv::Mat frame(videoFrame.yres, videoFrame.xres, CV_8UC4,
                      videoFrame.p_data);
        cv::cvtColor(frame, frame,
                     cv::COLOR_BGRA2BGR); // Convert BGRA to BGR for display

        // Calculate average color
        cv::Scalar avgColor = cv::mean(frame);
        double avgB = avgColor[0];
        double avgG = avgColor[1];
        double avgR = avgColor[2];

        // Overlay average color text on the frame
        std::string avgColorText =
            cv::format("Avg (B, G, R): (%.1f, %.1f, %.1f)", avgB, avgG, avgR);
        cv::putText(frame, avgColorText, cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255),
                    2);

        // Display the frame
        cv::imshow("NDI Video Stream", frame);
        cv::waitKey(1); // Required to update the window

        // Free the video frame
        NDIlib_recv_free_video_v2(m_recvInstance, &videoFrame);
        break;
    }
    case NDIlib_frame_type_none:
        // No data
        break;
    case NDIlib_frame_type_error:
        std::cerr << "Error receiving frame" << std::endl;
        break;
    default:
        // Ignore audio/metadata
        break;
    }
}

void NDIController::shutdown() {
    if (m_recvInstance) {
        std::cout << "Destroying NDI receiver..." << std::endl;
        NDIlib_recv_destroy(m_recvInstance);
        m_recvInstance = nullptr;
    }

    if (m_findInstance) {
        NDIlib_find_destroy(m_findInstance);
        m_findInstance = nullptr;
    }

    if (m_ndiInitialized) {
        NDIlib_destroy();
        m_ndiInitialized = false;
    }

    std::cout << "NDI shutdown complete" << std::endl;
}