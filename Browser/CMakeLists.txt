cmake_minimum_required(VERSION 3.16)
project(SecureBrowser VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure for MSVC
if(MSVC)
    add_compile_options(/W4 /WX /EHsc /std:c++20)
    # Security options
    add_compile_options(/guard:cf /sdl /Qspectre)
    add_link_options(/NXCOMPAT /DYNAMICBASE /CETCOMPAT /GUARD:CF)
endif()

# Download and Setup CEF for Windows x64 via CMake
include(FetchContent)

# Define CEF Version and Download URL
set(CEF_VERSION "120.2.7")
set(CEF_ARCHIVE "cef_binary_${CEF_VERSION}+ge8b92b6+chromium-120.0.6099.234_windows64")
set(CEF_URL "https://cef-builds.spotifycdn.com/${CEF_ARCHIVE}.tar.bz2")

# In a real environment (Windows x64):
# We would uncomment this to download the 1GB CEF binary release, but this is a sandbox.
# FetchContent_Declare(
#     cef
#     URL ${CEF_URL}
# )
# FetchContent_MakeAvailable(cef)

if(WIN32)
    add_compile_definitions(HAS_CEF)
    # Include CEF headers from the downloaded location
    # include_directories(${cef_SOURCE_DIR})
endif()

add_subdirectory(src)
