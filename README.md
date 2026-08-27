# SecureBrowser (CEF)

A production-ready, highly secure Windows browser shell built around the Chromium Embedded Framework (CEF) using C++.

## Core Features Implemented

*   **Custom HTML/CSS/JS UI:** A custom-built UI frontend that communicates strictly via secure IPC (`CefMessageRouter`) with the native C++ backend.
*   **Strict Security & Sandboxing:** Compiled with MSVC `/guard:cf`, `/sdl`, and `/Qspectre`. CEF Sandbox is enforced. Hardware acceleration runs in an isolated GPU sandbox.
*   **DPAPI Transparent Encryption:** User history, bookmarks, and settings are written to disk using Windows CryptAPI (`CryptProtectData`). Data is permanently tied to the specific Windows user profile and cannot be decrypted externally.
*   **Secure Navigation & Tab Management:** URL parsers block dangerous schemes (`javascript:`, `file:`, `data:`). Dynamic C++ tab management natively routes requests to the correct CEF browser instances.
*   **Secure Downloads Manager:** Automatically intercepts downloads and strictly blocks the saving of executable or potentially malicious files (`.exe`, `.bat`, `.vbs`, etc.).
*   **Permissions Management (Default Deny):** Proactively blocks hidden camera, microphone, and geolocation access requests from websites.
*   **Session Persistence:** Cookies and login sessions persist across restarts via the CEF `cache_path`.

## Build Instructions (Windows 10/11 x64)

### Requirements
*   Visual Studio 2022 (with Desktop Development with C++ workload)
*   CMake 3.16+
*   NSIS (Nullsoft Scriptable Install System) for `.exe` packaging.

### Compiling and Packaging
To compile the C++ binaries and build the final executable installer:
1. Open the Developer Command Prompt for Visual Studio.
2. Run the automated build script:
   `.\build_windows_release.bat`

This script will run CMake, build the Release binary, and invoke NSIS to output `SecureBrowser_Setup_x64.exe` in the `Browser/packaging/` folder.
