# Patch 1: Append CMAKE_POLICY_VERSION_MINIMUM=3.5 to COMMON_ARGS in ExternalProjectUtils.cmake.
# This propagates the flag to all ExternalProject_Add calls in the EDK (googletest, edtls,
# mbedtls, curl, etc.) whose bundled CMakeLists.txt use cmake_minimum_required < 3.5,
# which is no longer supported by modern CMake.
set(FILE "${SOURCE_DIR}/cmake/ExternalProjectUtils.cmake")
file(READ "${FILE}" CONTENT)
string(REPLACE
    [[-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})]]
    [[-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_POLICY_VERSION_MINIMUM=3.5)]]
    PATCHED "${CONTENT}"
)
file(WRITE "${FILE}" "${PATCHED}")

# Patch 2: Disable brotli and zstd in the curl build.
# curl auto-detects these from the system (Homebrew), but they are not propagated
# to the final link step, causing undefined symbol errors.
set(FILE "${SOURCE_DIR}/cmake/ExternalCurl.cmake")
file(READ "${FILE}" CONTENT)
string(REPLACE
    [[-DBUILD_TESTING=OFF]]
    [[-DBUILD_TESTING=OFF -DCURL_BROTLI=OFF -DCURL_ZSTD=OFF]]
    PATCHED "${CONTENT}"
)
file(WRITE "${FILE}" "${PATCHED}")
