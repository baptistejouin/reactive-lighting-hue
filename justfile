set dotenv-load
set export

default: build

# Configure + compile
build: configure
    cmake --build build -j$(sysctl -n hw.ncpu 2>/dev/null || nproc)

# CMake configure only
configure:
    cmake -S . -B build

# Run the app (auto-loads .env)
run:
    ./build/boiler-hue-edk

# Start the Hue simulator
simulator:
    cd ./build/_deps/philips_edk-src/tools/simulator && node server/main.js

# Full dev environment (simulator + app in parallel)
dev:
    just simulator & just run & wait

# Wipe build directory
clean:
    rm -rf build
