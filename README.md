# boiler-hue

Reactive Lighting System for Philips Hue Using the Hue Entertainment Development Kit (EDK), OpenCV, and NDI

**Objective:**
Develop a low-latency system that processes NDI video/audio inputs (e.g., from TouchDesigner) and converts them into dynamic lighting effects for Philips Hue devices, creating immersive, real-time experiences.

**Technical Stack:**
- Philips Hue EDK: Low-latency control of Hue lights.
- OpenCV: Real-time image analysis (color, motion, intensity).
- NDI: High-performance streaming over IP networks.

**Design Principles:**
- Plug-and-play integration.
- Optimized for low-latency synchronization.
- Modular and adaptable for custom applications.

## Install

### 1. Request Access to the EDK

Visit the Hue developer site and submit the access request form.  
Make sure to include your GitHub username so that repository cloning works during the build process.
Make sure you have configured correctly your GitHub SSH key on your device.

### 2. Install OpenCV

Install OpenCV through your preferred package manager.  
For macOS, an example using Homebrew:

```sh
brew install opencv
```

### 3. Install the NDI SDK

You can install the NDI SDK in one of two ways:

#### Option A — Homebrew (macOS)
```sh
brew install --cask libndi
```
after that, start the installation by opening the .pkg 

#### Option B — Direct Download
Download the SDK from the official NDI site and complete the required form.
https://ndi.video/for-developers/ndi-sdk/

After extracting, copy the `Include` headers and `libndi` library into `lib/`.

> NDI is excluded from the build by default. Re-enable it in `CMakeLists.txt` and `src/main.cpp` once the SDK is in place.

### 4. Install libidn2 (macOS only)

Required by curl (bundled in the EDK):

```sh
brew install libidn2
```

## Usage

This project uses [just](https://github.com/casey/just) as a command runner. Install it with:

```sh
brew install just
```

| Command | Description |
|---------|-------------|
| `just build` | Configure + compile (default) |
| `just run` | Run the app (auto-loads `.env`) |
| `just simulator` | Start the Hue light simulator |
| `just dev` | Run simulator + app in parallel |
| `just clean` | Wipe build directory |
| `just configure` | CMake configure only |

### Manual build

If you prefer not to use `just`:

```sh
cmake -S . -B build
cmake --build build
```

> In-source builds (`cmake .`) are not allowed — the build directory must be separate from the source root.

### Light simulator

The Hue EDK comes with a web simulator, useful for working with multiple lights and preparing compositions.

```sh
node ./build/_deps/philips_edk-src/tools/simulator/server/main.js
```
