cmake_minimum_required(VERSION 3.10)

# ---Guard against in-source builds---
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Run cmake from a build/ directory:\n  cmake -S . -B build")
endif()

# ---Project Configuration---
project(boiler-hue-edk LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*)
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*/ndi/.*") # NDI disabled
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE src)

# ---EDK Dependency---
include(FetchContent)
set(BUILD_CURL ON CACHE BOOL "Build Curl" FORCE)
set(BUILD_TEST OFF CACHE BOOL "Build tests" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "Build tools" FORCE)
set(BUILD_SWIG OFF CACHE BOOL "Build swig" FORCE)
set(BUILD_WRAPPERS OFF CACHE BOOL "Build wrappers" FORCE)
set(BUILD_TEST_COV OFF CACHE BOOL "Build tests with coverage" FORCE)
set(LIB_BUILD_MODE "STATIC" CACHE STRING "Library build mode" FORCE)
FetchContent_Declare(
    philips_edk
    GIT_REPOSITORY https://github.com/PhilipsHue/EDK.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    PATCH_COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=<SOURCE_DIR> -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PatchEDK.cmake
)
message(STATUS "Fetching Philips EDK...")
FetchContent_MakeAvailable(philips_edk)
target_include_directories(${PROJECT_NAME} PRIVATE ${philips_edk_SOURCE_DIR}/libhuestream/include)
target_link_libraries(${PROJECT_NAME} PRIVATE huestream)

# ---OpenCV Dependency---
find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

# ---Platform-Specific Libraries---
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 wsock32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework CoreFoundation"
        "-framework Security"
        "-framework SystemConfiguration"
        "-framework CFNetwork"
        "-framework Foundation"
    )
    find_library(IDN2_LIBRARY idn2)
    if(IDN2_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${IDN2_LIBRARY})
    else()
        message(FATAL_ERROR "libidn2 not found. Please install it before")
    endif()
    # curl (bundled in EDK) may be compiled with brotli/zstd support if found on the system
    find_library(BROTLI_LIBRARY NAMES brotlidec)
    find_library(ZSTD_LIBRARY NAMES zstd)
    if(BROTLI_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${BROTLI_LIBRARY})
    endif()
    if(ZSTD_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${ZSTD_LIBRARY})
    endif()
endif()
